#ifndef _NTEXAPI_H
#define _NTEXAPI_H

#include <ntkeapi.h>

#if (PHNT_MODE != PHNT_MODE_KERNEL)

// Thread execution

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDelayExecution(
    _In_ BOOLEAN Alertable,
    _In_ PLARGE_INTEGER DelayInterval
    );

// Environment values

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemEnvironmentValue(
    _In_ PUNICODE_STRING VariableName,
    _Out_writes_bytes_(ValueLength) PWSTR VariableValue,
    _In_ USHORT ValueLength,
    _Out_opt_ PUSHORT ReturnLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemEnvironmentValue(
    _In_ PUNICODE_STRING VariableName,
    _In_ PUNICODE_STRING VariableValue
    );

#if (PHNT_VERSION >= PHNT_WIN8)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemEnvironmentValueEx(
    _In_ PUNICODE_STRING VariableName,
    _In_ LPGUID VendorGuid,
    _Out_writes_bytes_opt_(*ValueLength) PVOID Value,
    _Inout_ PULONG ValueLength,
    _Out_opt_ PULONG Attributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemEnvironmentValueEx(
    _In_ PUNICODE_STRING VariableName,
    _In_ LPGUID VendorGuid,
    _In_reads_bytes_opt_(ValueLength) PVOID Value,
    _In_ ULONG ValueLength,
    _In_ ULONG Attributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtEnumerateSystemEnvironmentValuesEx(
    _In_ ULONG InformationClass,
    _Out_ PVOID Buffer,
    _Inout_ PULONG BufferLength
    );

#endif

// EFI

// private
typedef struct _BOOT_ENTRY
{
    ULONG Version;
    ULONG Length;
    ULONG Id;
    ULONG Attributes;
    ULONG FriendlyNameOffset;
    ULONG BootFilePathOffset;
    ULONG OsOptionsLength;
    UCHAR OsOptions[1];
} BOOT_ENTRY, *PBOOT_ENTRY;

// private
typedef struct _BOOT_ENTRY_LIST
{
    ULONG NextEntryOffset;
    BOOT_ENTRY BootEntry;
} BOOT_ENTRY_LIST, *PBOOT_ENTRY_LIST;

// private
typedef struct _BOOT_OPTIONS
{
    ULONG Version;
    ULONG Length;
    ULONG Timeout;
    ULONG CurrentBootEntryId;
    ULONG NextBootEntryId;
    WCHAR HeadlessRedirection[1];
} BOOT_OPTIONS, *PBOOT_OPTIONS;

// private
typedef struct _FILE_PATH
{
    ULONG Version;
    ULONG Length;
    ULONG Type;
    UCHAR FilePath[1];
} FILE_PATH, *PFILE_PATH;

// private
typedef struct _EFI_DRIVER_ENTRY
{
    ULONG Version;
    ULONG Length;
    ULONG Id;
    ULONG FriendlyNameOffset;
    ULONG DriverFilePathOffset;
} EFI_DRIVER_ENTRY, *PEFI_DRIVER_ENTRY;

// private
typedef struct _EFI_DRIVER_ENTRY_LIST
{
    ULONG NextEntryOffset;
    EFI_DRIVER_ENTRY DriverEntry;
} EFI_DRIVER_ENTRY_LIST, *PEFI_DRIVER_ENTRY_LIST;

#if (PHNT_VERSION >= PHNT_VISTA)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAddBootEntry(
    _In_ PBOOT_ENTRY BootEntry,
    _Out_opt_ PULONG Id
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteBootEntry(
    _In_ ULONG Id
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtModifyBootEntry(
    _In_ PBOOT_ENTRY BootEntry
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtEnumerateBootEntries(
    _Out_writes_bytes_opt_(*BufferLength) PVOID Buffer,
    _Inout_ PULONG BufferLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryBootEntryOrder(
    _Out_writes_opt_(*Count) PULONG Ids,
    _Inout_ PULONG Count
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetBootEntryOrder(
    _In_reads_(Count) PULONG Ids,
    _In_ ULONG Count
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryBootOptions(
    _Out_writes_bytes_opt_(*BootOptionsLength) PBOOT_OPTIONS BootOptions,
    _Inout_ PULONG BootOptionsLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetBootOptions(
    _In_ PBOOT_OPTIONS BootOptions,
    _In_ ULONG FieldsToChange
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtTranslateFilePath(
    _In_ PFILE_PATH InputFilePath,
    _In_ ULONG OutputType,
    _Out_writes_bytes_opt_(*OutputFilePathLength) PFILE_PATH OutputFilePath,
    _Inout_opt_ PULONG OutputFilePathLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAddDriverEntry(
    _In_ PEFI_DRIVER_ENTRY DriverEntry,
    _Out_opt_ PULONG Id
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteDriverEntry(
    _In_ ULONG Id
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtModifyDriverEntry(
    _In_ PEFI_DRIVER_ENTRY DriverEntry
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtEnumerateDriverEntries(
    _Out_writes_bytes_opt_(*BufferLength) PVOID Buffer,
    _Inout_ PULONG BufferLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDriverEntryOrder(
    _Out_writes_opt_(*Count) PULONG Ids,
    _Inout_ PULONG Count
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetDriverEntryOrder(
    _In_reads_(Count) PULONG Ids,
    _In_ ULONG Count
    );

#endif

// Event

#ifndef EVENT_QUERY_STATE
#define EVENT_QUERY_STATE 0x0001
#endif

typedef enum _EVENT_INFORMATION_CLASS
{
    EventBasicInformation
} EVENT_INFORMATION_CLASS;

typedef struct _EVENT_BASIC_INFORMATION
{
    EVENT_TYPE EventType;
    LONG EventState;
} EVENT_BASIC_INFORMATION, *PEVENT_BASIC_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateEvent(
    _Out_ PHANDLE EventHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ EVENT_TYPE EventType,
    _In_ BOOLEAN InitialState
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenEvent(
    _Out_ PHANDLE EventHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetEvent(
    _In_ HANDLE EventHandle,
    _Out_opt_ PLONG PreviousState
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetEventBoostPriority(
    _In_ HANDLE EventHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtClearEvent(
    _In_ HANDLE EventHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtResetEvent(
    _In_ HANDLE EventHandle,
    _Out_opt_ PLONG PreviousState
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtPulseEvent(
    _In_ HANDLE EventHandle,
    _Out_opt_ PLONG PreviousState
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryEvent(
    _In_ HANDLE EventHandle,
    _In_ EVENT_INFORMATION_CLASS EventInformationClass,
    _Out_writes_bytes_(EventInformationLength) PVOID EventInformation,
    _In_ ULONG EventInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

// Event Pair

#define EVENT_PAIR_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateEventPair(
    _Out_ PHANDLE EventPairHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenEventPair(
    _Out_ PHANDLE EventPairHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetLowEventPair(
    _In_ HANDLE EventPairHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetHighEventPair(
    _In_ HANDLE EventPairHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitLowEventPair(
    _In_ HANDLE EventPairHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitHighEventPair(
    _In_ HANDLE EventPairHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetLowWaitHighEventPair(
    _In_ HANDLE EventPairHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetHighWaitLowEventPair(
    _In_ HANDLE EventPairHandle
    );

// Mutant

typedef enum _MUTANT_INFORMATION_CLASS
{
    MutantBasicInformation,
    MutantOwnerInformation
} MUTANT_INFORMATION_CLASS;

typedef struct _MUTANT_BASIC_INFORMATION
{
    LONG CurrentCount;
    BOOLEAN OwnedByCaller;
    BOOLEAN AbandonedState;
} MUTANT_BASIC_INFORMATION, *PMUTANT_BASIC_INFORMATION;

typedef struct _MUTANT_OWNER_INFORMATION
{
    CLIENT_ID ClientId;
} MUTANT_OWNER_INFORMATION, *PMUTANT_OWNER_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateMutant(
    _Out_ PHANDLE MutantHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ BOOLEAN InitialOwner
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenMutant(
    _Out_ PHANDLE MutantHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseMutant(
    _In_ HANDLE MutantHandle,
    _Out_opt_ PLONG PreviousCount
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryMutant(
    _In_ HANDLE MutantHandle,
    _In_ MUTANT_INFORMATION_CLASS MutantInformationClass,
    _Out_writes_bytes_(MutantInformationLength) PVOID MutantInformation,
    _In_ ULONG MutantInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

// Semaphore

#ifndef SEMAPHORE_QUERY_STATE
#define SEMAPHORE_QUERY_STATE 0x0001
#endif

typedef enum _SEMAPHORE_INFORMATION_CLASS
{
    SemaphoreBasicInformation
} SEMAPHORE_INFORMATION_CLASS;

typedef struct _SEMAPHORE_BASIC_INFORMATION
{
    LONG CurrentCount;
    LONG MaximumCount;
} SEMAPHORE_BASIC_INFORMATION, *PSEMAPHORE_BASIC_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateSemaphore(
    _Out_ PHANDLE SemaphoreHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ LONG InitialCount,
    _In_ LONG MaximumCount
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenSemaphore(
    _Out_ PHANDLE SemaphoreHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseSemaphore(
    _In_ HANDLE SemaphoreHandle,
    _In_ LONG ReleaseCount,
    _Out_opt_ PLONG PreviousCount
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySemaphore(
    _In_ HANDLE SemaphoreHandle,
    _In_ SEMAPHORE_INFORMATION_CLASS SemaphoreInformationClass,
    _Out_writes_bytes_(SemaphoreInformationLength) PVOID SemaphoreInformation,
    _In_ ULONG SemaphoreInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

// Timer

typedef enum _TIMER_INFORMATION_CLASS
{
    TimerBasicInformation
} TIMER_INFORMATION_CLASS;

typedef struct _TIMER_BASIC_INFORMATION
{
    LARGE_INTEGER RemainingTime;
    BOOLEAN TimerState;
} TIMER_BASIC_INFORMATION, *PTIMER_BASIC_INFORMATION;

typedef VOID (NTAPI *PTIMER_APC_ROUTINE)(
    _In_ PVOID TimerContext,
    _In_ ULONG TimerLowValue,
    _In_ LONG TimerHighValue
    );

typedef enum _TIMER_SET_INFORMATION_CLASS
{
    TimerSetCoalescableTimer,
    MaxTimerInfoClass
} TIMER_SET_INFORMATION_CLASS;

#if (PHNT_VERSION >= PHNT_WIN7)
struct _COUNTED_REASON_CONTEXT;

typedef struct _TIMER_SET_COALESCABLE_TIMER_INFO
{
    _In_ LARGE_INTEGER DueTime;
    _In_opt_ PTIMER_APC_ROUTINE TimerApcRoutine;
    _In_opt_ PVOID TimerContext;
    _In_opt_ struct _COUNTED_REASON_CONTEXT *WakeContext;
    _In_opt_ ULONG Period;
    _In_ ULONG TolerableDelay;
    _Out_opt_ PBOOLEAN PreviousState;
} TIMER_SET_COALESCABLE_TIMER_INFO, *PTIMER_SET_COALESCABLE_TIMER_INFO;
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateTimer(
    _Out_ PHANDLE TimerHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ TIMER_TYPE TimerType
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenTimer(
    _Out_ PHANDLE TimerHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetTimer(
    _In_ HANDLE TimerHandle,
    _In_ PLARGE_INTEGER DueTime,
    _In_opt_ PTIMER_APC_ROUTINE TimerApcRoutine,
    _In_opt_ PVOID TimerContext,
    _In_ BOOLEAN ResumeTimer,
    _In_opt_ LONG Period,
    _Out_opt_ PBOOLEAN PreviousState
    );

#if (PHNT_VERSION >= PHNT_WIN7)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetTimerEx(
    _In_ HANDLE TimerHandle,
    _In_ TIMER_SET_INFORMATION_CLASS TimerSetInformationClass,
    _Inout_updates_bytes_opt_(TimerSetInformationLength) PVOID TimerSetInformation,
    _In_ ULONG TimerSetInformationLength
    );
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCancelTimer(
    _In_ HANDLE TimerHandle,
    _Out_opt_ PBOOLEAN CurrentState
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryTimer(
    _In_ HANDLE TimerHandle,
    _In_ TIMER_INFORMATION_CLASS TimerInformationClass,
    _Out_writes_bytes_(TimerInformationLength) PVOID TimerInformation,
    _In_ ULONG TimerInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

#if (PHNT_VERSION >= PHNT_WIN8)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateIRTimer(
    _Out_ PHANDLE TimerHandle,
    _In_ ACCESS_MASK DesiredAccess
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetIRTimer(
    _In_ HANDLE TimerHandle,
    _In_opt_ PLARGE_INTEGER DueTime
    );

#endif

typedef struct _T2_SET_PARAMETERS_V0
{
    ULONG Version;
    ULONG Reserved;
    LONGLONG NoWakeTolerance;
} T2_SET_PARAMETERS, *PT2_SET_PARAMETERS;

typedef PVOID PT2_CANCEL_PARAMETERS;

#if (PHNT_VERSION >= PHNT_THRESHOLD)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateTimer2(
    _Out_ PHANDLE TimerHandle,
    _In_opt_ PVOID Reserved1,
    _In_opt_ PVOID Reserved2,
    _In_ ULONG Attributes,
    _In_ ACCESS_MASK DesiredAccess
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetTimer2(
    _In_ HANDLE TimerHandle,
    _In_ PLARGE_INTEGER DueTime,
    _In_opt_ PLARGE_INTEGER Period,
    _In_ PT2_SET_PARAMETERS Parameters
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCancelTimer2(
    _In_ HANDLE TimerHandle,
    _In_ PT2_CANCEL_PARAMETERS Parameters
    );

#endif

// Profile

#define PROFILE_CONTROL 0x0001
#define PROFILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | PROFILE_CONTROL)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProfile(
    _Out_ PHANDLE ProfileHandle,
    _In_opt_ HANDLE Process,
    _In_ PVOID ProfileBase,
    _In_ SIZE_T ProfileSize,
    _In_ ULONG BucketSize,
    _In_reads_bytes_(BufferSize) PULONG Buffer,
    _In_ ULONG BufferSize,
    _In_ KPROFILE_SOURCE ProfileSource,
    _In_ KAFFINITY Affinity
    );

#if (PHNT_VERSION >= PHNT_WIN7)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProfileEx(
    _Out_ PHANDLE ProfileHandle,
    _In_opt_ HANDLE Process,
    _In_ PVOID ProfileBase,
    _In_ SIZE_T ProfileSize,
    _In_ ULONG BucketSize,
    _In_reads_bytes_(BufferSize) PULONG Buffer,
    _In_ ULONG BufferSize,
    _In_ KPROFILE_SOURCE ProfileSource,
    _In_ USHORT GroupCount,
    _In_reads_(GroupCount) PGROUP_AFFINITY GroupAffinity
    );
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtStartProfile(
    _In_ HANDLE ProfileHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtStopProfile(
    _In_ HANDLE ProfileHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryIntervalProfile(
    _In_ KPROFILE_SOURCE ProfileSource,
    _Out_ PULONG Interval
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetIntervalProfile(
    _In_ ULONG Interval,
    _In_ KPROFILE_SOURCE Source
    );

// Keyed Event

#define KEYEDEVENT_WAIT 0x0001
#define KEYEDEVENT_WAKE 0x0002
#define KEYEDEVENT_ALL_ACCESS \
    (STANDARD_RIGHTS_REQUIRED | KEYEDEVENT_WAIT | KEYEDEVENT_WAKE)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateKeyedEvent(
    _Out_ PHANDLE KeyedEventHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ ULONG Flags
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenKeyedEvent(
    _Out_ PHANDLE KeyedEventHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseKeyedEvent(
    _In_ HANDLE KeyedEventHandle,
    _In_ PVOID KeyValue,
    _In_ BOOLEAN Alertable,
    _In_opt_ PLARGE_INTEGER Timeout
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForKeyedEvent(
    _In_ HANDLE KeyedEventHandle,
    _In_ PVOID KeyValue,
    _In_ BOOLEAN Alertable,
    _In_opt_ PLARGE_INTEGER Timeout
    );

// UMS

#if (PHNT_VERSION >= PHNT_WIN7)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUmsThreadYield(
    _In_ PVOID SchedulerParam
    );
#endif

// WNF

// begin_private

typedef struct _WNF_STATE_NAME
{
    ULONG Data[2];
} WNF_STATE_NAME, *PWNF_STATE_NAME;

typedef const WNF_STATE_NAME *PCWNF_STATE_NAME;

typedef enum _WNF_STATE_NAME_LIFETIME
{
    WnfWellKnownStateName,
    WnfPermanentStateName,
    WnfPersistentStateName,
    WnfTemporaryStateName
} WNF_STATE_NAME_LIFETIME;

typedef enum _WNF_STATE_NAME_INFORMATION
{
    WnfInfoStateNameExist,
    WnfInfoSubscribersPresent,
    WnfInfoIsQuiescent
} WNF_STATE_NAME_INFORMATION;

typedef enum _WNF_DATA_SCOPE
{
    WnfDataScopeSystem,
    WnfDataScopeSession,
    WnfDataScopeUser,
    WnfDataScopeProcess
} WNF_DATA_SCOPE;

typedef struct _WNF_TYPE_ID
{
    GUID TypeId;
} WNF_TYPE_ID, *PWNF_TYPE_ID;

typedef const WNF_TYPE_ID *PCWNF_TYPE_ID;

// rev
typedef ULONG WNF_CHANGE_STAMP, *PWNF_CHANGE_STAMP;

typedef struct _WNF_DELIVERY_DESCRIPTOR
{
    ULONGLONG SubscriptionId;
    WNF_STATE_NAME StateName;
    WNF_CHANGE_STAMP ChangeStamp;
    ULONG StateDataSize;
    ULONG EventMask;
    WNF_TYPE_ID TypeId;
    ULONG StateDataOffset;
} WNF_DELIVERY_DESCRIPTOR, *PWNF_DELIVERY_DESCRIPTOR;

// end_private

#if (PHNT_VERSION >= PHNT_WIN8)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateWnfStateName(
    _Out_ PWNF_STATE_NAME StateName,
    _In_ WNF_STATE_NAME_LIFETIME NameLifetime,
    _In_ WNF_DATA_SCOPE DataScope,
    _In_ BOOLEAN PersistData,
    _In_opt_ PCWNF_TYPE_ID TypeId,
    _In_ ULONG MaximumStateSize,
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteWnfStateName(
    _In_ PCWNF_STATE_NAME StateName
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtUpdateWnfStateData(
    _In_ PCWNF_STATE_NAME StateName,
    _In_reads_bytes_opt_(Length) const VOID* Buffer,
    _In_opt_ ULONG Length,
    _In_opt_ PCWNF_TYPE_ID TypeId,
    _In_opt_ const PVOID ExplicitScope,
    _In_ WNF_CHANGE_STAMP MatchingChangeStamp,
    _In_ LOGICAL CheckStamp
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteWnfStateData(
    _In_ PCWNF_STATE_NAME StateName,
    _In_opt_ const PVOID ExplicitScope
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryWnfStateData(
    _In_ PCWNF_STATE_NAME StateName,
    _In_opt_ PCWNF_TYPE_ID TypeId,
    _In_opt_ const VOID* ExplicitScope,
    _Out_ PWNF_CHANGE_STAMP ChangeStamp,
    _Out_writes_bytes_to_opt_(*BufferSize, *BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryWnfStateNameInformation(
    _In_ PCWNF_STATE_NAME StateName,
    _In_ WNF_STATE_NAME_INFORMATION NameInfoClass,
    _In_opt_ const PVOID ExplicitScope,
    _Out_writes_bytes_(InfoBufferSize) PVOID InfoBuffer,
    _In_ ULONG InfoBufferSize
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSubscribeWnfStateChange(
    _In_ PCWNF_STATE_NAME StateName,
    _In_opt_ WNF_CHANGE_STAMP ChangeStamp,
    _In_ ULONG EventMask,
    _Out_opt_ PULONG64 SubscriptionId
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnsubscribeWnfStateChange(
    _In_ PCWNF_STATE_NAME StateName
    );

#if (PHNT_VERSION >= PHNT_THRESHOLD)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetCompleteWnfStateSubscription(
    _In_opt_ PWNF_STATE_NAME OldDescriptorStateName,
    _In_opt_ ULONG64 *OldSubscriptionId,
    _In_opt_ ULONG OldDescriptorEventMask,
    _In_opt_ ULONG OldDescriptorStatus,
    _Out_writes_bytes_(DescriptorSize) PWNF_DELIVERY_DESCRIPTOR NewDeliveryDescriptor,
    _In_ ULONG DescriptorSize
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetWnfProcessNotificationEvent(
    _In_ HANDLE NotificationEvent
    );

#endif

#endif

// Worker factory

// begin_rev

#define WORKER_FACTORY_RELEASE_WORKER 0x0001
#define WORKER_FACTORY_WAIT 0x0002
#define WORKER_FACTORY_SET_INFORMATION 0x0004
#define WORKER_FACTORY_QUERY_INFORMATION 0x0008
#define WORKER_FACTORY_READY_WORKER 0x0010
#define WORKER_FACTORY_SHUTDOWN 0x0020

#define WORKER_FACTORY_ALL_ACCESS ( \
    STANDARD_RIGHTS_REQUIRED | \
    WORKER_FACTORY_RELEASE_WORKER | \
    WORKER_FACTORY_WAIT | \
    WORKER_FACTORY_SET_INFORMATION | \
    WORKER_FACTORY_QUERY_INFORMATION | \
    WORKER_FACTORY_READY_WORKER | \
    WORKER_FACTORY_SHUTDOWN \
    )

// end_rev

// begin_private

typedef enum _WORKERFACTORYINFOCLASS
{
    WorkerFactoryTimeout,
    WorkerFactoryRetryTimeout,
    WorkerFactoryIdleTimeout,
    WorkerFactoryBindingCount,
    WorkerFactoryThreadMinimum,
    WorkerFactoryThreadMaximum,
    WorkerFactoryPaused,
    WorkerFactoryBasicInformation,
    WorkerFactoryAdjustThreadGoal,
    WorkerFactoryCallbackType,
    WorkerFactoryStackInformation, // 10
    WorkerFactoryThreadBasePriority,
    WorkerFactoryTimeoutWaiters, // since THRESHOLD
    WorkerFactoryFlags,
    WorkerFactoryThreadSoftMaximum,
    MaxWorkerFactoryInfoClass
} WORKERFACTORYINFOCLASS, *PWORKERFACTORYINFOCLASS;

typedef struct _WORKER_FACTORY_BASIC_INFORMATION
{
    LARGE_INTEGER Timeout;
    LARGE_INTEGER RetryTimeout;
    LARGE_INTEGER IdleTimeout;
    BOOLEAN Paused;
    BOOLEAN TimerSet;
    BOOLEAN QueuedToExWorker;
    BOOLEAN MayCreate;
    BOOLEAN CreateInProgress;
    BOOLEAN InsertedIntoQueue;
    BOOLEAN Shutdown;
    ULONG BindingCount;
    ULONG ThreadMinimum;
    ULONG ThreadMaximum;
    ULONG PendingWorkerCount;
    ULONG WaitingWorkerCount;
    ULONG TotalWorkerCount;
    ULONG ReleaseCount;
    LONGLONG InfiniteWaitGoal;
    PVOID StartRoutine;
    PVOID StartParameter;
    HANDLE ProcessId;
    SIZE_T StackReserve;
    SIZE_T StackCommit;
    NTSTATUS LastThreadCreationStatus;
} WORKER_FACTORY_BASIC_INFORMATION, *PWORKER_FACTORY_BASIC_INFORMATION;

// end_private

#if (PHNT_VERSION >= PHNT_VISTA)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateWorkerFactory(
    _Out_ PHANDLE WorkerFactoryHandleReturn,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE CompletionPortHandle,
    _In_ HANDLE WorkerProcessHandle,
    _In_ PVOID StartRoutine,
    _In_opt_ PVOID StartParameter,
    _In_opt_ ULONG MaxThreadCount,
    _In_opt_ SIZE_T StackReserve,
    _In_opt_ SIZE_T StackCommit
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationWorkerFactory(
    _In_ HANDLE WorkerFactoryHandle,
    _In_ WORKERFACTORYINFOCLASS WorkerFactoryInformationClass,
    _Out_writes_bytes_(WorkerFactoryInformationLength) PVOID WorkerFactoryInformation,
    _In_ ULONG WorkerFactoryInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationWorkerFactory(
    _In_ HANDLE WorkerFactoryHandle,
    _In_ WORKERFACTORYINFOCLASS WorkerFactoryInformationClass,
    _In_reads_bytes_(WorkerFactoryInformationLength) PVOID WorkerFactoryInformation,
    _In_ ULONG WorkerFactoryInformationLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtShutdownWorkerFactory(
    _In_ HANDLE WorkerFactoryHandle,
    _Inout_ volatile LONG *PendingWorkerCount
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseWorkerFactoryWorker(
    _In_ HANDLE WorkerFactoryHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWorkerFactoryWorkerReady(
    _In_ HANDLE WorkerFactoryHandle
    );

struct _FILE_IO_COMPLETION_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForWorkViaWorkerFactory(
    _In_ HANDLE WorkerFactoryHandle,
    _Out_ struct _FILE_IO_COMPLETION_INFORMATION *MiniPacket
    );

#endif

// Time

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemTime(
    _Out_ PLARGE_INTEGER SystemTime
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemTime(
    _In_opt_ PLARGE_INTEGER SystemTime,
    _Out_opt_ PLARGE_INTEGER PreviousTime
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryTimerResolution(
    _Out_ PULONG MaximumTime,
    _Out_ PULONG MinimumTime,
    _Out_ PULONG CurrentTime
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetTimerResolution(
    _In_ ULONG DesiredTime,
    _In_ BOOLEAN SetResolution,
    _Out_ PULONG ActualTime
    );

// Performance Counter

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryPerformanceCounter(
    _Out_ PLARGE_INTEGER PerformanceCounter,
    _Out_opt_ PLARGE_INTEGER PerformanceFrequency
    );

// LUIDs

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateLocallyUniqueId(
    _Out_ PLUID Luid
    );

// UUIDs

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetUuidSeed(
    _In_ PCHAR Seed
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateUuids(
    _Out_ PULARGE_INTEGER Time,
    _Out_ PULONG Range,
    _Out_ PULONG Sequence,
    _Out_ PCHAR Seed
    );

// System Information

#endif // (PHNT_MODE != PHNT_MODE_KERNEL)

// rev
// private
// source:http://www.microsoft.com/whdc/system/Sysinternals/MoreThan64proc.mspx
typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemBasicInformation, // q: SYSTEM_BASIC_INFORMATION
    SystemProcessorInformation, // q: SYSTEM_PROCESSOR_INFORMATION
    SystemPerformanceInformation, // q: SYSTEM_PERFORMANCE_INFORMATION
    SystemTimeOfDayInformation, // q: SYSTEM_TIMEOFDAY_INFORMATION
    SystemPathInformation, // not implemented
    SystemProcessInformation, // q: SYSTEM_PROCESS_INFORMATION
    SystemCallCountInformation, // q: SYSTEM_CALL_COUNT_INFORMATION
    SystemDeviceInformation, // q: SYSTEM_DEVICE_INFORMATION
    SystemProcessorPerformanceInformation, // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION
    SystemFlagsInformation, // q: SYSTEM_FLAGS_INFORMATION
    SystemCallTimeInformation, // 10, not implemented
    SystemModuleInformation, // q: RTL_PROCESS_MODULES
    SystemLocksInformation,
    SystemStackTraceInformation,
    SystemPagedPoolInformation, // not implemented
    SystemNonPagedPoolInformation, // not implemented
    SystemHandleInformation, // q: SYSTEM_HANDLE_INFORMATION
    SystemObjectInformation, // q: SYSTEM_OBJECTTYPE_INFORMATION mixed with SYSTEM_OBJECT_INFORMATION
    SystemPageFileInformation, // q: SYSTEM_PAGEFILE_INFORMATION
    SystemVdmInstemulInformation, // q
    SystemVdmBopInformation, // 20, not implemented
    SystemFileCacheInformation, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemCache)
    SystemPoolTagInformation, // q: SYSTEM_POOLTAG_INFORMATION
    SystemInterruptInformation, // q: SYSTEM_INTERRUPT_INFORMATION
    SystemDpcBehaviorInformation, // q: SYSTEM_DPC_BEHAVIOR_INFORMATION; s: SYSTEM_DPC_BEHAVIOR_INFORMATION (requires SeLoadDriverPrivilege)
    SystemFullMemoryInformation, // not implemented
    SystemLoadGdiDriverInformation, // s (kernel-mode only)
    SystemUnloadGdiDriverInformation, // s (kernel-mode only)
    SystemTimeAdjustmentInformation, // q: SYSTEM_QUERY_TIME_ADJUST_INFORMATION; s: SYSTEM_SET_TIME_ADJUST_INFORMATION (requires SeSystemtimePrivilege)
    SystemSummaryMemoryInformation, // not implemented
    SystemMirrorMemoryInformation, // 30, s (requires license value "Kernel-MemoryMirroringSupported") (requires SeShutdownPrivilege)
    SystemPerformanceTraceInformation, // s
    SystemObsolete0, // not implemented
    SystemExceptionInformation, // q: SYSTEM_EXCEPTION_INFORMATION
    SystemCrashDumpStateInformation, // s (requires SeDebugPrivilege)
    SystemKernelDebuggerInformation, // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION
    SystemContextSwitchInformation, // q: SYSTEM_CONTEXT_SWITCH_INFORMATION
    SystemRegistryQuotaInformation, // q: SYSTEM_REGISTRY_QUOTA_INFORMATION; s (requires SeIncreaseQuotaPrivilege)
    SystemExtendServiceTableInformation, // s (requires SeLoadDriverPrivilege) // loads win32k only
    SystemPrioritySeperation, // s (requires SeTcbPrivilege)
    SystemVerifierAddDriverInformation, // 40, s (requires SeDebugPrivilege)
    SystemVerifierRemoveDriverInformation, // s (requires SeDebugPrivilege)
    SystemProcessorIdleInformation, // q: SYSTEM_PROCESSOR_IDLE_INFORMATION
    SystemLegacyDriverInformation, // q: SYSTEM_LEGACY_DRIVER_INFORMATION
    SystemCurrentTimeZoneInformation, // q
    SystemLookasideInformation, // q: SYSTEM_LOOKASIDE_INFORMATION
    SystemTimeSlipNotification, // s (requires SeSystemtimePrivilege)
    SystemSessionCreate, // not implemented
    SystemSessionDetach, // not implemented
    SystemSessionInformation, // not implemented
    SystemRangeStartInformation, // 50, q
    SystemVerifierInformation, // q: SYSTEM_VERIFIER_INFORMATION; s (requires SeDebugPrivilege)
    SystemVerifierThunkExtend, // s (kernel-mode only)
    SystemSessionProcessInformation, // q: SYSTEM_SESSION_PROCESS_INFORMATION
    SystemLoadGdiDriverInSystemSpace, // s (kernel-mode only) (same as SystemLoadGdiDriverInformation)
    SystemNumaProcessorMap, // q
    SystemPrefetcherInformation, // q: PREFETCHER_INFORMATION; s: PREFETCHER_INFORMATION // PfSnQueryPrefetcherInformation
    SystemExtendedProcessInformation, // q: SYSTEM_PROCESS_INFORMATION
    SystemRecommendedSharedDataAlignment, // q
    SystemComPlusPackage, // q; s
    SystemNumaAvailableMemory, // 60
    SystemProcessorPowerInformation, // q: SYSTEM_PROCESSOR_POWER_INFORMATION
    SystemEmulationBasicInformation, // q
    SystemEmulationProcessorInformation,
    SystemExtendedHandleInformation, // q: SYSTEM_HANDLE_INFORMATION_EX
    SystemLostDelayedWriteInformation, // q: ULONG
    SystemBigPoolInformation, // q: SYSTEM_BIGPOOL_INFORMATION
    SystemSessionPoolTagInformation, // q: SYSTEM_SESSION_POOLTAG_INFORMATION
    SystemSessionMappedViewInformation, // q: SYSTEM_SESSION_MAPPED_VIEW_INFORMATION
    SystemHotpatchInformation, // q; s
    SystemObjectSecurityMode, // 70, q
    SystemWatchdogTimerHandler, // s (kernel-mode only)
    SystemWatchdogTimerInformation, // q (kernel-mode only); s (kernel-mode only)
    SystemLogicalProcessorInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION
    SystemWow64SharedInformationObsolete, // not implemented
    SystemRegisterFirmwareTableInformationHandler, // s (kernel-mode only)
    SystemFirmwareTableInformation, // not implemented
    SystemModuleInformationEx, // q: RTL_PROCESS_MODULE_INFORMATION_EX
    SystemVerifierTriageInformation, // not implemented
    SystemSuperfetchInformation, // q: SUPERFETCH_INFORMATION; s: SUPERFETCH_INFORMATION // PfQuerySuperfetchInformation
    SystemMemoryListInformation, // 80, q: SYSTEM_MEMORY_LIST_INFORMATION; s: SYSTEM_MEMORY_LIST_COMMAND (requires SeProfileSingleProcessPrivilege)
    SystemFileCacheInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (same as SystemFileCacheInformation)
    SystemThreadPriorityClientIdInformation, // s: SYSTEM_THREAD_CID_PRIORITY_INFORMATION (requires SeIncreaseBasePriorityPrivilege)
    SystemProcessorIdleCycleTimeInformation, // q: SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION[]
    SystemVerifierCancellationInformation, // not implemented // name:wow64:whNT32QuerySystemVerifierCancellationInformation
    SystemProcessorPowerInformationEx, // not implemented
    SystemRefTraceInformation, // q; s // ObQueryRefTraceInformation
    SystemSpecialPoolInformation, // q; s (requires SeDebugPrivilege) // MmSpecialPoolTag, then MmSpecialPoolCatchOverruns != 0
    SystemProcessIdInformation, // q: SYSTEM_PROCESS_ID_INFORMATION
    SystemErrorPortInformation, // s (requires SeTcbPrivilege)
    SystemBootEnvironmentInformation, // 90, q: SYSTEM_BOOT_ENVIRONMENT_INFORMATION
    SystemHypervisorInformation, // q; s (kernel-mode only)
    SystemVerifierInformationEx, // q; s
    SystemTimeZoneInformation, // s (requires SeTimeZonePrivilege)
    SystemImageFileExecutionOptionsInformation, // s: SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION (requires SeTcbPrivilege)
    SystemCoverageInformation, // q; s // name:wow64:whNT32QuerySystemCoverageInformation; ExpCovQueryInformation
    SystemPrefetchPatchInformation, // not implemented
    SystemVerifierFaultsInformation, // s (requires SeDebugPrivilege)
    SystemSystemPartitionInformation, // q: SYSTEM_SYSTEM_PARTITION_INFORMATION
    SystemSystemDiskInformation, // q: SYSTEM_SYSTEM_DISK_INFORMATION
    SystemProcessorPerformanceDistribution, // 100, q: SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION
    SystemNumaProximityNodeInformation, // q
    SystemDynamicTimeZoneInformation, // q; s (requires SeTimeZonePrivilege)
    SystemCodeIntegrityInformation, // q // SeCodeIntegrityQueryInformation
    SystemProcessorMicrocodeUpdateInformation, // s
    SystemProcessorBrandString, // q // HaliQuerySystemInformation -> HalpGetProcessorBrandString, info class 23
    SystemVirtualAddressInformation, // q: SYSTEM_VA_LIST_INFORMATION[]; s: SYSTEM_VA_LIST_INFORMATION[] (requires SeIncreaseQuotaPrivilege) // MmQuerySystemVaInformation
    SystemLogicalProcessorAndGroupInformation, // q: SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX // since WIN7 // KeQueryLogicalProcessorRelationship
    SystemProcessorCycleTimeInformation, // q: SYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION[]
    SystemStoreInformation, // q; s // SmQueryStoreInformation
    SystemRegistryAppendString, // 110, s: SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS
    SystemAitSamplingValue, // s: ULONG (requires SeProfileSingleProcessPrivilege)
    SystemVhdBootInformation, // q: SYSTEM_VHD_BOOT_INFORMATION
    SystemCpuQuotaInformation, // q; s // PsQueryCpuQuotaInformation
    SystemNativeBasicInformation, // not implemented
    SystemSpare1, // not implemented
    SystemLowPriorityIoInformation, // q: SYSTEM_LOW_PRIORITY_IO_INFORMATION
    SystemTpmBootEntropyInformation, // q: TPM_BOOT_ENTROPY_NT_RESULT // ExQueryTpmBootEntropyInformation
    SystemVerifierCountersInformation, // q: SYSTEM_VERIFIER_COUNTERS_INFORMATION
    SystemPagedPoolInformationEx, // q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypePagedPool)
    SystemSystemPtesInformationEx, // 120, q: SYSTEM_FILECACHE_INFORMATION; s (requires SeIncreaseQuotaPrivilege) (info for WorkingSetTypeSystemPtes)
    SystemNodeDistanceInformation, // q
    SystemAcpiAuditInformation, // q: SYSTEM_ACPI_AUDIT_INFORMATION // HaliQuerySystemInformation -> HalpAuditQueryResults, info class 26
    SystemBasicPerformanceInformation, // q: SYSTEM_BASIC_PERFORMANCE_INFORMATION // name:wow64:whNtQuerySystemInformation_SystemBasicPerformanceInformation
    SystemQueryPerformanceCounterInformation, // q: SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION // since WIN7 SP1
    SystemSessionBigPoolInformation, // since WIN8
    SystemBootGraphicsInformation,
    SystemScrubPhysicalMemoryInformation,
    SystemBadPageInformation,
    SystemProcessorProfileControlArea,
    SystemCombinePhysicalMemoryInformation, // 130
    SystemEntropyInterruptTimingCallback,
    SystemConsoleInformation,
    SystemPlatformBinaryInformation,
    SystemThrottleNotificationInformation,
    SystemHypervisorProcessorCountInformation,
    SystemDeviceDataInformation,
    SystemDeviceDataEnumerationInformation,
    SystemMemoryTopologyInformation,
    SystemMemoryChannelInformation,
    SystemBootLogoInformation, // 140
    SystemProcessorPerformanceInformationEx, // q: SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX // since WINBLUE
    SystemSpare0,
    SystemSecureBootPolicyInformation,
    SystemPageFileInformationEx, // q: SYSTEM_PAGEFILE_INFORMATION_EX
    SystemSecureBootInformation,
    SystemEntropyInterruptTimingRawInformation,
    SystemPortableWorkspaceEfiLauncherInformation,
    SystemFullProcessInformation, // q: SYSTEM_PROCESS_INFORMATION with SYSTEM_PROCESS_INFORMATION_EXTENSION (requires admin)
    SystemKernelDebuggerInformationEx, // q: SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX
    SystemBootMetadataInformation, // 150
    SystemSoftRebootInformation,
    SystemElamCertificateInformation,
    SystemOfflineDumpConfigInformation,
    SystemProcessorFeaturesInformation, // q: SYSTEM_PROCESSOR_FEATURES_INFORMATION
    SystemRegistryReconciliationInformation,
    SystemEdidInformation,
    SystemManufacturingInformation, // q: SYSTEM_MANUFACTURING_INFORMATION // since THRESHOLD
    SystemEnergyEstimationConfigInformation, // q: SYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION
    SystemHypervisorDetailInformation, // q: SYSTEM_HYPERVISOR_DETAIL_INFORMATION
    SystemProcessorCycleStatsInformation, // q: SYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION // 160
    SystemVmGenerationCountInformation,
    SystemTrustedPlatformModuleInformation, // q: SYSTEM_TPM_INFORMATION
    SystemKernelDebuggerFlags,
    SystemCodeIntegrityPolicyInformation,
    SystemIsolatedUserModeInformation,
    SystemHardwareSecurityTestInterfaceResultsInformation,
    SystemSingleModuleInformation, // q: SYSTEM_SINGLE_MODULE_INFORMATION
    SystemAllowedCpuSetsInformation,
    SystemDmaProtectionInformation,
    SystemInterruptCpuSetsInformation,
    SystemSecureBootPolicyFullInformation,
    SystemCodeIntegrityPolicyFullInformation,
    SystemAffinitizedInterruptProcessorInformation,
    SystemRootSiloInformation, // q: SYSTEM_ROOT_SILO_INFORMATION
    MaxSystemInfoClass
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_BASIC_INFORMATION
{
    ULONG Reserved;
    ULONG TimerResolution;
    ULONG PageSize;
    ULONG NumberOfPhysicalPages;
    ULONG LowestPhysicalPageNumber;
    ULONG HighestPhysicalPageNumber;
    ULONG AllocationGranularity;
    ULONG_PTR MinimumUserModeAddress;
    ULONG_PTR MaximumUserModeAddress;
    ULONG_PTR ActiveProcessorsAffinityMask;
    CCHAR NumberOfProcessors;
} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

typedef struct _SYSTEM_PROCESSOR_INFORMATION
{
    USHORT ProcessorArchitecture;
    USHORT ProcessorLevel;
    USHORT ProcessorRevision;
    USHORT Reserved;
    ULONG ProcessorFeatureBits;
} SYSTEM_PROCESSOR_INFORMATION, *PSYSTEM_PROCESSOR_INFORMATION;

typedef struct _SYSTEM_PERFORMANCE_INFORMATION
{
    LARGE_INTEGER IdleProcessTime;
    LARGE_INTEGER IoReadTransferCount;
    LARGE_INTEGER IoWriteTransferCount;
    LARGE_INTEGER IoOtherTransferCount;
    ULONG IoReadOperationCount;
    ULONG IoWriteOperationCount;
    ULONG IoOtherOperationCount;
    ULONG AvailablePages;
    ULONG CommittedPages;
    ULONG CommitLimit;
    ULONG PeakCommitment;
    ULONG PageFaultCount;
    ULONG CopyOnWriteCount;
    ULONG TransitionCount;
    ULONG CacheTransitionCount;
    ULONG DemandZeroCount;
    ULONG PageReadCount;
    ULONG PageReadIoCount;
    ULONG CacheReadCount;
    ULONG CacheIoCount;
    ULONG DirtyPagesWriteCount;
    ULONG DirtyWriteIoCount;
    ULONG MappedPagesWriteCount;
    ULONG MappedWriteIoCount;
    ULONG PagedPoolPages;
    ULONG NonPagedPoolPages;
    ULONG PagedPoolAllocs;
    ULONG PagedPoolFrees;
    ULONG NonPagedPoolAllocs;
    ULONG NonPagedPoolFrees;
    ULONG FreeSystemPtes;
    ULONG ResidentSystemCodePage;
    ULONG TotalSystemDriverPages;
    ULONG TotalSystemCodePages;
    ULONG NonPagedPoolLookasideHits;
    ULONG PagedPoolLookasideHits;
    ULONG AvailablePagedPoolPages;
    ULONG ResidentSystemCachePage;
    ULONG ResidentPagedPoolPage;
    ULONG ResidentSystemDriverPage;
    ULONG CcFastReadNoWait;
    ULONG CcFastReadWait;
    ULONG CcFastReadResourceMiss;
    ULONG CcFastReadNotPossible;
    ULONG CcFastMdlReadNoWait;
    ULONG CcFastMdlReadWait;
    ULONG CcFastMdlReadResourceMiss;
    ULONG CcFastMdlReadNotPossible;
    ULONG CcMapDataNoWait;
    ULONG CcMapDataWait;
    ULONG CcMapDataNoWaitMiss;
    ULONG CcMapDataWaitMiss;
    ULONG CcPinMappedDataCount;
    ULONG CcPinReadNoWait;
    ULONG CcPinReadWait;
    ULONG CcPinReadNoWaitMiss;
    ULONG CcPinReadWaitMiss;
    ULONG CcCopyReadNoWait;
    ULONG CcCopyReadWait;
    ULONG CcCopyReadNoWaitMiss;
    ULONG CcCopyReadWaitMiss;
    ULONG CcMdlReadNoWait;
    ULONG CcMdlReadWait;
    ULONG CcMdlReadNoWaitMiss;
    ULONG CcMdlReadWaitMiss;
    ULONG CcReadAheadIos;
    ULONG CcLazyWriteIos;
    ULONG CcLazyWritePages;
    ULONG CcDataFlushes;
    ULONG CcDataPages;
    ULONG ContextSwitches;
    ULONG FirstLevelTbFills;
    ULONG SecondLevelTbFills;
    ULONG SystemCalls;
} SYSTEM_PERFORMANCE_INFORMATION, *PSYSTEM_PERFORMANCE_INFORMATION;

typedef struct _SYSTEM_TIMEOFDAY_INFORMATION
{
    LARGE_INTEGER BootTime;
    LARGE_INTEGER CurrentTime;
    LARGE_INTEGER TimeZoneBias;
    ULONG TimeZoneId;
    ULONG Reserved;
    ULONGLONG BootTimeBias;
    ULONGLONG SleepTimeBias;
} SYSTEM_TIMEOFDAY_INFORMATION, *PSYSTEM_TIMEOFDAY_INFORMATION;

typedef struct _SYSTEM_THREAD_INFORMATION
{
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    ULONG ThreadState;
    KWAIT_REASON WaitReason;
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

typedef struct _TEB *PTEB;

// private
typedef struct _SYSTEM_EXTENDED_THREAD_INFORMATION
{
    SYSTEM_THREAD_INFORMATION ThreadInfo;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID Win32StartAddress;
    PTEB TebBase; // since VISTA
    ULONG_PTR Reserved2;
    ULONG_PTR Reserved3;
    ULONG_PTR Reserved4;
} SYSTEM_EXTENDED_THREAD_INFORMATION, *PSYSTEM_EXTENDED_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize; // since VISTA
    ULONG HardFaultCount; // since WIN7
    ULONG NumberOfThreadsHighWatermark; // since WIN7
    ULONGLONG CycleTime; // since WIN7
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR UniqueProcessKey; // since VISTA (requires SystemExtendedProcessInformation)
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    SYSTEM_THREAD_INFORMATION Threads[1];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef struct _SYSTEM_CALL_COUNT_INFORMATION
{
    ULONG Length;
    ULONG NumberOfTables;
} SYSTEM_CALL_COUNT_INFORMATION, *PSYSTEM_CALL_COUNT_INFORMATION;

typedef struct _SYSTEM_DEVICE_INFORMATION
{
    ULONG NumberOfDisks;
    ULONG NumberOfFloppies;
    ULONG NumberOfCdRoms;
    ULONG NumberOfTapes;
    ULONG NumberOfSerialPorts;
    ULONG NumberOfParallelPorts;
} SYSTEM_DEVICE_INFORMATION, *PSYSTEM_DEVICE_INFORMATION;

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION
{
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG InterruptCount;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef struct _SYSTEM_FLAGS_INFORMATION
{
    ULONG Flags; // NtGlobalFlag
} SYSTEM_FLAGS_INFORMATION, *PSYSTEM_FLAGS_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
    USHORT UniqueProcessId;
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;
    UCHAR HandleAttributes;
    USHORT HandleValue;
    PVOID Object;
    ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG NumberOfHandles;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct _SYSTEM_OBJECTTYPE_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfObjects;
    ULONG NumberOfHandles;
    ULONG TypeIndex;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG ValidAccessMask;
    ULONG PoolType;
    BOOLEAN SecurityRequired;
    BOOLEAN WaitableObject;
    UNICODE_STRING TypeName;
} SYSTEM_OBJECTTYPE_INFORMATION, *PSYSTEM_OBJECTTYPE_INFORMATION;

typedef struct _SYSTEM_OBJECT_INFORMATION
{
    ULONG NextEntryOffset;
    PVOID Object;
    HANDLE CreatorUniqueProcess;
    USHORT CreatorBackTraceIndex;
    USHORT Flags;
    LONG PointerCount;
    LONG HandleCount;
    ULONG PagedPoolCharge;
    ULONG NonPagedPoolCharge;
    HANDLE ExclusiveProcessId;
    PVOID SecurityDescriptor;
    UNICODE_STRING NameInfo;
} SYSTEM_OBJECT_INFORMATION, *PSYSTEM_OBJECT_INFORMATION;

typedef struct _SYSTEM_PAGEFILE_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG TotalSize;
    ULONG TotalInUse;
    ULONG PeakUsage;
    UNICODE_STRING PageFileName;
} SYSTEM_PAGEFILE_INFORMATION, *PSYSTEM_PAGEFILE_INFORMATION;

#define MM_WORKING_SET_MAX_HARD_ENABLE 0x1
#define MM_WORKING_SET_MAX_HARD_DISABLE 0x2
#define MM_WORKING_SET_MIN_HARD_ENABLE 0x4
#define MM_WORKING_SET_MIN_HARD_DISABLE 0x8

typedef struct _SYSTEM_FILECACHE_INFORMATION
{
    SIZE_T CurrentSize;
    SIZE_T PeakSize;
    ULONG PageFaultCount;
    SIZE_T MinimumWorkingSet;
    SIZE_T MaximumWorkingSet;
    SIZE_T CurrentSizeIncludingTransitionInPages;
    SIZE_T PeakSizeIncludingTransitionInPages;
    ULONG TransitionRePurposeCount;
    ULONG Flags;
} SYSTEM_FILECACHE_INFORMATION, *PSYSTEM_FILECACHE_INFORMATION;

// Can be used instead of SYSTEM_FILECACHE_INFORMATION
typedef struct _SYSTEM_BASIC_WORKING_SET_INFORMATION
{
    SIZE_T CurrentSize;
    SIZE_T PeakSize;
    ULONG PageFaultCount;
} SYSTEM_BASIC_WORKING_SET_INFORMATION, *PSYSTEM_BASIC_WORKING_SET_INFORMATION;

typedef struct _SYSTEM_POOLTAG
{
    union
    {
        UCHAR Tag[4];
        ULONG TagUlong;
    };
    ULONG PagedAllocs;
    ULONG PagedFrees;
    SIZE_T PagedUsed;
    ULONG NonPagedAllocs;
    ULONG NonPagedFrees;
    SIZE_T NonPagedUsed;
} SYSTEM_POOLTAG, *PSYSTEM_POOLTAG;

typedef struct _SYSTEM_POOLTAG_INFORMATION
{
    ULONG Count;
    SYSTEM_POOLTAG TagInfo[1];
} SYSTEM_POOLTAG_INFORMATION, *PSYSTEM_POOLTAG_INFORMATION;

typedef struct _SYSTEM_INTERRUPT_INFORMATION
{
    ULONG ContextSwitches;
    ULONG DpcCount;
    ULONG DpcRate;
    ULONG TimeIncrement;
    ULONG DpcBypassCount;
    ULONG ApcBypassCount;
} SYSTEM_INTERRUPT_INFORMATION, *PSYSTEM_INTERRUPT_INFORMATION;

typedef struct _SYSTEM_DPC_BEHAVIOR_INFORMATION
{
    ULONG Spare;
    ULONG DpcQueueDepth;
    ULONG MinimumDpcRate;
    ULONG AdjustDpcThreshold;
    ULONG IdealDpcRate;
} SYSTEM_DPC_BEHAVIOR_INFORMATION, *PSYSTEM_DPC_BEHAVIOR_INFORMATION;

typedef struct _SYSTEM_QUERY_TIME_ADJUST_INFORMATION
{
    ULONG TimeAdjustment;
    ULONG TimeIncrement;
    BOOLEAN Enable;
} SYSTEM_QUERY_TIME_ADJUST_INFORMATION, *PSYSTEM_QUERY_TIME_ADJUST_INFORMATION;

typedef struct _SYSTEM_SET_TIME_ADJUST_INFORMATION
{
    ULONG TimeAdjustment;
    BOOLEAN Enable;
} SYSTEM_SET_TIME_ADJUST_INFORMATION, *PSYSTEM_SET_TIME_ADJUST_INFORMATION;

typedef struct _SYSTEM_EXCEPTION_INFORMATION
{
    ULONG AlignmentFixupCount;
    ULONG ExceptionDispatchCount;
    ULONG FloatingEmulationCount;
    ULONG ByteWordEmulationCount;
} SYSTEM_EXCEPTION_INFORMATION, *PSYSTEM_EXCEPTION_INFORMATION;

typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION
{
    BOOLEAN KernelDebuggerEnabled;
    BOOLEAN KernelDebuggerNotPresent;
} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

typedef struct _SYSTEM_CONTEXT_SWITCH_INFORMATION
{
    ULONG ContextSwitches;
    ULONG FindAny;
    ULONG FindLast;
    ULONG FindIdeal;
    ULONG IdleAny;
    ULONG IdleCurrent;
    ULONG IdleLast;
    ULONG IdleIdeal;
    ULONG PreemptAny;
    ULONG PreemptCurrent;
    ULONG PreemptLast;
    ULONG SwitchToIdle;
} SYSTEM_CONTEXT_SWITCH_INFORMATION, *PSYSTEM_CONTEXT_SWITCH_INFORMATION;

typedef struct _SYSTEM_REGISTRY_QUOTA_INFORMATION
{
    ULONG RegistryQuotaAllowed;
    ULONG RegistryQuotaUsed;
    SIZE_T PagedPoolSize;
} SYSTEM_REGISTRY_QUOTA_INFORMATION, *PSYSTEM_REGISTRY_QUOTA_INFORMATION;

typedef struct _SYSTEM_PROCESSOR_IDLE_INFORMATION
{
    ULONGLONG IdleTime;
    ULONGLONG C1Time;
    ULONGLONG C2Time;
    ULONGLONG C3Time;
    ULONG C1Transitions;
    ULONG C2Transitions;
    ULONG C3Transitions;
    ULONG Padding;
} SYSTEM_PROCESSOR_IDLE_INFORMATION, *PSYSTEM_PROCESSOR_IDLE_INFORMATION;

typedef struct _SYSTEM_LEGACY_DRIVER_INFORMATION
{
    ULONG VetoType;
    UNICODE_STRING VetoList;
} SYSTEM_LEGACY_DRIVER_INFORMATION, *PSYSTEM_LEGACY_DRIVER_INFORMATION;

typedef struct _SYSTEM_LOOKASIDE_INFORMATION
{
    USHORT CurrentDepth;
    USHORT MaximumDepth;
    ULONG TotalAllocates;
    ULONG AllocateMisses;
    ULONG TotalFrees;
    ULONG FreeMisses;
    ULONG Type;
    ULONG Tag;
    ULONG Size;
} SYSTEM_LOOKASIDE_INFORMATION, *PSYSTEM_LOOKASIDE_INFORMATION;

typedef struct _SYSTEM_VERIFIER_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG Level;
    UNICODE_STRING DriverName;

    ULONG RaiseIrqls;
    ULONG AcquireSpinLocks;
    ULONG SynchronizeExecutions;
    ULONG AllocationsAttempted;

    ULONG AllocationsSucceeded;
    ULONG AllocationsSucceededSpecialPool;
    ULONG AllocationsWithNoTag;
    ULONG TrimRequests;

    ULONG Trims;
    ULONG AllocationsFailed;
    ULONG AllocationsFailedDeliberately;
    ULONG Loads;

    ULONG Unloads;
    ULONG UnTrackedPool;
    ULONG CurrentPagedPoolAllocations;
    ULONG CurrentNonPagedPoolAllocations;

    ULONG PeakPagedPoolAllocations;
    ULONG PeakNonPagedPoolAllocations;

    SIZE_T PagedPoolUsageInBytes;
    SIZE_T NonPagedPoolUsageInBytes;
    SIZE_T PeakPagedPoolUsageInBytes;
    SIZE_T PeakNonPagedPoolUsageInBytes;
} SYSTEM_VERIFIER_INFORMATION, *PSYSTEM_VERIFIER_INFORMATION;

typedef struct _SYSTEM_SESSION_PROCESS_INFORMATION
{
    ULONG SessionId;
    ULONG SizeOfBuf;
    PVOID Buffer;
} SYSTEM_SESSION_PROCESS_INFORMATION, *PSYSTEM_SESSION_PROCESS_INFORMATION;

typedef struct _SYSTEM_PROCESSOR_POWER_INFORMATION
{
    UCHAR CurrentFrequency;
    UCHAR ThermalLimitFrequency;
    UCHAR ConstantThrottleFrequency;
    UCHAR DegradedThrottleFrequency;
    UCHAR LastBusyFrequency;
    UCHAR LastC3Frequency;
    UCHAR LastAdjustedBusyFrequency;
    UCHAR ProcessorMinThrottle;
    UCHAR ProcessorMaxThrottle;
    ULONG NumberOfFrequencies;
    ULONG PromotionCount;
    ULONG DemotionCount;
    ULONG ErrorCount;
    ULONG RetryCount;
    ULONGLONG CurrentFrequencyTime;
    ULONGLONG CurrentProcessorTime;
    ULONGLONG CurrentProcessorIdleTime;
    ULONGLONG LastProcessorTime;
    ULONGLONG LastProcessorIdleTime;
} SYSTEM_PROCESSOR_POWER_INFORMATION, *PSYSTEM_PROCESSOR_POWER_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX
{
    PVOID Object;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR HandleValue;
    ULONG GrantedAccess;
    USHORT CreatorBackTraceIndex;
    USHORT ObjectTypeIndex;
    ULONG HandleAttributes;
    ULONG Reserved;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX;

typedef struct _SYSTEM_HANDLE_INFORMATION_EX
{
    ULONG_PTR NumberOfHandles;
    ULONG_PTR Reserved;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handles[1];
} SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;

typedef struct _SYSTEM_BIGPOOL_ENTRY
{
    union
    {
        PVOID VirtualAddress;
        ULONG_PTR NonPaged : 1;
    };
    SIZE_T SizeInBytes;
    union
    {
        UCHAR Tag[4];
        ULONG TagUlong;
    };
} SYSTEM_BIGPOOL_ENTRY, *PSYSTEM_BIGPOOL_ENTRY;

typedef struct _SYSTEM_BIGPOOL_INFORMATION
{
    ULONG Count;
    SYSTEM_BIGPOOL_ENTRY AllocatedInfo[1];
} SYSTEM_BIGPOOL_INFORMATION, *PSYSTEM_BIGPOOL_INFORMATION;

typedef struct _SYSTEM_POOL_ENTRY
{
    BOOLEAN Allocated;
    BOOLEAN Spare0;
    USHORT AllocatorBackTraceIndex;
    ULONG Size;
    union
    {
        UCHAR Tag[4];
        ULONG TagUlong;
        PVOID ProcessChargedQuota;
    };
} SYSTEM_POOL_ENTRY, *PSYSTEM_POOL_ENTRY;

typedef struct _SYSTEM_POOL_INFORMATION
{
    SIZE_T TotalSize;
    PVOID FirstEntry;
    USHORT EntryOverhead;
    BOOLEAN PoolTagPresent;
    BOOLEAN Spare0;
    ULONG NumberOfEntries;
    SYSTEM_POOL_ENTRY Entries[1];
} SYSTEM_POOL_INFORMATION, *PSYSTEM_POOL_INFORMATION;

typedef struct _SYSTEM_SESSION_POOLTAG_INFORMATION
{
    SIZE_T NextEntryOffset;
    ULONG SessionId;
    ULONG Count;
    SYSTEM_POOLTAG TagInfo[1];
} SYSTEM_SESSION_POOLTAG_INFORMATION, *PSYSTEM_SESSION_POOLTAG_INFORMATION;

typedef struct _SYSTEM_SESSION_MAPPED_VIEW_INFORMATION
{
    SIZE_T NextEntryOffset;
    ULONG SessionId;
    ULONG ViewFailures;
    SIZE_T NumberOfBytesAvailable;
    SIZE_T NumberOfBytesAvailableContiguous;
} SYSTEM_SESSION_MAPPED_VIEW_INFORMATION, *PSYSTEM_SESSION_MAPPED_VIEW_INFORMATION;

// private
typedef struct _SYSTEM_MEMORY_LIST_INFORMATION
{
    ULONG_PTR ZeroPageCount;
    ULONG_PTR FreePageCount;
    ULONG_PTR ModifiedPageCount;
    ULONG_PTR ModifiedNoWritePageCount;
    ULONG_PTR BadPageCount;
    ULONG_PTR PageCountByPriority[8];
    ULONG_PTR RepurposedPagesByPriority[8];
    ULONG_PTR ModifiedPageCountPageFile;
} SYSTEM_MEMORY_LIST_INFORMATION, *PSYSTEM_MEMORY_LIST_INFORMATION;

// private
typedef enum _SYSTEM_MEMORY_LIST_COMMAND
{
    MemoryCaptureAccessedBits,
    MemoryCaptureAndResetAccessedBits,
    MemoryEmptyWorkingSets,
    MemoryFlushModifiedList,
    MemoryPurgeStandbyList,
    MemoryPurgeLowPriorityStandbyList,
    MemoryCommandMax
} SYSTEM_MEMORY_LIST_COMMAND;

// private
typedef struct _SYSTEM_THREAD_CID_PRIORITY_INFORMATION
{
    CLIENT_ID ClientId;
    KPRIORITY Priority;
} SYSTEM_THREAD_CID_PRIORITY_INFORMATION, *PSYSTEM_THREAD_CID_PRIORITY_INFORMATION;

// private
typedef struct _SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION
{
    ULONGLONG CycleTime;
} SYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION, *PSYSTEM_PROCESSOR_IDLE_CYCLE_TIME_INFORMATION;

// private
typedef struct _SYSTEM_REF_TRACE_INFORMATION
{
    BOOLEAN TraceEnable;
    BOOLEAN TracePermanent;
    UNICODE_STRING TraceProcessName;
    UNICODE_STRING TracePoolTags;
} SYSTEM_REF_TRACE_INFORMATION, *PSYSTEM_REF_TRACE_INFORMATION;

// private
typedef struct _SYSTEM_PROCESS_ID_INFORMATION
{
    HANDLE ProcessId;
    UNICODE_STRING ImageName;
} SYSTEM_PROCESS_ID_INFORMATION, *PSYSTEM_PROCESS_ID_INFORMATION;

#if (PHNT_MODE == PHNT_MODE_KERNEL)
typedef enum _FIRMWARE_TYPE
{
    FirmwareTypeUnknown,
    FirmwareTypeBios,
    FirmwareTypeUefi,
    FirmwareTypeMax
} FIRMWARE_TYPE, *PFIRMWARE_TYPE;
#endif

// private
typedef struct _SYSTEM_BOOT_ENVIRONMENT_INFORMATION
{
    GUID BootIdentifier;
    FIRMWARE_TYPE FirmwareType;
} SYSTEM_BOOT_ENVIRONMENT_INFORMATION, PSYSTEM_BOOT_ENVIRONMENT_INFORMATION;

// private
typedef struct _SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION
{
    ULONG FlagsToEnable;
    ULONG FlagsToDisable;
} SYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION, *PSYSTEM_IMAGE_FILE_EXECUTION_OPTIONS_INFORMATION;

// private
typedef struct _SYSTEM_SYSTEM_PARTITION_INFORMATION
{
    UNICODE_STRING SystemPartition;
} SYSTEM_SYSTEM_PARTITION_INFORMATION, *PSYSTEM_SYSTEM_PARTITION_INFORMATION;

// private
typedef struct _SYSTEM_SYSTEM_DISK_INFORMATION
{
    UNICODE_STRING SystemDisk;
} SYSTEM_SYSTEM_DISK_INFORMATION, *PSYSTEM_SYSTEM_DISK_INFORMATION;

// private
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT
{
    LARGE_INTEGER Hits; // ULONG in WIN8
    UCHAR PercentFrequency;
} SYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT, *PSYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT;

// private
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION
{
    ULONG ProcessorNumber;
    ULONG StateCount;
    SYSTEM_PROCESSOR_PERFORMANCE_HITCOUNT States[1];
} SYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION, *PSYSTEM_PROCESSOR_PERFORMANCE_STATE_DISTRIBUTION;

// private
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION
{
    ULONG ProcessorCount;
    ULONG Offsets[1];
} SYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION, *PSYSTEM_PROCESSOR_PERFORMANCE_DISTRIBUTION;

// private
typedef enum _SYSTEM_VA_TYPE
{
    SystemVaTypeAll,
    SystemVaTypeNonPagedPool,
    SystemVaTypePagedPool,
    SystemVaTypeSystemCache,
    SystemVaTypeSystemPtes,
    SystemVaTypeSessionSpace,
    SystemVaTypeMax
} SYSTEM_VA_TYPE, *PSYSTEM_VA_TYPE;

// private
typedef struct _SYSTEM_VA_LIST_INFORMATION
{
    SIZE_T VirtualSize;
    SIZE_T VirtualPeak;
    SIZE_T VirtualLimit;
    SIZE_T AllocationFailures;
} SYSTEM_VA_LIST_INFORMATION, *PSYSTEM_VA_LIST_INFORMATION;

// private
typedef struct _SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS
{
    HANDLE KeyHandle;
    PUNICODE_STRING ValueNamePointer;
    PULONG RequiredLengthPointer;
    PUCHAR Buffer;
    ULONG BufferLength;
    ULONG Type;
    PUCHAR AppendBuffer;
    ULONG AppendBufferLength;
    BOOLEAN CreateIfDoesntExist;
    BOOLEAN TruncateExistingValue;
} SYSTEM_REGISTRY_APPEND_STRING_PARAMETERS, *PSYSTEM_REGISTRY_APPEND_STRING_PARAMETERS;

// msdn
typedef struct _SYSTEM_VHD_BOOT_INFORMATION
{
    BOOLEAN OsDiskIsVhd;
    ULONG OsVhdFilePathOffset;
    WCHAR OsVhdParentVolume[ANYSIZE_ARRAY];
} SYSTEM_VHD_BOOT_INFORMATION, *PSYSTEM_VHD_BOOT_INFORMATION;

// private
typedef struct _SYSTEM_LOW_PRIORITY_IO_INFORMATION
{
    ULONG LowPriReadOperations;
    ULONG LowPriWriteOperations;
    ULONG KernelBumpedToNormalOperations;
    ULONG LowPriPagingReadOperations;
    ULONG KernelPagingReadsBumpedToNormal;
    ULONG LowPriPagingWriteOperations;
    ULONG KernelPagingWritesBumpedToNormal;
    ULONG BoostedIrpCount;
    ULONG BoostedPagingIrpCount;
    ULONG BlanketBoostCount;
} SYSTEM_LOW_PRIORITY_IO_INFORMATION, *PSYSTEM_LOW_PRIORITY_IO_INFORMATION;

// symbols
typedef enum _TPM_BOOT_ENTROPY_RESULT_CODE
{
    TpmBootEntropyStructureUninitialized,
    TpmBootEntropyDisabledByPolicy,
    TpmBootEntropyNoTpmFound,
    TpmBootEntropyTpmError,
    TpmBootEntropySuccess
} TPM_BOOT_ENTROPY_RESULT_CODE;

// Contents of KeLoaderBlock->Extension->TpmBootEntropyResult (TPM_BOOT_ENTROPY_LDR_RESULT).
// EntropyData is truncated to 40 bytes.

// private
typedef struct _TPM_BOOT_ENTROPY_NT_RESULT
{
    ULONGLONG Policy;
    TPM_BOOT_ENTROPY_RESULT_CODE ResultCode;
    NTSTATUS ResultStatus;
    ULONGLONG Time;
    ULONG EntropyLength;
    UCHAR EntropyData[40];
} TPM_BOOT_ENTROPY_NT_RESULT, *PTPM_BOOT_ENTROPY_NT_RESULT;

// private
typedef struct _SYSTEM_VERIFIER_COUNTERS_INFORMATION
{
    SYSTEM_VERIFIER_INFORMATION Legacy;
    ULONG RaiseIrqls;
    ULONG AcquireSpinLocks;
    ULONG SynchronizeExecutions;
    ULONG AllocationsWithNoTag;
    ULONG AllocationsFailed;
    ULONG AllocationsFailedDeliberately;
    SIZE_T LockedBytes;
    SIZE_T PeakLockedBytes;
    SIZE_T MappedLockedBytes;
    SIZE_T PeakMappedLockedBytes;
    SIZE_T MappedIoSpaceBytes;
    SIZE_T PeakMappedIoSpaceBytes;
    SIZE_T PagesForMdlBytes;
    SIZE_T PeakPagesForMdlBytes;
    SIZE_T ContiguousMemoryBytes;
    SIZE_T PeakContiguousMemoryBytes;
} SYSTEM_VERIFIER_COUNTERS_INFORMATION, *PSYSTEM_VERIFIER_COUNTERS_INFORMATION;

// private
typedef struct _SYSTEM_ACPI_AUDIT_INFORMATION
{
    ULONG RsdpCount;
    ULONG SameRsdt : 1;
    ULONG SlicPresent : 1;
    ULONG SlicDifferent : 1;
} SYSTEM_ACPI_AUDIT_INFORMATION, *PSYSTEM_ACPI_AUDIT_INFORMATION;

// private
typedef struct _SYSTEM_BASIC_PERFORMANCE_INFORMATION
{
    SIZE_T AvailablePages;
    SIZE_T CommittedPages;
    SIZE_T CommitLimit;
    SIZE_T PeakCommitment;
} SYSTEM_BASIC_PERFORMANCE_INFORMATION, *PSYSTEM_BASIC_PERFORMANCE_INFORMATION;

// begin_msdn

typedef struct _QUERY_PERFORMANCE_COUNTER_FLAGS
{
    union
    {
        struct
        {
            ULONG KernelTransition : 1;
            ULONG Reserved : 31;
        };
        ULONG ul;
    };
} QUERY_PERFORMANCE_COUNTER_FLAGS;

typedef struct _SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION
{
    ULONG Version;
    QUERY_PERFORMANCE_COUNTER_FLAGS Flags;
    QUERY_PERFORMANCE_COUNTER_FLAGS ValidFlags;
} SYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION, *PSYSTEM_QUERY_PERFORMANCE_COUNTER_INFORMATION;

// end_msdn

// private
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX
{
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG InterruptCount;
    ULONG Spare0;
    LARGE_INTEGER AvailableTime;
    LARGE_INTEGER Spare1;
    LARGE_INTEGER Spare2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION_EX;

// private
typedef struct _SYSTEM_PAGEFILE_INFORMATION_EX
{
    SYSTEM_PAGEFILE_INFORMATION Info;
    ULONG MinimumSize;
    ULONG MaximumSize;
} SYSTEM_PAGEFILE_INFORMATION_EX, *PSYSTEM_PAGEFILE_INFORMATION_EX;

// private
typedef struct _PROCESS_DISK_COUNTERS
{
    ULONGLONG BytesRead;
    ULONGLONG BytesWritten;
    ULONGLONG ReadOperationCount;
    ULONGLONG WriteOperationCount;
    ULONGLONG FlushOperationCount;
} PROCESS_DISK_COUNTERS, *PPROCESS_DISK_COUNTERS;

// private
typedef struct _PROCESS_ENERGY_VALUES
{
    ULONGLONG Cycles[2][4];
    ULONGLONG DiskEnergy;
    ULONGLONG NetworkTailEnergy;
    ULONGLONG MBBTailEnergy;
    ULONGLONG NetworkTxRxBytes;
    ULONGLONG MBBTxRxBytes;
    union
    {
        struct
        {
            ULONG Foreground : 1;
        };
        ULONG WindowInformation;
    };
    ULONG PixelArea;
    LONGLONG PixelReportTimestamp;
    ULONGLONG PixelTime;
    LONGLONG ForegroundReportTimestamp;
    ULONGLONG ForegroundTime;
} PROCESS_ENERGY_VALUES, *PPROCESS_ENERGY_VALUES;

// private
typedef struct _SYSTEM_PROCESS_INFORMATION_EXTENSION
{
    PROCESS_DISK_COUNTERS DiskCounters;
    ULONGLONG ContextSwitches;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG HasStrongId : 1;
            ULONG Spare : 31;
        };
    };
    ULONG UserSidOffset;
    ULONG PackageFullNameOffset; // since THRESHOLD
    PROCESS_ENERGY_VALUES EnergyValues; // since THRESHOLD
    ULONG AppIdOffset; // since THRESHOLD
} SYSTEM_PROCESS_INFORMATION_EXTENSION, *PSYSTEM_PROCESS_INFORMATION_EXTENSION;

// private
typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX
{
    BOOLEAN DebuggerAllowed;
    BOOLEAN DebuggerEnabled;
    BOOLEAN DebuggerPresent;
} SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION_EX;

// private
typedef struct _SYSTEM_PROCESSOR_FEATURES_INFORMATION
{
    ULONGLONG ProcessorFeatureBits;
    ULONGLONG Reserved[3];
} SYSTEM_PROCESSOR_FEATURES_INFORMATION, *PSYSTEM_PROCESSOR_FEATURES_INFORMATION;

// private
typedef struct _SYSTEM_MANUFACTURING_INFORMATION
{
    ULONG Options;
    UNICODE_STRING ProfileName;
} SYSTEM_MANUFACTURING_INFORMATION, *PSYSTEM_MANUFACTURING_INFORMATION;

// private
typedef struct _SYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION
{
    BOOLEAN Enabled;
} SYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION, *PSYSTEM_ENERGY_ESTIMATION_CONFIG_INFORMATION;

// private
typedef struct _HV_DETAILS
{
    ULONG Data[4];
} HV_DETAILS, *PHV_DETAILS;

// private
typedef struct _SYSTEM_HYPERVISOR_DETAIL_INFORMATION
{
    HV_DETAILS HvVendorAndMaxFunction;
    HV_DETAILS HypervisorInterface;
    HV_DETAILS HypervisorVersion;
    HV_DETAILS HvFeatures;
    HV_DETAILS HwFeatures;
    HV_DETAILS EnlightenmentInfo;
    HV_DETAILS ImplementationLimits;
} SYSTEM_HYPERVISOR_DETAIL_INFORMATION, *PSYSTEM_HYPERVISOR_DETAIL_INFORMATION;

// private
typedef struct _SYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION
{
    ULONGLONG Cycles[2][4];
} SYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION, *PSYSTEM_PROCESSOR_CYCLE_STATS_INFORMATION;

// private
typedef struct _SYSTEM_TPM_INFORMATION
{
    ULONG Flags;
} SYSTEM_TPM_INFORMATION, *PSYSTEM_TPM_INFORMATION;

// private
typedef struct _SYSTEM_SINGLE_MODULE_INFORMATION
{
    PVOID TargetModuleAddress;
    RTL_PROCESS_MODULE_INFORMATION_EX ExInfo;
} SYSTEM_SINGLE_MODULE_INFORMATION, *PSYSTEM_SINGLE_MODULE_INFORMATION;

#if (PHNT_MODE != PHNT_MODE_KERNEL)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemInformation(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _Out_writes_bytes_opt_(SystemInformationLength) PVOID SystemInformation,
    _In_ ULONG SystemInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

#if (PHNT_VERSION >= PHNT_WIN7)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySystemInformationEx(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _In_reads_bytes_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_opt_(SystemInformationLength) PVOID SystemInformation,
    _In_ ULONG SystemInformationLength,
    _Out_opt_ PULONG ReturnLength
    );
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemInformation(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _In_reads_bytes_opt_(SystemInformationLength) PVOID SystemInformation,
    _In_ ULONG SystemInformationLength
    );

// SysDbg APIs

// private
typedef enum _SYSDBG_COMMAND
{
    SysDbgQueryModuleInformation,
    SysDbgQueryTraceInformation,
    SysDbgSetTracepoint,
    SysDbgSetSpecialCall,
    SysDbgClearSpecialCalls,
    SysDbgQuerySpecialCalls,
    SysDbgBreakPoint,
    SysDbgQueryVersion,
    SysDbgReadVirtual,
    SysDbgWriteVirtual,
    SysDbgReadPhysical,
    SysDbgWritePhysical,
    SysDbgReadControlSpace,
    SysDbgWriteControlSpace,
    SysDbgReadIoSpace,
    SysDbgWriteIoSpace,
    SysDbgReadMsr,
    SysDbgWriteMsr,
    SysDbgReadBusData,
    SysDbgWriteBusData,
    SysDbgCheckLowMemory,
    SysDbgEnableKernelDebugger,
    SysDbgDisableKernelDebugger,
    SysDbgGetAutoKdEnable,
    SysDbgSetAutoKdEnable,
    SysDbgGetPrintBufferSize,
    SysDbgSetPrintBufferSize,
    SysDbgGetKdUmExceptionEnable,
    SysDbgSetKdUmExceptionEnable,
    SysDbgGetTriageDump,
    SysDbgGetKdBlockEnable,
    SysDbgSetKdBlockEnable,
    SysDbgRegisterForUmBreakInfo,
    SysDbgGetUmBreakPid,
    SysDbgClearUmBreakPid,
    SysDbgGetUmAttachPid,
    SysDbgClearUmAttachPid
} SYSDBG_COMMAND, *PSYSDBG_COMMAND;

typedef struct _SYSDBG_VIRTUAL
{
    PVOID Address;
    PVOID Buffer;
    ULONG Request;
} SYSDBG_VIRTUAL, *PSYSDBG_VIRTUAL;

typedef struct _SYSDBG_PHYSICAL
{
    PHYSICAL_ADDRESS Address;
    PVOID Buffer;
    ULONG Request;
} SYSDBG_PHYSICAL, *PSYSDBG_PHYSICAL;

typedef struct _SYSDBG_CONTROL_SPACE
{
    ULONG64 Address;
    PVOID Buffer;
    ULONG Request;
    ULONG Processor;
} SYSDBG_CONTROL_SPACE, *PSYSDBG_CONTROL_SPACE;

enum _INTERFACE_TYPE;

typedef struct _SYSDBG_IO_SPACE
{
    ULONG64 Address;
    PVOID Buffer;
    ULONG Request;
    enum _INTERFACE_TYPE InterfaceType;
    ULONG BusNumber;
    ULONG AddressSpace;
} SYSDBG_IO_SPACE, *PSYSDBG_IO_SPACE;

typedef struct _SYSDBG_MSR
{
    ULONG Msr;
    ULONG64 Data;
} SYSDBG_MSR, *PSYSDBG_MSR;

enum _BUS_DATA_TYPE;

typedef struct _SYSDBG_BUS_DATA
{
    ULONG Address;
    PVOID Buffer;
    ULONG Request;
    enum _BUS_DATA_TYPE BusDataType;
    ULONG BusNumber;
    ULONG SlotNumber;
} SYSDBG_BUS_DATA, *PSYSDBG_BUS_DATA;

// private
typedef struct _SYSDBG_TRIAGE_DUMP
{
    ULONG Flags;
    ULONG BugCheckCode;
    ULONG_PTR BugCheckParam1;
    ULONG_PTR BugCheckParam2;
    ULONG_PTR BugCheckParam3;
    ULONG_PTR BugCheckParam4;
    ULONG ProcessHandles;
    ULONG ThreadHandles;
    PHANDLE Handles;
} SYSDBG_TRIAGE_DUMP, *PSYSDBG_TRIAGE_DUMP;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSystemDebugControl(
    _In_ SYSDBG_COMMAND Command,
    _Inout_updates_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength,
    _Out_opt_ PULONG ReturnLength
    );

// Hard errors

typedef enum _HARDERROR_RESPONSE_OPTION
{
    OptionAbortRetryIgnore,
    OptionOk,
    OptionOkCancel,
    OptionRetryCancel,
    OptionYesNo,
    OptionYesNoCancel,
    OptionShutdownSystem,
    OptionOkNoWait,
    OptionCancelTryContinue
} HARDERROR_RESPONSE_OPTION;

typedef enum _HARDERROR_RESPONSE
{
    ResponseReturnToCaller,
    ResponseNotHandled,
    ResponseAbort,
    ResponseCancel,
    ResponseIgnore,
    ResponseNo,
    ResponseOk,
    ResponseRetry,
    ResponseYes,
    ResponseTryAgain,
    ResponseContinue
} HARDERROR_RESPONSE;

// HARDERROR_MSG not included

NTSYSCALLAPI
NTSTATUS
NTAPI
NtRaiseHardError(
    _In_ NTSTATUS ErrorStatus,
    _In_ ULONG NumberOfParameters,
    _In_ ULONG UnicodeStringParameterMask,
    _In_reads_(NumberOfParameters) PULONG_PTR Parameters,
    _In_ ULONG ValidResponseOptions,
    _Out_ PULONG Response
    );

// Kernel-user shared data

typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE
{
    StandardDesign,
    NEC98x86,
    EndAlternatives
} ALTERNATIVE_ARCHITECTURE_TYPE;

#define PROCESSOR_FEATURE_MAX 64

#define MAX_WOW64_SHARED_ENTRIES 16

#define NX_SUPPORT_POLICY_ALWAYSOFF 0
#define NX_SUPPORT_POLICY_ALWAYSON 1
#define NX_SUPPORT_POLICY_OPTIN 2
#define NX_SUPPORT_POLICY_OPTOUT 3

#include <pshpack4.h>
typedef struct _KUSER_SHARED_DATA
{
    ULONG TickCountLowDeprecated;
    ULONG TickCountMultiplier;

    volatile KSYSTEM_TIME InterruptTime;
    volatile KSYSTEM_TIME SystemTime;
    volatile KSYSTEM_TIME TimeZoneBias;

    USHORT ImageNumberLow;
    USHORT ImageNumberHigh;

    WCHAR NtSystemRoot[260];

    ULONG MaxStackTraceDepth;

    ULONG CryptoExponent;

    ULONG TimeZoneId;
    ULONG LargePageMinimum;
    ULONG AitSamplingValue;
    ULONG AppCompatFlag;
    ULONGLONG RNGSeedVersion;
    ULONG GlobalValidationRunlevel;
    LONG TimeZoneBiasStamp;
    ULONG Reserved2;

    ULONG NtProductType;
    BOOLEAN ProductTypeIsValid;
    UCHAR Reserved0[1];
    USHORT NativeProcessorArchitecture;

    ULONG NtMajorVersion;
    ULONG NtMinorVersion;

    BOOLEAN ProcessorFeatures[PROCESSOR_FEATURE_MAX];

    ULONG Reserved1;
    ULONG Reserved3;

    volatile ULONG TimeSlip;

    ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;
    ULONG AltArchitecturePad[1];

    LARGE_INTEGER SystemExpirationDate;

    ULONG SuiteMask;

    BOOLEAN KdDebuggerEnabled;
    union
    {
        UCHAR MitigationPolicies;
        struct
        {
            UCHAR NXSupportPolicy : 2;
            UCHAR SEHValidationPolicy : 2;
            UCHAR CurDirDevicesSkippedForDlls : 2;
            UCHAR Reserved : 2;
        };
    };
    UCHAR Reserved6[2];

    volatile ULONG ActiveConsoleId;

    volatile ULONG DismountCount;

    ULONG ComPlusPackage;

    ULONG LastSystemRITEventTickCount;

    ULONG NumberOfPhysicalPages;

    BOOLEAN SafeBootMode;
    UCHAR Reserved12[3];

    union
    {
        ULONG SharedDataFlags;
        struct
        {
            ULONG DbgErrorPortPresent : 1;
            ULONG DbgElevationEnabled : 1;
            ULONG DbgVirtEnabled : 1;
            ULONG DbgInstallerDetectEnabled : 1;
            ULONG DbgLkgEnabled : 1;
            ULONG DbgDynProcessorEnabled : 1;
            ULONG DbgConsoleBrokerEnabled : 1;
            ULONG DbgSecureBootEnabled : 1;
            ULONG SpareBits : 24;
        };
    };
    ULONG DataFlagsPad[1];

    ULONGLONG TestRetInstruction;
    ULONGLONG QpcFrequency;
    ULONGLONG SystemCallPad[3];

    union
    {
        volatile KSYSTEM_TIME TickCount;
        volatile ULONG64 TickCountQuad;
        ULONG ReservedTickCountOverlay[3];
    };
    ULONG TickCountPad[1];

    ULONG Cookie;
    ULONG CookiePad[1];

    LONGLONG ConsoleSessionForegroundProcessId;
    ULONGLONG TimeUpdateLock;
    ULONGLONG BaselineSystemTimeQpc;
    ULONGLONG BaselineInterruptTimeQpc;
    ULONGLONG QpcSystemTimeIncrement;
    ULONGLONG QpcInterruptTimeIncrement;
    ULONG QpcSystemTimeIncrement32;
    ULONG QpcInterruptTimeIncrement32;
    UCHAR QpcSystemTimeIncrementShift;
    UCHAR QpcInterruptTimeIncrementShift;
    UCHAR Reserved8[14];

    USHORT UserModeGlobalLogger[16];
    ULONG ImageFileExecutionOptions;

    ULONG LangGenerationCount;
    ULONGLONG Reserved4;
    volatile ULONG64 InterruptTimeBias;
    volatile ULONG64 QpcBias;

    volatile ULONG ActiveProcessorCount;
    volatile UCHAR ActiveGroupCount;
    UCHAR Reserved9;
    union
    {
        USHORT QpcData;
        struct
        {
            UCHAR QpcBypassEnabled : 1;
            UCHAR QpcShift : 1;
        };
    };

    LARGE_INTEGER TimeZoneBiasEffectiveStart;
    LARGE_INTEGER TimeZoneBiasEffectiveEnd;
    XSTATE_CONFIGURATION XState;
} KUSER_SHARED_DATA, *PKUSER_SHARED_DATA;
#include <poppack.h>

C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TickCountMultiplier) == 0x4);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, InterruptTime) == 0x8);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SystemTime) == 0x14);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TimeZoneBias) == 0x20);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ImageNumberLow) == 0x2c);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ImageNumberHigh) == 0x2e);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NtSystemRoot) == 0x30);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, MaxStackTraceDepth) == 0x238);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, CryptoExponent) == 0x23c);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TimeZoneId) == 0x240);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, LargePageMinimum) == 0x244);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NtProductType) == 0x264);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ProductTypeIsValid) == 0x268);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NtMajorVersion) == 0x26c);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NtMinorVersion) == 0x270);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ProcessorFeatures) == 0x274);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, Reserved1) == 0x2b4);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, Reserved3) == 0x2b8);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TimeSlip) == 0x2bc);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, AlternativeArchitecture) == 0x2c0);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SystemExpirationDate) == 0x2c8);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SuiteMask) == 0x2d0);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, KdDebuggerEnabled) == 0x2d4);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ActiveConsoleId) == 0x2d8);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, DismountCount) == 0x2dc);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, ComPlusPackage) == 0x2e0);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, LastSystemRITEventTickCount) == 0x2e4);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, NumberOfPhysicalPages) == 0x2e8);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SafeBootMode) == 0x2ec);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TestRetInstruction) == 0x2f8);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, SystemCallPad) == 0x308);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TickCount) == 0x320);
C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, TickCountQuad) == 0x320);
// C_ASSERT(FIELD_OFFSET(KUSER_SHARED_DATA, XState) == 0x3d8); // Visual Studio has a problem with this

