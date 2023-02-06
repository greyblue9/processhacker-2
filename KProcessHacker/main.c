/*
 * KProcessHacker
 *
 * Copyright (C) 2010-2011 wj32
 *
 * This file is part of Process Hacker.
 *
 * Process Hacker is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Process Hacker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Process Hacker.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <kph.h>
#include <dyndata.h>

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;
__drv_dispatchType(IRP_MJ_CREATE) DRIVER_DISPATCH KphDispatchCreate;

ULONG KphpReadIntegerParameter(
    __in_opt HANDLE KeyHandle,
    __in PUNICODE_STRING ValueName,
    __in ULONG DefaultValue
    );

NTSTATUS KphpReadDriverParameters(
    __in PUNICODE_STRING RegistryPath
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, DriverEntry)
#pragma alloc_text(PAGE, DriverUnload)
#pragma alloc_text(PAGE, KphpReadIntegerParameter)
#pragma alloc_text(PAGE, KphpReadDriverParameters)
#pragma alloc_text(PAGE, KpiGetFeatures)
#endif

PDRIVER_OBJECT KphDriverObject;
PDEVICE_OBJECT KphDeviceObject;
ULONG KphFeatures;
KPH_PARAMETERS KphParameters;

NTSTATUS DriverEntry(
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS status;
    UNICODE_STRING deviceName;
    PDEVICE_OBJECT deviceObject;

    PAGED_CODE();

    KphDriverObject = DriverObject;

    if (!NT_SUCCESS(status = KphDynamicDataInitialization()))
        return status;

    KphDynamicImport();

    if (!NT_SUCCESS(status = KphpReadDriverParameters(RegistryPath)))
        return status;

    // Create the device.

    RtlInitUnicodeString(&deviceName, KPH_DEVICE_NAME);

    status = IoCreateDevice(
        DriverObject,
        0,
        &deviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &deviceObject
        );

    if (!NT_SUCCESS(status))
        return status;

    KphDeviceObject = deviceObject;

    // Set up I/O.

    DriverObject->MajorFunction[IRP_MJ_CREATE] = KphDispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KphDispatchDeviceControl;
    DriverObject->DriverUnload = DriverUnload;

    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    dprintf("Driver loaded\n");

    return status;
}

VOID DriverUnload(
    __in PDRIVER_OBJECT DriverObject
    )
{
    PAGED_CODE();

    IoDeleteDevice(KphDeviceObject);

    dprintf("Driver unloaded\n");
}

NTSTATUS KphDispatchCreate(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION stackLocation;
    PIO_SECURITY_CONTEXT securityContext;

    stackLocation = IoGetCurrentIrpStackLocation(Irp);
    securityContext = stackLocation->Parameters.Create.SecurityContext;

    dprintf("Client (PID %Iu) is connecting\n", PsGetCurrentProcessId());

    if (KphParameters.SecurityLevel == KphSecurityPrivilegeCheck)
    {
        UCHAR requiredPrivilegesBuffer[FIELD_OFFSET(PRIVILEGE_SET, Privilege) + sizeof(LUID_AND_ATTRIBUTES)];
        PPRIVILEGE_SET requiredPrivileges;

        // Check for SeDebugPrivilege.

        requiredPrivileges = (PPRIVILEGE_SET)requiredPrivilegesBuffer;
        requiredPrivileges->PrivilegeCount = 1;
        requiredPrivileges->Control = PRIVILEGE_SET_ALL_NECESSARY;
        requiredPrivileges->Privilege[0].Luid.LowPart = SE_DEBUG_PRIVILEGE;
        requiredPrivileges->Privilege[0].Luid.HighPart = 0;
        requiredPrivileges->Privilege[0].Attributes = 0;

        if (!SePrivilegeCheck(
            requiredPrivileges,
            &securityContext->AccessState->SubjectSecurityContext,
            Irp->RequestorMode
            ))
        {
            status = STATUS_PRIVILEGE_NOT_HELD;
            dprintf("Client (PID %Iu) was rejected\n", PsGetCurrentProcessId());
        }
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

/**
 * Reads an integer (REG_DWORD) parameter from the registry.
 *
 * \param KeyHandle A handle to the Parameters key. If NULL, the function
 * fails immediately and returns \a DefaultValue.
 * \param ValueName The name of the parameter.
 * \param DefaultValue The value that is returned if the function fails
 * to retrieve the parameter from the registry.
 *
 * \return The parameter value, or \a DefaultValue if the function failed.
 */
ULONG KphpReadIntegerParameter(
    __in_opt HANDLE KeyHandle,
    __in PUNICODE_STRING ValueName,
    __in ULONG DefaultValue
    )
{
    NTSTATUS status;
    UCHAR buffer[FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) + sizeof(ULONG)];
    PKEY_VALUE_PARTIAL_INFORMATION info;
    ULONG resultLength;

    PAGED_CODE();

    if (!KeyHandle)
        return DefaultValue;

    info = (PKEY_VALUE_PARTIAL_INFORMATION)buffer;

    status = ZwQueryValueKey(
        KeyHandle,
        ValueName,
        KeyValuePartialInformation,
        info,
        sizeof(buffer),
        &resultLength
        );

    if (info->Type != REG_DWORD)
        status = STATUS_OBJECT_TYPE_MISMATCH;

    if (!NT_SUCCESS(status))
    {
        dprintf("Unable to query parameter %.*S: 0x%x\n", ValueName->Length / sizeof(WCHAR), ValueName->Buffer, status);
        return DefaultValue;
    }

    return *(PULONG)info->Data;
}

