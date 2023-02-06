#ifndef _KPHAPI_H
#define _KPHAPI_H

// This file contains KProcessHacker definitions shared across
// kernel-mode and user-mode.

// Process information

typedef enum _KPH_PROCESS_INFORMATION_CLASS
{
    KphProcessProtectionInformation = 1, // q: KPH_PROCESS_PROTECTION_INFORMATION
    KphProcessExecuteFlags = 2, // s: ULONG
    KphProcessIoPriority = 3, // qs: ULONG
    MaxKphProcessInfoClass
} KPH_PROCESS_INFORMATION_CLASS;

typedef struct _KPH_PROCESS_PROTECTION_INFORMATION
{
    BOOLEAN IsProtectedProcess;
} KPH_PROCESS_PROTECTION_INFORMATION, *PKPH_PROCESS_PROTECTION_INFORMATION;

// Thread information

typedef enum _KPH_THREAD_INFORMATION_CLASS
{
    KphThreadWin32Thread = 1, // q: PVOID
    KphThreadImpersonationToken = 2, // s: HANDLE
    KphThreadIoPriority = 3, // qs: ULONG
    MaxKphThreadInfoClass
} KPH_THREAD_INFORMATION_CLASS;

// Process handle information

typedef struct _KPH_PROCESS_HANDLE
{
    HANDLE Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
    USHORT ObjectTypeIndex;
    USHORT Reserved1;
    ULONG HandleAttributes;
    ULONG Reserved2;
} KPH_PROCESS_HANDLE, *PKPH_PROCESS_HANDLE;

typedef struct _KPH_PROCESS_HANDLE_INFORMATION
{
    ULONG HandleCount;
    KPH_PROCESS_HANDLE Handles[1];
} KPH_PROCESS_HANDLE_INFORMATION, *PKPH_PROCESS_HANDLE_INFORMATION;

// Object information

typedef enum _KPH_OBJECT_INFORMATION_CLASS
{
    KphObjectBasicInformation, // q: OBJECT_BASIC_INFORMATION
    KphObjectNameInformation, // q: OBJECT_NAME_INFORMATION
    KphObjectTypeInformation, // q: OBJECT_TYPE_INFORMATION
    KphObjectHandleFlagInformation, // qs: OBJECT_HANDLE_FLAG_INFORMATION
    KphObjectProcessBasicInformation, // q: PROCESS_BASIC_INFORMATION
    KphObjectThreadBasicInformation, // q: THREAD_BASIC_INFORMATION
    KphObjectEtwRegBasicInformation, // q: ETWREG_BASIC_INFORMATION
    KphObjectFileObjectInformation, // q: KPH_FILE_OBJECT_INFORMATION
    KphObjectFileObjectDriver, // q: KPH_FILE_OBJECT_DRIVER
    MaxKphObjectInfoClass
} KPH_OBJECT_INFORMATION_CLASS;

typedef struct _KPH_FILE_OBJECT_INFORMATION
{
    BOOLEAN LockOperation;
    BOOLEAN DeletePending;
    BOOLEAN ReadAccess;
    BOOLEAN WriteAccess;
    BOOLEAN DeleteAccess;
    BOOLEAN SharedRead;
    BOOLEAN SharedWrite;
    BOOLEAN SharedDelete;
    LARGE_INTEGER CurrentByteOffset;
    ULONG Flags;
} KPH_FILE_OBJECT_INFORMATION, *PKPH_FILE_OBJECT_INFORMATION;

typedef struct _KPH_FILE_OBJECT_DRIVER
{
    HANDLE DriverHandle;
} KPH_FILE_OBJECT_DRIVER, *PKPH_FILE_OBJECT_DRIVER;

// Driver information

typedef enum _DRIVER_INFORMATION_CLASS
{
    DriverBasicInformation,
    DriverNameInformation,
    DriverServiceKeyNameInformation,
    MaxDriverInfoClass
} DRIVER_INFORMATION_CLASS;

typedef struct _DRIVER_BASIC_INFORMATION
{
    ULONG Flags;
    PVOID DriverStart;
    ULONG DriverSize;
} DRIVER_BASIC_INFORMATION, *PDRIVER_BASIC_INFORMATION;

typedef struct _DRIVER_NAME_INFORMATION
{
    UNICODE_STRING DriverName;
} DRIVER_NAME_INFORMATION, *PDRIVER_NAME_INFORMATION;

typedef struct _DRIVER_SERVICE_KEY_NAME_INFORMATION
{
    UNICODE_STRING ServiceKeyName;
} DRIVER_SERVICE_KEY_NAME_INFORMATION, *PDRIVER_SERVICE_KEY_NAME_INFORMATION;

// ETW registration object information