#define USER_SHARED_DATA ((KUSER_SHARED_DATA * const)0x7ffe0000)

#if (PHNT_VERSION >= PHNT_WS03)

FORCEINLINE ULONGLONG NtGetTickCount64()
{
    ULARGE_INTEGER tickCount;

#ifdef _WIN64

    tickCount.QuadPart = USER_SHARED_DATA->TickCountQuad;

#else

    while (TRUE)
    {
        tickCount.HighPart = (ULONG)USER_SHARED_DATA->TickCount.High1Time;
        tickCount.LowPart = USER_SHARED_DATA->TickCount.LowPart;

        if (tickCount.HighPart == (ULONG)USER_SHARED_DATA->TickCount.High2Time)
            break;

        YieldProcessor();
    }

#endif

    return (UInt32x32To64(tickCount.LowPart, USER_SHARED_DATA->TickCountMultiplier) >> 24) +
        (UInt32x32To64(tickCount.HighPart, USER_SHARED_DATA->TickCountMultiplier) << 8);
}

FORCEINLINE ULONG NtGetTickCount()
{
#ifdef _WIN64

    return (ULONG)((USER_SHARED_DATA->TickCountQuad * USER_SHARED_DATA->TickCountMultiplier) >> 24);

#else

    ULARGE_INTEGER tickCount;

    while (TRUE)
    {
        tickCount.HighPart = (ULONG)USER_SHARED_DATA->TickCount.High1Time;
        tickCount.LowPart = USER_SHARED_DATA->TickCount.LowPart;

        if (tickCount.HighPart == (ULONG)USER_SHARED_DATA->TickCount.High2Time)
            break;

        YieldProcessor();
    }

    return (ULONG)((UInt32x32To64(tickCount.LowPart, USER_SHARED_DATA->TickCountMultiplier) >> 24) +
        UInt32x32To64((tickCount.HighPart << 8) & 0xffffffff, USER_SHARED_DATA->TickCountMultiplier));

#endif
}