/**
 * Reads the driver parameters.
 *
 * \param RegistryPath The registry path of the driver.
 */
NTSTATUS KphpReadDriverParameters(
    __in PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS status;
    HANDLE parametersKeyHandle;
    UNICODE_STRING parametersString;
    UNICODE_STRING parametersKeyName;
    OBJECT_ATTRIBUTES objectAttributes;
    UNICODE_STRING valueName;

    PAGED_CODE();

    // Open the Parameters key.

    RtlInitUnicodeString(&parametersString, L"\\Parameters");

    parametersKeyName.Length = RegistryPath->Length + parametersString.Length;
    parametersKeyName.MaximumLength = parametersKeyName.Length;
    parametersKeyName.Buffer = ExAllocatePoolWithTag(PagedPool, parametersKeyName.MaximumLength, 'ThpK');

    if (!parametersKeyName.Buffer)
        return STATUS_INSUFFICIENT_RESOURCES;

    memcpy(parametersKeyName.Buffer, RegistryPath->Buffer, RegistryPath->Length);
    memcpy(&parametersKeyName.Buffer[RegistryPath->Length / sizeof(WCHAR)], parametersString.Buffer, parametersString.Length);

    InitializeObjectAttributes(
        &objectAttributes,
        &parametersKeyName,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL,
        NULL
        );
    status = ZwOpenKey(
        &parametersKeyHandle,
        KEY_READ,
        &objectAttributes
        );
    ExFreePoolWithTag(parametersKeyName.Buffer, 'ThpK');

    if (!NT_SUCCESS(status))
    {
        dprintf("Unable to open Parameters key: 0x%x\n", status);
        status = STATUS_SUCCESS;
        parametersKeyHandle = NULL;
        // Continue so we can set up defaults.
    }

    // Read in the parameters.

    RtlInitUnicodeString(&valueName, L"SecurityLevel");
    KphParameters.SecurityLevel = KphpReadIntegerParameter(parametersKeyHandle, &valueName, KphSecurityPrivilegeCheck);

    RtlInitUnicodeString(&valueName, L"DisableDynamicProcedureScan");
    KphParameters.DisableDynamicProcedureScan = KphpReadIntegerParameter(parametersKeyHandle, &valueName, FALSE);

    KphReadDynamicDataParameters(parametersKeyHandle);

    if (parametersKeyHandle)
        ZwClose(parametersKeyHandle);

    return status;
}

NTSTATUS KpiGetFeatures(
    __out PULONG Features,
    __in KPROCESSOR_MODE AccessMode
    )
{
    PAGED_CODE();

    if (AccessMode != KernelMode)
    {
        __try
        {
            ProbeForWrite(Features, sizeof(ULONG), sizeof(ULONG));
            *Features = KphFeatures;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return GetExceptionCode();
        }
    }
    else
    {
        *Features = KphFeatures;
    }

    return STATUS_SUCCESS;
}

/**
 * Enumerates the modules loaded by the kernel.
 *
 * \param Modules A variable which receives a pointer
 * to a structure containing information about
 * the kernel modules. The structure must be freed with
 * the tag 'ThpK'.
 */
NTSTATUS KphEnumerateSystemModules(
    __out PRTL_PROCESS_MODULES *Modules
    )
{
    NTSTATUS status;
    PVOID buffer;
    ULONG bufferSize;
    ULONG attempts;

    bufferSize = 2048;
    attempts = 8;

    do
    {
        buffer = ExAllocatePoolWithTag(PagedPool, bufferSize, 'ThpK');

        if (!buffer)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        status = ZwQuerySystemInformation(
            SystemModuleInformation,
            buffer,
            bufferSize,
            &bufferSize
            );

        if (NT_SUCCESS(status))
        {
            *Modules = buffer;

            return status;
        }

        ExFreePoolWithTag(buffer, 'ThpK');

        if (status != STATUS_INFO_LENGTH_MISMATCH)
        {
            break;
        }
    } while (--attempts);

    return status;
}

/**
 * Checks if an address range lies within a kernel module.
 *
 * \param Address The beginning of the address range.
 * \param Length The number of bytes in the address range.
 */
NTSTATUS KphValidateAddressForSystemModules(
    __in PVOID Address,
    __in SIZE_T Length
    )
{
    NTSTATUS status;
    PRTL_PROCESS_MODULES modules;
    ULONG i;
    BOOLEAN valid;

    status = KphEnumerateSystemModules(&modules);

    if (!NT_SUCCESS(status))
        return status;

    valid = FALSE;

    for (i = 0; i < modules->NumberOfModules; i++)
    {
        if (
            (ULONG_PTR)Address + Length >= (ULONG_PTR)Address &&
            (ULONG_PTR)Address >= (ULONG_PTR)modules->Modules[i].ImageBase &&
            (ULONG_PTR)Address + Length <= (ULONG_PTR)modules->Modules[i].ImageBase + modules->Modules[i].ImageSize
            )
        {
            dprintf("Validated address 0x%Ix in %s\n", Address, modules->Modules[i].FullPathName);
            valid = TRUE;
            break;
        }
    }

    ExFreePoolWithTag(modules, 'ThpK');

    if (valid)
        status = STATUS_SUCCESS;
    else
        status = STATUS_ACCESS_VIOLATION;

    return status;
}