typedef struct _ETWREG_BASIC_INFORMATION
{
    GUID Guid;
    ULONG_PTR SessionId;
} ETWREG_BASIC_INFORMATION, *PETWREG_BASIC_INFORMATION;

// Device

#define KPH_DEVICE_SHORT_NAME L"KProcessHacker2"
#define KPH_DEVICE_TYPE 0x9999
#define KPH_DEVICE_NAME (L"\\Device\\" KPH_DEVICE_SHORT_NAME)

// Parameters

typedef enum _KPH_SECURITY_LEVEL
{
    KphSecurityNone = 0, // all clients are allowed
    KphSecurityPrivilegeCheck = 1, // require SeDebugPrivilege
    KphMaxSecurityLevel
} KPH_SECURITY_LEVEL, *PKPH_SECURITY_LEVEL;

typedef struct _KPH_DYN_STRUCT_DATA
{
    SHORT EgeGuid;
    SHORT EpObjectTable;
    SHORT Reserved0;
    SHORT Reserved1;
    SHORT EpRundownProtect;
    SHORT EreGuidEntry;
    SHORT HtHandleContentionEvent;
    SHORT OtName;
    SHORT OtIndex;
    SHORT ObDecodeShift;
    SHORT ObAttributesShift;
} KPH_DYN_STRUCT_DATA, *PKPH_DYN_STRUCT_DATA;

typedef struct _KPH_DYN_PACKAGE
{
    USHORT MajorVersion;
    USHORT MinorVersion;
    USHORT ServicePackMajor; // -1 to ignore
    USHORT BuildNumber; // -1 to ignore
    ULONG ResultingNtVersion; // PHNT_*
    KPH_DYN_STRUCT_DATA StructData;
} KPH_DYN_PACKAGE, *PKPH_DYN_PACKAGE;

#define KPH_DYN_CONFIGURATION_VERSION 2
#define KPH_DYN_MAXIMUM_PACKAGES 64

typedef struct _KPH_DYN_CONFIGURATION
{
    ULONG Version;
    ULONG NumberOfPackages;
    KPH_DYN_PACKAGE Packages[1];
} KPH_DYN_CONFIGURATION, *PKPH_DYN_CONFIGURATION;

// Features

// No features defined.

// Control codes

#define KPH_CTL_CODE(x) CTL_CODE(KPH_DEVICE_TYPE, 0x800 + x, METHOD_NEITHER, FILE_ANY_ACCESS)

// General
#define KPH_GETFEATURES KPH_CTL_CODE(0)

// Processes
#define KPH_OPENPROCESS KPH_CTL_CODE(50)
#define KPH_OPENPROCESSTOKEN KPH_CTL_CODE(51)
#define KPH_OPENPROCESSJOB KPH_CTL_CODE(52)
#define KPH_SUSPENDPROCESS KPH_CTL_CODE(53)
#define KPH_RESUMEPROCESS KPH_CTL_CODE(54)
#define KPH_TERMINATEPROCESS KPH_CTL_CODE(55)
#define KPH_READVIRTUALMEMORY KPH_CTL_CODE(56)
#define KPH_WRITEVIRTUALMEMORY KPH_CTL_CODE(57)
#define KPH_READVIRTUALMEMORYUNSAFE KPH_CTL_CODE(58)
#define KPH_QUERYINFORMATIONPROCESS KPH_CTL_CODE(59)
#define KPH_SETINFORMATIONPROCESS KPH_CTL_CODE(60)

// Threads
#define KPH_OPENTHREAD KPH_CTL_CODE(100)
#define KPH_OPENTHREADPROCESS KPH_CTL_CODE(101)
#define KPH_TERMINATETHREAD KPH_CTL_CODE(102)
#define KPH_TERMINATETHREADUNSAFE KPH_CTL_CODE(103)
#define KPH_GETCONTEXTTHREAD KPH_CTL_CODE(104)
#define KPH_SETCONTEXTTHREAD KPH_CTL_CODE(105)
#define KPH_CAPTURESTACKBACKTRACETHREAD KPH_CTL_CODE(106)
#define KPH_QUERYINFORMATIONTHREAD KPH_CTL_CODE(107)
#define KPH_SETINFORMATIONTHREAD KPH_CTL_CODE(108)

// Handles
#define KPH_ENUMERATEPROCESSHANDLES KPH_CTL_CODE(150)
#define KPH_QUERYINFORMATIONOBJECT KPH_CTL_CODE(151)
#define KPH_SETINFORMATIONOBJECT KPH_CTL_CODE(152)
#define KPH_DUPLICATEOBJECT KPH_CTL_CODE(153)

// Misc.
#define KPH_OPENDRIVER KPH_CTL_CODE(200)
#define KPH_QUERYINFORMATIONDRIVER KPH_CTL_CODE(201)

#endif