#endif

// Locale

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDefaultLocale(
    _In_ BOOLEAN UserProfile,
    _Out_ PLCID DefaultLocaleId
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetDefaultLocale(
    _In_ BOOLEAN UserProfile,
    _In_ LCID DefaultLocaleId
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInstallUILanguage(
    _Out_ LANGID *InstallUILanguageId
    );

#if (PHNT_VERSION >= PHNT_VISTA)
// private
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushInstallUILanguage(
    _In_ LANGID InstallUILanguage,
    _In_ ULONG SetComittedFlag
    );
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDefaultUILanguage(
    _Out_ LANGID *DefaultUILanguageId
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetDefaultUILanguage(
    _In_ LANGID DefaultUILanguageId
    );

#if (PHNT_VERSION >= PHNT_VISTA)
// private
NTSYSCALLAPI
NTSTATUS
NTAPI
NtIsUILanguageComitted(
    VOID
    );
#endif

// NLS

// begin_private

#if (PHNT_VERSION >= PHNT_VISTA)

#if (PHNT_VERSION >= PHNT_WIN7)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtInitializeNlsFiles(
    _Out_ PVOID *BaseAddress,
    _Out_ PLCID DefaultLocaleId,
    _Out_ PLARGE_INTEGER DefaultCasingTableSize
    );
#else
NTSYSCALLAPI
NTSTATUS
NTAPI
NtInitializeNlsFiles(
    _Out_ PVOID *BaseAddress,
    _Out_ PLCID DefaultLocaleId,
    _Out_ PLARGE_INTEGER DefaultCasingTableSize,
    _Out_opt_ PULONG CurrentNLSVersion
    );
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetNlsSectionPtr(
    _In_ ULONG SectionType,
    _In_ ULONG SectionData,
    _In_ PVOID ContextData,
    _Out_ PVOID *SectionPointer,
    _Out_ PULONG SectionSize
    );

#if (PHNT_VERSION < PHNT_WIN7)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAcquireCMFViewOwnership(
    _Out_ PULONGLONG TimeStamp,
    _Out_ PBOOLEAN tokenTaken,
    _In_ BOOLEAN replaceExisting
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseCMFViewOwnership(
    VOID
    );

#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtMapCMFModule(
    _In_ ULONG What,
    _In_ ULONG Index,
    _Out_opt_ PULONG CacheIndexOut,
    _Out_opt_ PULONG CacheFlagsOut,
    _Out_opt_ PULONG ViewSizeOut,
    _Out_opt_ PVOID *BaseAddress
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetMUIRegistryInfo(
    _In_ ULONG Flags,
    _Inout_ PULONG DataSize,
    _Out_ PVOID Data
    );

#endif

// end_private

// Global atoms

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAddAtom(
    _In_reads_bytes_opt_(Length) PWSTR AtomName,
    _In_ ULONG Length,
    _Out_opt_ PRTL_ATOM Atom
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtFindAtom(
    _In_reads_bytes_opt_(Length) PWSTR AtomName,
    _In_ ULONG Length,
    _Out_opt_ PRTL_ATOM Atom
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteAtom(
    _In_ RTL_ATOM Atom
    );

typedef enum _ATOM_INFORMATION_CLASS
{
    AtomBasicInformation,
    AtomTableInformation
} ATOM_INFORMATION_CLASS;

typedef struct _ATOM_BASIC_INFORMATION
{
    USHORT UsageCount;
    USHORT Flags;
    USHORT NameLength;
    WCHAR Name[1];
} ATOM_BASIC_INFORMATION, *PATOM_BASIC_INFORMATION;

typedef struct _ATOM_TABLE_INFORMATION
{
    ULONG NumberOfAtoms;
    RTL_ATOM Atoms[1];
} ATOM_TABLE_INFORMATION, *PATOM_TABLE_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationAtom(
    _In_ RTL_ATOM Atom,
    _In_ ATOM_INFORMATION_CLASS AtomInformationClass,
    _Out_writes_bytes_(AtomInformationLength) PVOID AtomInformation,
    _In_ ULONG AtomInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

// Global flags

#define FLG_STOP_ON_EXCEPTION 0x00000001 // uk
#define FLG_SHOW_LDR_SNAPS 0x00000002 // uk
#define FLG_DEBUG_INITIAL_COMMAND 0x00000004 // k
#define FLG_STOP_ON_HUNG_GUI 0x00000008 // k

#define FLG_HEAP_ENABLE_TAIL_CHECK 0x00000010 // u
#define FLG_HEAP_ENABLE_FREE_CHECK 0x00000020 // u
#define FLG_HEAP_VALIDATE_PARAMETERS 0x00000040 // u
#define FLG_HEAP_VALIDATE_ALL 0x00000080 // u

#define FLG_APPLICATION_VERIFIER 0x00000100 // u
#define FLG_POOL_ENABLE_TAGGING 0x00000400 // k
#define FLG_HEAP_ENABLE_TAGGING 0x00000800 // u

#define FLG_USER_STACK_TRACE_DB 0x00001000 // u,32
#define FLG_KERNEL_STACK_TRACE_DB 0x00002000 // k,32
#define FLG_MAINTAIN_OBJECT_TYPELIST 0x00004000 // k
#define FLG_HEAP_ENABLE_TAG_BY_DLL 0x00008000 // u

#define FLG_DISABLE_STACK_EXTENSION 0x00010000 // u
#define FLG_ENABLE_CSRDEBUG 0x00020000 // k
#define FLG_ENABLE_KDEBUG_SYMBOL_LOAD 0x00040000 // k
#define FLG_DISABLE_PAGE_KERNEL_STACKS 0x00080000 // k

#define FLG_ENABLE_SYSTEM_CRIT_BREAKS 0x00100000 // u
#define FLG_HEAP_DISABLE_COALESCING 0x00200000 // u
#define FLG_ENABLE_CLOSE_EXCEPTIONS 0x00400000 // k
#define FLG_ENABLE_EXCEPTION_LOGGING 0x00800000 // k

#define FLG_ENABLE_HANDLE_TYPE_TAGGING 0x01000000 // k
#define FLG_HEAP_PAGE_ALLOCS 0x02000000 // u
#define FLG_DEBUG_INITIAL_COMMAND_EX 0x04000000 // k
#define FLG_DISABLE_DBGPRINT 0x08000000 // k

#define FLG_CRITSEC_EVENT_CREATION 0x10000000 // u
#define FLG_LDR_TOP_DOWN 0x20000000 // u,64
#define FLG_ENABLE_HANDLE_EXCEPTIONS 0x40000000 // k
#define FLG_DISABLE_PROTDLLS 0x80000000 // u

#define FLG_VALID_BITS 0xfffffdff

#define FLG_USERMODE_VALID_BITS (FLG_STOP_ON_EXCEPTION | \
    FLG_SHOW_LDR_SNAPS | \
    FLG_HEAP_ENABLE_TAIL_CHECK | \
    FLG_HEAP_ENABLE_FREE_CHECK | \
    FLG_HEAP_VALIDATE_PARAMETERS | \
    FLG_HEAP_VALIDATE_ALL | \
    FLG_APPLICATION_VERIFIER | \
    FLG_HEAP_ENABLE_TAGGING | \
    FLG_USER_STACK_TRACE_DB | \
    FLG_HEAP_ENABLE_TAG_BY_DLL | \
    FLG_DISABLE_STACK_EXTENSION | \
    FLG_ENABLE_SYSTEM_CRIT_BREAKS | \
    FLG_HEAP_DISABLE_COALESCING | \
    FLG_DISABLE_PROTDLLS | \
    FLG_HEAP_PAGE_ALLOCS | \
    FLG_CRITSEC_EVENT_CREATION | \
    FLG_LDR_TOP_DOWN)

#define FLG_BOOTONLY_VALID_BITS (FLG_KERNEL_STACK_TRACE_DB | \
    FLG_MAINTAIN_OBJECT_TYPELIST | \
    FLG_ENABLE_CSRDEBUG | \
    FLG_DEBUG_INITIAL_COMMAND | \
    FLG_DEBUG_INITIAL_COMMAND_EX | \
    FLG_DISABLE_PAGE_KERNEL_STACKS)

#define FLG_KERNELMODE_VALID_BITS (FLG_STOP_ON_EXCEPTION | \
    FLG_SHOW_LDR_SNAPS | \
    FLG_STOP_ON_HUNG_GUI | \
    FLG_POOL_ENABLE_TAGGING | \
    FLG_ENABLE_KDEBUG_SYMBOL_LOAD | \
    FLG_ENABLE_CLOSE_EXCEPTIONS | \
    FLG_ENABLE_EXCEPTION_LOGGING | \
    FLG_ENABLE_HANDLE_TYPE_TAGGING | \
    FLG_DISABLE_DBGPRINT | \
    FLG_ENABLE_HANDLE_EXCEPTIONS)

// Licensing

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryLicenseValue(
    _In_ PUNICODE_STRING ValueName,
    _Out_opt_ PULONG Type,
    _Out_writes_bytes_to_opt_(DataSize, *ResultDataSize) PVOID Data,
    _In_ ULONG DataSize,
    _Out_ PULONG ResultDataSize
    );

// Misc.

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetDefaultHardErrorPort(
    _In_ HANDLE DefaultHardErrorPort
    );

typedef enum _SHUTDOWN_ACTION
{
    ShutdownNoReboot,
    ShutdownReboot,
    ShutdownPowerOff
} SHUTDOWN_ACTION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtShutdownSystem(
    _In_ SHUTDOWN_ACTION Action
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDisplayString(
    _In_ PUNICODE_STRING String
    );

#if (PHNT_VERSION >= PHNT_WIN7)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDrawText(
    _In_ PUNICODE_STRING String
    );
#endif

#endif // (PHNT_MODE != PHNT_MODE_KERNEL)

#endif
