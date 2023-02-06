/*
 * KProcessHacker
 *
 * Copyright (C) 2010-2015 wj32
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
#define _DYNDATA_PRIVATE
#include <dyndata.h>

#define INIT_SCAN(scan, bytes, length, address, scanLength, displacement) \
    ( \
    ((scan)->Initialized = TRUE), \
    ((scan)->Scanned = FALSE), \
    ((scan)->Bytes = (bytes)), \
    ((scan)->Length = (length)), \
    ((scan)->StartAddress = (address)), \
    ((scan)->ScanLength = (scanLength)), \
    ((scan)->Displacement = (displacement)), \
    ((scan)->ProcedureAddress = NULL), \
    bytes \
    )
#define C_2sTo4(x) ((unsigned int)(signed short)(x))

NTSTATUS KphpLoadDynamicConfiguration(
    __in PVOID Buffer,
    __in ULONG Length
    );

#ifdef _X86_

NTSTATUS KphpX86DataInitialization(
    VOID
    );

#else

NTSTATUS KphpAmd64DataInitialization(
    VOID
    );

#endif

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, KphDynamicDataInitialization)
#pragma alloc_text(PAGE, KphReadDynamicDataParameters)
#pragma alloc_text(PAGE, KphpLoadDynamicConfiguration)
#ifdef _X86_
#pragma alloc_text(PAGE, KphpX86DataInitialization)
#else
#pragma alloc_text(PAGE, KphpAmd64DataInitialization)
#endif
#endif

#ifdef _X86_

// x86

// PsTerminateProcess/PspTerminateProcess
static UCHAR PspTerminateProcess51Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x56, 0x64, 0xa1,
    0x24, 0x01, 0x00, 0x00, 0x8b, 0x75, 0x08, 0x3b
};
static UCHAR PspTerminateProcess52Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x56, 0x8b, 0x75,
    0x08, 0x57, 0x8d, 0xbe, 0x40, 0x02, 0x00, 0x00
};
static UCHAR PsTerminateProcess60Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x53, 0x56, 0x57,
    0x33, 0xd2, 0x6a, 0x08, 0x42, 0x5e, 0x8d, 0xb9
};
static UCHAR PsTerminateProcess61Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x51, 0x51, 0x53,
    0x56, 0x64, 0x8b, 0x35, 0x24, 0x01, 0x00, 0x00,
    0x66, 0xff, 0x8e, 0x84, 0x00, 0x00, 0x00, 0x57,
    0xc7, 0x45, 0xfc
}; // a lot of functions seem to share the first
   // 16 bytes of the Windows 7 PsTerminateProcess,
   // and a few even share the first 24 bytes.
static UCHAR PsTerminateProcess62Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x51, 0x53, 0x64,
    0x8b, 0x1d, 0x24, 0x01, 0x00, 0x00, 0x56, 0x8d,
    0xb3, 0x3c, 0x01, 0x00, 0x00, 0x66, 0xff, 0x0e
};
static UCHAR PsTerminateProcess63Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x83, 0xe4, 0xf8,
    0x56, 0x64, 0x8b, 0x35, 0x24, 0x01, 0x00, 0x00,
    0x57, 0x66, 0xff, 0x8e, 0x3c, 0x01, 0x00, 0x00
};

// PspTerminateThreadByPointer
static UCHAR PspTerminateThreadByPointer51Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x83, 0xec, 0x0c,
    0x83, 0x4d, 0xf8, 0xff, 0x56, 0x57, 0x8b, 0x7d
};
static UCHAR PspTerminateThreadByPointer52Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x53, 0x56, 0x57,
    0x8b, 0x7d, 0x08, 0x8d, 0xb7, 0x40, 0x02, 0x00
};
static UCHAR PspTerminateThreadByPointer60Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x83, 0xe4, 0xf8,
    0x51, 0x53, 0x56, 0x8b, 0x75, 0x08, 0x57, 0x8d,
    0xbe, 0x60, 0x02, 0x00, 0x00, 0xf6, 0x07, 0x40
};
static UCHAR PspTerminateThreadByPointer61Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x83, 0xe4, 0xf8,
    0x51, 0x53, 0x56, 0x8b, 0x75, 0x08, 0x57, 0x8d,
    0xbe, 0x80, 0x02, 0x00, 0x00, 0xf6, 0x07, 0x40
};
static UCHAR PspTerminateThreadByPointer62Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x8d, 0x87, 0x68,
    0x02, 0x00, 0x00, 0xf6, 0x00, 0x20, 0x53, 0x8a
};
static UCHAR PspTerminateThreadByPointer63Bytes[] =
{
    0x8b, 0xff, 0x55, 0x8b, 0xec, 0x53, 0x56, 0x8b,
    0xf1, 0x8b, 0xda, 0x57, 0x8d, 0xbe, 0xb8, 0x03
};

#endif

NTSTATUS KphDynamicDataInitialization(
    VOID
    )
{
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    // Get Windows version information.

    KphDynOsVersionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
    status = RtlGetVersion((PRTL_OSVERSIONINFOW)&KphDynOsVersionInfo);

    if (!NT_SUCCESS(status))
        return status;

    // Dynamic data should be read from the registry, but we use the fallback
    // function here if needed.
    if (KphDynNtVersion == 0)
    {
#ifdef _X86_
        KphpX86DataInitialization();
#else
        KphpAmd64DataInitialization();
#endif
    }

    return status;
}

NTSTATUS KphReadDynamicDataParameters(
    __in_opt HANDLE KeyHandle
    )
{
    NTSTATUS status;
    UNICODE_STRING valueName;
    PKEY_VALUE_PARTIAL_INFORMATION info;
    ULONG resultLength;

    PAGED_CODE();

    if (!KeyHandle)
        return STATUS_UNSUCCESSFUL;

    RtlInitUnicodeString(&valueName, L"DynamicConfiguration");

    status = ZwQueryValueKey(
        KeyHandle,
        &valueName,
        KeyValuePartialInformation,
        NULL,
        0,
        &resultLength
        );

    if (status != STATUS_BUFFER_OVERFLOW && status != STATUS_BUFFER_TOO_SMALL)
    {
        // Unexpected status; fail now.
        return STATUS_UNSUCCESSFUL;
    }

    info = ExAllocatePoolWithTag(PagedPool, resultLength, 'ThpK');

    if (!info)
        return STATUS_INSUFFICIENT_RESOURCES;

    status = ZwQueryValueKey(
        KeyHandle,
        &valueName,
        KeyValuePartialInformation,
        info,
        resultLength,
        &resultLength
        );

    if (NT_SUCCESS(status))
    {
        if (info->Type == REG_BINARY)
            status = KphpLoadDynamicConfiguration(info->Data, info->DataLength);
        else
            status = STATUS_OBJECT_TYPE_MISMATCH;

        if (!NT_SUCCESS(status))
            dprintf("Unable to load dynamic configuration: 0x%x\n", status);
    }

    ExFreePoolWithTag(info, 'ThpK');

    return status;
}

NTSTATUS KphpLoadDynamicConfiguration(
    __in PVOID Buffer,
    __in ULONG Length
    )
{
    PKPH_DYN_CONFIGURATION config;
    ULONG i;
    PKPH_DYN_PACKAGE package;

    PAGED_CODE();

    config = Buffer;

    if (Length < FIELD_OFFSET(KPH_DYN_CONFIGURATION, Packages))
        return STATUS_INVALID_PARAMETER;
    if (config->Version != KPH_DYN_CONFIGURATION_VERSION)
        return STATUS_INVALID_PARAMETER;
    if (config->NumberOfPackages > KPH_DYN_MAXIMUM_PACKAGES)
        return STATUS_INVALID_PARAMETER;
    if (Length < FIELD_OFFSET(KPH_DYN_CONFIGURATION, Packages) + config->NumberOfPackages * sizeof(KPH_DYN_PACKAGE))
        return STATUS_INVALID_PARAMETER;

    dprintf("Loading dynamic configuration with %u package(s)\n", config->NumberOfPackages);

    for (i = 0; i < config->NumberOfPackages; i++)
    {
        package = &config->Packages[i];

        if (package->MajorVersion == KphDynOsVersionInfo.dwMajorVersion &&
            package->MinorVersion == KphDynOsVersionInfo.dwMinorVersion &&
            (package->ServicePackMajor == (USHORT)-1 || package->ServicePackMajor == KphDynOsVersionInfo.wServicePackMajor) &&
            (package->BuildNumber == (USHORT)-1 || package->BuildNumber == KphDynOsVersionInfo.dwBuildNumber))
        {
            dprintf("Found matching package at index %u for Windows %u.%u\n", i, package->MajorVersion, package->MinorVersion);

            KphDynNtVersion = package->ResultingNtVersion;

            KphDynEgeGuid = C_2sTo4(package->StructData.EgeGuid);
            KphDynEpObjectTable = C_2sTo4(package->StructData.EpObjectTable);
            KphDynEpRundownProtect = C_2sTo4(package->StructData.EpRundownProtect);
            KphDynEreGuidEntry = C_2sTo4(package->StructData.EreGuidEntry);
            KphDynHtHandleContentionEvent = C_2sTo4(package->StructData.HtHandleContentionEvent);
            KphDynOtName = C_2sTo4(package->StructData.OtName);
            KphDynOtIndex = C_2sTo4(package->StructData.OtIndex);
            KphDynObDecodeShift = C_2sTo4(package->StructData.ObDecodeShift);
            KphDynObAttributesShift = C_2sTo4(package->StructData.ObAttributesShift);

            return STATUS_SUCCESS;
        }
    }

    return STATUS_NOT_FOUND;
}

#ifdef _X86_

static NTSTATUS KphpX86DataInitialization(
    VOID
    )
{
    ULONG majorVersion, minorVersion, servicePack, buildNumber;

    PAGED_CODE();

    majorVersion = KphDynOsVersionInfo.dwMajorVersion;
    minorVersion = KphDynOsVersionInfo.dwMinorVersion;
    servicePack = KphDynOsVersionInfo.wServicePackMajor;
    buildNumber = KphDynOsVersionInfo.dwBuildNumber;
    dprintf("Windows %d.%d, SP%d.%d, build %d\n",
        majorVersion, minorVersion, servicePack,
        KphDynOsVersionInfo.wServicePackMinor, buildNumber
        );

    // Windows XP
    if (majorVersion == 5 && minorVersion == 1)
    {
        ULONG_PTR searchOffset = (ULONG_PTR)KphGetSystemRoutineAddress(L"NtClose");
        ULONG scanLength = 0x100000;

        KphDynNtVersion = PHNT_WINXP;

        // Windows XP SP0 and 1 are not supported
        if (servicePack == 0)
        {
            return STATUS_NOT_SUPPORTED;
        }
        else if (servicePack == 1)
        {
            return STATUS_NOT_SUPPORTED;
        }
        else if (servicePack == 2)
        {
        }
        else if (servicePack == 3)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }

        KphDynEpObjectTable = 0xc4;
        KphDynEpRundownProtect = 0x80;
        KphDynOtName = 0x40;
        KphDynOtIndex = 0x4c;

        if (searchOffset)
        {
            // We are scanning for PspTerminateProcess which has
            // the same signature as PsTerminateProcess because
            // PsTerminateProcess is simply a wrapper on XP.
            INIT_SCAN(
                &KphDynPsTerminateProcessScan,
                PspTerminateProcess51Bytes,
                sizeof(PspTerminateProcess51Bytes),
                searchOffset, scanLength, 0
                );
            INIT_SCAN(
                &KphDynPspTerminateThreadByPointerScan,
                PspTerminateThreadByPointer51Bytes,
                sizeof(PspTerminateThreadByPointer51Bytes),
                searchOffset, scanLength, 0
                );
        }

        dprintf("Initialized version-specific data for Windows XP SP%d\n", servicePack);
    }
    // Windows Server 2003
    else if (majorVersion == 5 && minorVersion == 2)
    {
        ULONG_PTR searchOffset = (ULONG_PTR)KphGetSystemRoutineAddress(L"RtlCreateHeap");
        ULONG scanLength = 0x100000;

        KphDynNtVersion = PHNT_WS03;

        if (servicePack == 0)
        {
        }
        else if (servicePack == 1)
        {
        }
        else if (servicePack == 2)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }

        KphDynEpObjectTable = 0xd4;
        KphDynEpRundownProtect = 0x90;
        KphDynOtName = 0x40;
        KphDynOtIndex = 0x4c;

        if (searchOffset)
        {
            // We are scanning for PspTerminateProcess which has
            // the same signature as PsTerminateProcess because
            // PsTerminateProcess is simply a wrapper on Server 2003.
            INIT_SCAN(
                &KphDynPsTerminateProcessScan,
                PspTerminateProcess52Bytes,
                sizeof(PspTerminateProcess52Bytes),
                searchOffset - 0x50000, scanLength, 0
                );
            INIT_SCAN(
                &KphDynPspTerminateThreadByPointerScan,
                PspTerminateThreadByPointer52Bytes,
                sizeof(PspTerminateThreadByPointer52Bytes),
                searchOffset - 0x20000, scanLength, 0
                );
        }

        dprintf("Initialized version-specific data for Windows Server 2003 SP%d\n", servicePack);
    }
    // Windows Vista, Windows Server 2008
    else if (majorVersion == 6 && minorVersion == 0)
    {
        ULONG_PTR searchOffset = (ULONG_PTR)KphGetSystemRoutineAddress(L"NtClose");
        ULONG scanLength = 0x100000;

        KphDynNtVersion = PHNT_VISTA;

        if (servicePack == 0)
        {
            KphDynOtName = 0x40;
            KphDynOtIndex = 0x4c;
        }
        else if (servicePack == 1)
        {
            KphDynOtName = 0x8; // they moved Mutex (ERESOURCE) further down
            KphDynOtIndex = 0x14;
        }
        else if (servicePack == 2)
        {
            KphDynOtName = 0x8;
            KphDynOtIndex = 0x14;
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }

        KphDynEgeGuid = 0xc;
        KphDynEpObjectTable = 0xdc;
        KphDynEpRundownProtect = 0x98;
        KphDynEreGuidEntry = 0x8;

        if (searchOffset)
        {
            INIT_SCAN(
                &KphDynPsTerminateProcessScan,
                PsTerminateProcess60Bytes,
                sizeof(PsTerminateProcess60Bytes),
                searchOffset, scanLength, 0
                );
            INIT_SCAN(
                &KphDynPspTerminateThreadByPointerScan,
                PspTerminateThreadByPointer60Bytes,
                sizeof(PspTerminateThreadByPointer60Bytes),
                searchOffset - 0x50000, scanLength, 0
                );
        }

        dprintf("Initialized version-specific data for Windows Vista SP%d/Windows Server 2008\n", servicePack);
    }
    // Windows 7, Windows Server 2008 R2
    else if (majorVersion == 6 && minorVersion == 1)
    {
        ULONG_PTR searchOffset1 = (ULONG_PTR)KphGetSystemRoutineAddress(L"LsaFreeReturnBuffer");
        ULONG_PTR searchOffset2 = (ULONG_PTR)KphGetSystemRoutineAddress(L"RtlMapGenericMask");

        KphDynNtVersion = PHNT_WIN7;

        if (servicePack == 0)
        {
        }
        else if (servicePack == 1)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }

        KphDynEgeGuid = 0xc;
        KphDynEpObjectTable = 0xf4;
        KphDynEpRundownProtect = 0xb0;
        KphDynEreGuidEntry = 0x8;
        KphDynOtName = 0x8;
        KphDynOtIndex = 0x14; // now only a UCHAR, not a ULONG

        if (searchOffset1)
        {
            INIT_SCAN(
                &KphDynPsTerminateProcessScan,
                PsTerminateProcess61Bytes,
                sizeof(PsTerminateProcess61Bytes),
                searchOffset1, 0xa000, 0
                );
        }

        if (searchOffset2)
        {
            INIT_SCAN(
                &KphDynPspTerminateThreadByPointerScan,
                PspTerminateThreadByPointer61Bytes,
                sizeof(PspTerminateThreadByPointer61Bytes),
                searchOffset2, 0x1a000, 0
                );
        }

        dprintf("Initialized version-specific data for Windows 7 SP%d\n", servicePack);
    }
    // Windows 8, Windows Server 2012
    else if (majorVersion == 6 && minorVersion == 2)
    {
        ULONG_PTR searchOffset1 = (ULONG_PTR)KphGetSystemRoutineAddress(L"IoSetIoCompletion");
        ULONG_PTR searchOffset2 = searchOffset1;

        KphDynNtVersion = PHNT_WIN8;

        if (servicePack == 0)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }

        KphDynEgeGuid = 0xc;
        KphDynEpObjectTable = 0x150;
        KphDynEpRundownProtect = 0xb0;
        KphDynEreGuidEntry = 0x8;
        KphDynOtName = 0x8;
        KphDynOtIndex = 0x14;

        if (searchOffset1)
        {
            INIT_SCAN(
                &KphDynPsTerminateProcessScan,
                PsTerminateProcess62Bytes,
                sizeof(PsTerminateProcess62Bytes),
                searchOffset1, 0x8000, 0
                );
        }

        if (searchOffset2)
        {
            INIT_SCAN(
                &KphDynPspTerminateThreadByPointerScan,
                PspTerminateThreadByPointer62Bytes,
                sizeof(PspTerminateThreadByPointer62Bytes),
                searchOffset2, 0x8000, 0
                );
        }

        dprintf("Initialized version-specific data for Windows 8 SP%d\n", servicePack);
    }
    // Windows 8.1, Windows Server 2012 R2
    else if (majorVersion == 6 && minorVersion == 3)
    {
        ULONG_PTR searchOffset1 = (ULONG_PTR)KphGetSystemRoutineAddress(L"IoSetIoCompletion");
        ULONG_PTR searchOffset2 = searchOffset1;

        KphDynNtVersion = PHNT_WINBLUE;

        if (servicePack == 0)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }

        KphDynEgeGuid = 0xc;
        KphDynEpObjectTable = 0x150;
        KphDynEpRundownProtect = 0xb0;
        KphDynOtName = 0x8;
        KphDynOtIndex = 0x14;

        if (searchOffset1)
        {
            INIT_SCAN(
                &KphDynPsTerminateProcessScan,
                PsTerminateProcess63Bytes,
                sizeof(PsTerminateProcess63Bytes),
                searchOffset1, 0x8000, 0
                );
        }

        if (searchOffset2)
        {
            INIT_SCAN(
                &KphDynPspTerminateThreadByPointerScan,
                PspTerminateThreadByPointer63Bytes,
                sizeof(PspTerminateThreadByPointer63Bytes),
                searchOffset2, 0x8000, 0
                );
        }

        dprintf("Initialized version-specific data for Windows 8.1 SP%d\n", servicePack);
    }
    else if (majorVersion == 6 && minorVersion > 3 || majorVersion > 6)
    {
        KphDynNtVersion = 0xffffffff;
        return STATUS_NOT_SUPPORTED;
    }
    else
    {
        return STATUS_NOT_SUPPORTED;
    }

    return STATUS_SUCCESS;
}

#else

static NTSTATUS KphpAmd64DataInitialization(
    VOID
    )
{
    ULONG majorVersion, minorVersion, servicePack, buildNumber;

    PAGED_CODE();

    majorVersion = KphDynOsVersionInfo.dwMajorVersion;
    minorVersion = KphDynOsVersionInfo.dwMinorVersion;
    servicePack = KphDynOsVersionInfo.wServicePackMajor;
    buildNumber = KphDynOsVersionInfo.dwBuildNumber;
    dprintf("Windows %d.%d, SP%d.%d, build %d\n",
        majorVersion, minorVersion, servicePack,
        KphDynOsVersionInfo.wServicePackMinor, buildNumber
        );

    // Windows XP
    if (majorVersion == 5 && minorVersion == 1)
    {
        KphDynNtVersion = PHNT_WINXP;

        if (servicePack == 0)
        {
            return STATUS_NOT_SUPPORTED;
        }
        else if (servicePack == 1)
        {
            return STATUS_NOT_SUPPORTED;
        }
        else if (servicePack == 2)
        {
        }
        else if (servicePack == 3)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }
    }
    // Windows Server 2003
    else if (majorVersion == 5 && minorVersion == 2)
    {
        KphDynNtVersion = PHNT_WS03;

        if (servicePack == 0)
        {
        }
        else if (servicePack == 1)
        {
        }
        else if (servicePack == 2)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }
    }
    // Windows Vista, Windows Server 2008
    else if (majorVersion == 6 && minorVersion == 0)
    {
        KphDynNtVersion = PHNT_VISTA;

        if (servicePack == 0)
        {
        }
        else if (servicePack == 1)
        {
        }
        else if (servicePack == 2)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }
    }
    // Windows 7, Windows Server 2008 R2
    else if (majorVersion == 6 && minorVersion == 1)
    {
        KphDynNtVersion = PHNT_WIN7;

        if (servicePack == 0)
        {
        }
        else if (servicePack == 1)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }
    }
    // Windows 8, Windows Server 2012
    else if (majorVersion == 6 && minorVersion == 2)
    {
        KphDynNtVersion = PHNT_WIN8;

        if (servicePack == 0)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }
    }
    // Windows 8.1, Windows Server 2012 R2
    else if (majorVersion == 6 && minorVersion == 3)
    {
        KphDynNtVersion = PHNT_WINBLUE;

        if (servicePack == 0)
        {
        }
        else
        {
            return STATUS_NOT_SUPPORTED;
        }
    }
    else if (majorVersion == 6 && minorVersion > 3 || majorVersion > 6)
    {
        KphDynNtVersion = 0xffffffff;
        return STATUS_NOT_SUPPORTED;
    }
    else
    {
        return STATUS_NOT_SUPPORTED;
    }

    return STATUS_SUCCESS;
}

#endif

PVOID KphGetDynamicProcedureScan(
    __inout PKPH_PROCEDURE_SCAN ProcedureScan
    )
{
    PUCHAR bytes;
    ULONG length;
    ULONG_PTR endAddress;
    ULONG_PTR address;

    if (!ProcedureScan->Initialized)
        return NULL;

    // This function is thread-safe.

    if (!ProcedureScan->Scanned)
    {
        bytes = ProcedureScan->Bytes;
        length = ProcedureScan->Length;
        endAddress = ProcedureScan->StartAddress + ProcedureScan->ScanLength;

        // Make sure we're scanning inside valid memory.
        if (NT_SUCCESS(KphValidateAddressForSystemModules((PVOID)ProcedureScan->StartAddress, ProcedureScan->ScanLength)))
        {
            for (address = ProcedureScan->StartAddress; address < endAddress; address++)
            {
                if (RtlCompareMemory((PVOID)address, bytes, length) == length)
                {
                    ProcedureScan->ProcedureAddress = (PVOID)(address + ProcedureScan->Displacement);
                    break;
                }
            }
        }
        else
        {
            ProcedureScan->ProcedureAddress = NULL;
        }

        ProcedureScan->Scanned = TRUE;
    }

    return ProcedureScan->ProcedureAddress;
}
