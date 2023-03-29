#pragma once

#include "WIE_ntdef.h"

#ifndef RSA32API
#define RSA32API __stdcall
#endif

#define LOGONID_CURRENT     ((ULONG)-1)
#define SERVERNAME_CURRENT  ((HANDLE)NULL)

typedef struct _LEAP_SECOND_DATA {
    UCHAR Enabled;
    UCHAR Padding[3];
    ULONG Count;
    LARGE_INTEGER Data[ANYSIZE_ARRAY];
} LEAP_SECOND_DATA, *PLEAP_SECOND_DATA;

typedef struct _ACTIVATION_CONTEXT_DATA {
    ULONG Magic;
    ULONG HeaderSize;
    ULONG FormatVersion;
    ULONG TotalSize;
    ULONG DefaultTocOffset;
    ULONG ExtendedTocOffset;
    ULONG AssemblyRosterOffset;
    ULONG Flags;
} ACTIVATION_CONTEXT_DATA, *PACTIVATION_CONTEXT_DATA;

#pragma region System Information

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemProcessorInformation = 1,
    SystemPerformanceInformation = 2,
    SystemTimeOfDayInformation = 3,
    SystemPathInformation = 4,
    SystemProcessInformation = 5,
    SystemCallCountInformation = 6,
    SystemDeviceInformation = 7,
    SystemProcessorPerformanceInformation = 8,
    SystemFlagsInformation = 9,
    SystemCallTimeInformation = 10,
    SystemModuleInformation = 11,
    SystemLocksInformation = 12,
    SystemStackTraceInformation = 13,
    SystemPagedPoolInformation = 14,
    SystemNonPagedPoolInformation = 15,
    SystemHandleInformation = 16,
    SystemObjectInformation = 17,
    SystemPageFileInformation = 18,
    SystemVdmInstemulInformation = 19,
    SystemVdmBopInformation = 20,
    SystemFileCacheInformation = 21,
    SystemPoolTagInformation = 22,
    SystemInterruptInformation = 23,
    SystemDpcBehaviorInformation = 24,
    SystemFullMemoryInformation = 25,
    SystemLoadGdiDriverInformation = 26,
    SystemUnloadGdiDriverInformation = 27,
    SystemTimeAdjustmentInformation = 28,
    SystemSummaryMemoryInformation = 29,
    SystemMirrorMemoryInformation = 30,
    SystemPerformanceTraceInformation = 31,
    SystemObsolete0 = 32,
    SystemExceptionInformation = 33,
    SystemCrashDumpStateInformation = 34,
    SystemKernelDebuggerInformation = 35,
    SystemContextSwitchInformation = 36,
    SystemRegistryQuotaInformation = 37,
    SystemExtendServiceTableInformation = 38,
    SystemPrioritySeperation = 39,
    SystemVerifierAddDriverInformation = 40,
    SystemVerifierRemoveDriverInformation = 41,
    SystemProcessorIdleInformation = 42,
    SystemLegacyDriverInformation = 43,
    SystemCurrentTimeZoneInformation = 44,
    SystemLookasideInformation = 45,
    SystemTimeSlipNotification = 46,
    SystemSessionCreate = 47,
    SystemSessionDetach = 48,
    SystemSessionInformation = 49,
    SystemRangeStartInformation = 50,
    SystemVerifierInformation = 51,
    SystemVerifierThunkExtend = 52,
    SystemSessionProcessInformation = 53,
    SystemLoadGdiDriverInSystemSpace = 54,
    SystemNumaProcessorMap = 55,
    SystemPrefetcherInformation = 56,
    SystemExtendedProcessInformation = 57,
    SystemRecommendedSharedDataAlignment = 58,
    SystemComPlusPackage = 59,
    SystemNumaAvailableMemory = 60,
    SystemProcessorPowerInformation = 61,
    SystemEmulationBasicInformation = 62,
    SystemEmulationProcessorInformation = 63,
    SystemExtendedHandleInformation = 64,
    SystemLostDelayedWriteInformation = 65,
    SystemBigPoolInformation = 66,
    SystemSessionPoolTagInformation = 67,
    SystemSessionMappedViewInformation = 68,
    SystemHotpatchInformation = 69,
    SystemObjectSecurityMode = 70,
    SystemWatchdogTimerHandler = 71,
    SystemWatchdogTimerInformation = 72,
    SystemLogicalProcessorInformation = 73,
    SystemWow64SharedInformationObsolete = 74,
    SystemRegisterFirmwareTableInformationHandler = 75,
    SystemFirmwareTableInformation = 76,
    SystemModuleInformationEx = 77,
    SystemVerifierTriageInformation = 78,
    SystemSuperfetchInformation = 79,
    SystemMemoryListInformation = 80,
    SystemFileCacheInformationEx = 81,
    SystemThreadPriorityClientIdInformation = 82,
    SystemProcessorIdleCycleTimeInformation = 83,
    SystemVerifierCancellationInformation = 84,
    SystemProcessorPowerInformationEx = 85,
    SystemRefTraceInformation = 86,
    SystemSpecialPoolInformation = 87,
    SystemProcessIdInformation = 88,
    SystemErrorPortInformation = 89,
    SystemBootEnvironmentInformation = 90,
    SystemHypervisorInformation = 91,
    SystemVerifierInformationEx = 92,
    SystemTimeZoneInformation = 93,
    SystemImageFileExecutionOptionsInformation = 94,
    SystemCoverageInformation = 95,
    SystemPrefetchPatchInformation = 96,
    SystemVerifierFaultsInformation = 97,
    SystemSystemPartitionInformation = 98,
    SystemSystemDiskInformation = 99,
    SystemProcessorPerformanceDistribution = 100,
    SystemNumaProximityNodeInformation = 101,
    SystemDynamicTimeZoneInformation = 102,
    SystemCodeIntegrityInformation = 103,
    SystemProcessorMicrocodeUpdateInformation = 104,
    SystemProcessorBrandString = 105,
    SystemVirtualAddressInformation = 106,
    SystemLogicalProcessorAndGroupInformation = 107,
    SystemProcessorCycleTimeInformation = 108,
    SystemStoreInformation = 109,
    SystemRegistryAppendString = 110,
    SystemAitSamplingValue = 111,
    SystemVhdBootInformation = 112,
    SystemCpuQuotaInformation = 113,
    SystemNativeBasicInformation = 114,
    SystemErrorPortTimeouts = 115,
    SystemLowPriorityIoInformation = 116,
    SystemBootEntropyInformation = 117,
    SystemVerifierCountersInformation = 118,
    SystemPagedPoolInformationEx = 119,
    SystemSystemPtesInformationEx = 120,
    SystemNodeDistanceInformation = 121,
    SystemAcpiAuditInformation = 122,
    SystemBasicPerformanceInformation = 123,
    SystemQueryPerformanceCounterInformation = 124,
    SystemSessionBigPoolInformation = 125,
    SystemBootGraphicsInformation = 126,
    SystemScrubPhysicalMemoryInformation = 127,
    SystemBadPageInformation = 128,
    SystemProcessorProfileControlArea = 129,
    SystemCombinePhysicalMemoryInformation = 130,
    SystemEntropyInterruptTimingInformation = 131,
    SystemConsoleInformation = 132,
    SystemPlatformBinaryInformation = 133,
    SystemPolicyInformation = 134,
    SystemHypervisorProcessorCountInformation = 135,
    SystemDeviceDataInformation = 136,
    SystemDeviceDataEnumerationInformation = 137,
    SystemMemoryTopologyInformation = 138,
    SystemMemoryChannelInformation = 139,
    SystemBootLogoInformation = 140,
    SystemProcessorPerformanceInformationEx = 141,
    SystemCriticalProcessErrorLogInformation = 142,
    SystemSecureBootPolicyInformation = 143,
    SystemPageFileInformationEx = 144,
    SystemSecureBootInformation = 145,
    SystemEntropyInterruptTimingRawInformation = 146,
    SystemPortableWorkspaceEfiLauncherInformation = 147,
    SystemFullProcessInformation = 148,
    SystemKernelDebuggerInformationEx = 149,
    SystemBootMetadataInformation = 150,
    SystemSoftRebootInformation = 151,
    SystemElamCertificateInformation = 152,
    SystemOfflineDumpConfigInformation = 153,
    SystemProcessorFeaturesInformation = 154,
    SystemRegistryReconciliationInformation = 155,
    SystemEdidInformation = 156,
    SystemManufacturingInformation = 157,
    SystemEnergyEstimationConfigInformation = 158,
    SystemHypervisorDetailInformation = 159,
    SystemProcessorCycleStatsInformation = 160,
    SystemVmGenerationCountInformation = 161,
    SystemTrustedPlatformModuleInformation = 162,
    SystemKernelDebuggerFlags = 163,
    SystemCodeIntegrityPolicyInformation = 164,
    SystemIsolatedUserModeInformation = 165,
    SystemHardwareSecurityTestInterfaceResultsInformation = 166,
    SystemSingleModuleInformation = 167,
    SystemAllowedCpuSetsInformation = 168,
    SystemVsmProtectionInformation = 169,
    SystemInterruptCpuSetsInformation = 170,
    SystemSecureBootPolicyFullInformation = 171,
    SystemCodeIntegrityPolicyFullInformation = 172,
    SystemAffinitizedInterruptProcessorInformation = 173,
    SystemRootSiloInformation = 174,
    SystemCpuSetInformation = 175,
    SystemCpuSetTagInformation = 176,
    SystemWin32WerStartCallout = 177,
    SystemSecureKernelProfileInformation = 178,
    SystemCodeIntegrityPlatformManifestInformation = 179,
    SystemInterruptSteeringInformation = 180,
    SystemSupportedProcessorArchitectures = 181,
    SystemMemoryUsageInformation = 182,
    SystemCodeIntegrityCertificateInformation = 183,
    SystemPhysicalMemoryInformation = 184,
    SystemControlFlowTransition = 185,
    SystemKernelDebuggingAllowed = 186,
    SystemActivityModerationExeState = 187,
    SystemActivityModerationUserSettings = 188,
    SystemCodeIntegrityPoliciesFullInformation = 189,
    SystemCodeIntegrityUnlockInformation = 190,
    SystemIntegrityQuotaInformation = 191,
    SystemFlushInformation = 192,
    SystemProcessorIdleMaskInformation = 193,
    SystemSecureDumpEncryptionInformation = 194,
    SystemWriteConstraintInformation = 195,
    SystemKernelVaShadowInformation = 196,
    SystemHypervisorSharedPageInformation = 197,
    SystemFirmwareBootPerformanceInformation = 198,
    SystemCodeIntegrityVerificationInformation = 199,
    SystemFirmwarePartitionInformation = 200,
    SystemSpeculationControlInformation = 201,
    SystemDmaGuardPolicyInformation = 202,
    SystemEnclaveLaunchControlInformation = 203,
    SystemWorkloadAllowedCpuSetsInformation = 204,
    SystemCodeIntegrityUnlockModeInformation = 205,
    SystemLeapSecondInformation = 206,
    SystemFlags2Information = 207,
    SystemSecurityModelInformation = 208,
    SystemCodeIntegritySyntheticCacheInformation = 209,
    SystemFeatureConfigurationInformation = 210,
    SystemFeatureConfigurationSectionInformation = 211,
    SystemFeatureUsageSubscriptionInformation = 212,
    SystemSecureSpeculationControlInformation = 213,
    SystemSpacesBootInformation = 214,
    SystemFwRamdiskInformation = 215,
    SystemWheaIpmiHardwareInformation = 216,
    SystemDifSetRuleClassInformation = 217,
    SystemDifClearRuleClassInformation = 218,
    SystemDifApplyPluginVerificationOnDriver = 219,
    SystemDifRemovePluginVerificationOnDriver = 220,
    SystemShadowStackInformation = 221,
    SystemBuildVersionInformation = 222,
    SystemPoolLimitInformation = 223,
    SystemCodeIntegrityAddDynamicStore = 224,
    SystemCodeIntegrityClearDynamicStores = 225,
    SystemPoolZeroingInformation = 227,
    MaxSystemInfoClass = 228
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_MODULE_ENTRY {
    ULONG_PTR  Unused;
    ULONG_PTR  Always0;
    PVOID  ModuleBaseAddress;
    ULONG  ModuleSize;
    ULONG  Unknown;
    ULONG  ModuleEntryIndex;
    USHORT ModuleNameLength;
    USHORT ModuleNameOffset;
    CHAR   ModuleName[256];
} SYSTEM_MODULE_ENTRY, *PSYSTEM_MODULE_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION {
    ULONG               Count;
    SYSTEM_MODULE_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize;
    ULONG HardFaultCount;
    ULONG NumberOfThreadsHighWatermark;
    ULONGLONG CycleTime;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR PageDirectoryBase;
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
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef struct _SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    KTHREAD_STATE ThreadState;
    KWAIT_REASON WaitReason;
    ULONG PadPadAlignment;
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

#pragma endregion System Information

#pragma region Ldr*

typedef struct _LDR_SERVICE_TAG_RECORD LDR_SERVICE_TAG_RECORD, *PLDR_SERVICE_TAG_RECORD;

struct _LDR_SERVICE_TAG_RECORD {
    PLDR_SERVICE_TAG_RECORD Next;
    UINT ServiceTag;
};

typedef enum _LDR_DDAG_STATE {
    LdrModulesMerged = -5,
    LdrModulesInitError = -4,
    LdrModulesSnapError = -3,
    LdrModulesUnloaded = -2,
    LdrModulesUnloading = -1,
    LdrModulesPlaceHolder = 0,
    LdrModulesMapping = 1,
    LdrModulesMapped = 2,
    LdrModulesWaitingForDependencies = 3,
    LdrModulesSnapping = 4,
    LdrModulesSnapped = 5,
    LdrModulesCondensed = 6,
    LdrModulesReadyToInit = 7,
    LdrModulesInitializing = 8,
    LdrModulesReadyToRun = 9,
} LDR_DDAG_STATE, *PLDR_DDAG_STATE;

typedef struct _LDRP_CSLIST {
    SINGLE_LIST_ENTRY Tail;
} LDRP_CSLIST, *PLDRP_CSLIST;

typedef struct _LDR_DDAG_NODE {
    LIST_ENTRY Modules;
    PLDR_SERVICE_TAG_RECORD ServiceTagList;
    UINT LoadCount;
    UINT LoadWhileUnloadingCount;
    PVOID LowestLink;
    LDRP_CSLIST Dependencies;
    LDRP_CSLIST IncomingDependencies;
    LDR_DDAG_STATE State;
    SINGLE_LIST_ENTRY CondenseLink;
    UINT PreorderNumber;
} LDR_DDAG_NODE, *PLDR_DDAG_NODE;

typedef enum _LDR_HOT_PATCH_STATE {
    LdrHotPatchBaseImage = 0,
    LdrHotPatchNotApplied = 1,
    LdrHotPatchAppliedReverse = 2,
    LdrHotPatchAppliedForward = 3,
    LdrHotPatchFailedToPatch = 4,
    LdrHotPatchStateMax = 5
}LDR_HOT_PATCH_STATE, *PLDR_HOT_PATCH_STATE;

typedef enum _LDR_DLL_LOAD_REASON {
    LoadReasonStaticDependency = 0,
    LoadReasonStaticForwarderDependency = 1,
    LoadReasonDynamicForwarderDependency = 2,
    LoadReasonDelayloadDependency = 3,
    LoadReasonDynamicLoad = 4,
    LoadReasonAsImageLoad = 5,
    LoadReasonAsDataLoad = 6,
    LoadReasonEnclavePrimary = 7,
    LoadReasonEnclaveDependency = 8,
    LoadReasonPatchImage = 9,
    LoadReasonUnknown = -1
} LDR_DLL_LOAD_REASON, *PLDR_DLL_LOAD_REASON;

#pragma endregion Ldr*;

#pragma region ASSEMBLY_STORAGE_MAP[_ENTRY][64/32]

typedef struct _ASSEMBLY_STORAGE_MAP_ENTRY {
    ULONG Flags;
    UNICODE_STRING DosPath;
    HANDLE Handle;
} ASSEMBLY_STORAGE_MAP_ENTRY, *PASSEMBLY_STORAGE_MAP_ENTRY;

typedef struct _ASSEMBLY_STORAGE_MAP_ENTRY64 {
    ULONG Flags;
    UNICODE_STRING64 DosPath;
    VOID* POINTER_64 Handle;
} ASSEMBLY_STORAGE_MAP_ENTRY64, *PASSEMBLY_STORAGE_MAP_ENTRY64;

typedef struct _ASSEMBLY_STORAGE_MAP_ENTRY32 {
    ULONG Flags;
    UNICODE_STRING32 DosPath;
    VOID* POINTER_32 Handle;
} ASSEMBLY_STORAGE_MAP_ENTRY32, *PASSEMBLY_STORAGE_MAP_ENTRY32;

typedef struct _ASSEMBLY_STORAGE_MAP {
    ULONG Flags;
    ULONG AssemblyCount;
    PASSEMBLY_STORAGE_MAP_ENTRY *AssemblyArray;
} ASSEMBLY_STORAGE_MAP, *PASSEMBLY_STORAGE_MAP;

typedef struct _ASSEMBLY_STORAGE_MAP64 {
    ULONG Flags;
    ULONG AssemblyCount;
    ASSEMBLY_STORAGE_MAP_ENTRY64* POINTER_64* AssemblyArray;
} ASSEMBLY_STORAGE_MAP64, *PASSEMBLY_STORAGE_MAP64;

typedef struct _ASSEMBLY_STORAGE_MAP32 {
    ULONG Flags;
    ULONG AssemblyCount;
    ASSEMBLY_STORAGE_MAP_ENTRY32* POINTER_32* AssemblyArray;
} ASSEMBLY_STORAGE_MAP32, *PASSEMBLY_STORAGE_MAP32;

#pragma endregion ASSEMBLY_STORAGE_MAP[_ENTRY][64/32]

#pragma region RTL_CRITICAL_SECTION[_DEBUG][64/32]

typedef struct _RTL_CRITICAL_SECTION64 RTL_CRITICAL_SECTION64, *PRTL_CRITICAL_SECTION64;
typedef struct _RTL_CRITICAL_SECTION32 RTL_CRITICAL_SECTION32, *PRTL_CRITICAL_SECTION32;

typedef struct _RTL_CRITICAL_SECTION_DEBUG64 {
    WORD Type;
    WORD CreatorBackTraceIndex;
    RTL_CRITICAL_SECTION64* POINTER_64 CriticalSection;
    LIST_ENTRY64 ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Flags;
    WORD CreatorBackTraceIndexHigh;
    WORD Identifier;
} RTL_CRITICAL_SECTION_DEBUG64, *PRTL_CRITICAL_SECTION_DEBUG64, RTL_RESOURCE_DEBUG64, *PRTL_RESOURCE_DEBUG64;

typedef struct _RTL_CRITICAL_SECTION_DEBUG32 {
    WORD Type;
    WORD CreatorBackTraceIndex;
    RTL_CRITICAL_SECTION32* POINTER_32 CriticalSection;
    LIST_ENTRY32 ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Flags;
    WORD CreatorBackTraceIndexHigh;
    WORD Identifier;
} RTL_CRITICAL_SECTION_DEBUG32, *PRTL_CRITICAL_SECTION_DEBUG32, RTL_RESOURCE_DEBUG32, *PRTL_RESOURCE_DEBUG32;

struct _RTL_CRITICAL_SECTION64 {
    RTL_CRITICAL_SECTION_DEBUG64* POINTER_64 DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    VOID* POINTER_64 OwningThread;
    VOID* POINTER_64 LockSemaphore;
    ULONGLONG SpinCount;
};

struct _RTL_CRITICAL_SECTION32 {
    RTL_CRITICAL_SECTION_DEBUG32* POINTER_32 DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    VOID* POINTER_32 OwningThread;
    VOID* POINTER_32 LockSemaphore;
    ULONG SpinCount;
};

#pragma endregion RTL_CRITICAL_SECTION[_DEBUG][64/32]

#pragma region CURDIR[64/32]

typedef struct _CURDIR {
    UNICODE_STRING DosPath;
    HANDLE Handle;
} CURDIR, *PCURDIR;

typedef struct _CURDIR64 {
    UNICODE_STRING64 DosPath;
    VOID* POINTER_64 Handle;
} CURDIR64, *PCURDIR64;

typedef struct _CURDIR32 {
    UNICODE_STRING32 DosPath;
    VOID* POINTER_32 Handle;
} CURDIR32, *PCURDIR32;

#pragma endregion CURDIR[64/32]

#pragma region LDR_DATA_TABLE_ENTRY[64/32]

typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    HMODULE DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    union {
        UCHAR FlagGroup[4];
        ULONG Flags;
        struct {
            ULONG PackagedBinary : 1;
            ULONG MarkedForRemoval : 1;
            ULONG ImageDll : 1;
            ULONG LoadNotificationsSent : 1;
            ULONG TelemetryEntryProcessed : 1;
            ULONG ProcessStaticImport : 1;
            ULONG InLegacyLists : 1;
            ULONG InIndexes : 1;
            ULONG ShimDll : 1;
            ULONG InExceptionTable : 1;
            ULONG ReservedFlags1 : 2;
            ULONG LoadInProgress : 1;
            ULONG LoadConfigProcessed : 1;
            ULONG EntryProcessed : 1;
            ULONG ProtectDelayLoad : 1;
            ULONG ReservedFlags3 : 2;
            ULONG DontCallForThreads : 1;
            ULONG ProcessAttachCalled : 1;
            ULONG ProcessAttachFailed : 1;
            ULONG CorDeferredValidate : 1;
            ULONG CorImage : 1;
            ULONG DontRelocate : 1;
            ULONG CorILOnly : 1;
            ULONG ChpeImage : 1;
            ULONG ChpeEmulatorImage : 1;
            ULONG ReservedFlags5 : 1;
            ULONG Redirected : 1;
            ULONG ReservedFlags6 : 2;
            ULONG CompatDatabaseProcessed : 1;
        };
    };
    USHORT ObsoleteLoadCount;
    USHORT TlsIndex;
    LIST_ENTRY HashLinks;
    ULONG TimeDateStamp;
    struct ACTIVATION_CONTEXT* EntryPointActivationContext;
    PVOID Lock;
    PLDR_DDAG_NODE DdagNode;
    LIST_ENTRY NodeModuleLink;
    struct LDRP_LOAD_CONTEXT* LoadContext;
    HMODULE ParentDllBase;
    PVOID SwitchBackContext;
    RTL_BALANCED_NODE BaseAddressIndexNode;
    RTL_BALANCED_NODE MappingInfoIndexNode;
    ULONG_PTR OriginalBase;
    LARGE_INTEGER LoadTime;
    ULONG BaseNameHashValue;
    LDR_DLL_LOAD_REASON LoadReason;
    ULONG ImplicitPathOptions;
    ULONG ReferenceCount;
    ULONG DependentLoadFlags;
    UCHAR SigningLevel;
    ULONG CheckSum;
    PVOID ActivePatchImageBase;
    LDR_HOT_PATCH_STATE HotPatchState;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _LDR_DATA_TABLE_ENTRY64 {
    LIST_ENTRY64 InLoadOrderModuleList;
    LIST_ENTRY64 InMemoryOrderModuleList;
    LIST_ENTRY64 InInitializationOrderModuleList;
    VOID* POINTER_64 DllBase;
    VOID* POINTER_64 EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING64 FullDllName;
    UNICODE_STRING64 BaseDllName;
    union {
        UCHAR FlagGroup[4];
        ULONG Flags;
        struct {
            ULONG PackagedBinary : 1;
            ULONG MarkedForRemoval : 1;
            ULONG ImageDll : 1;
            ULONG LoadNotificationsSent : 1;
            ULONG TelemetryEntryProcessed : 1;
            ULONG ProcessStaticImport : 1;
            ULONG InLegacyLists : 1;
            ULONG InIndexes : 1;
            ULONG ShimDll : 1;
            ULONG InExceptionTable : 1;
            ULONG ReservedFlags1 : 2;
            ULONG LoadInProgress : 1;
            ULONG LoadConfigProcessed : 1;
            ULONG EntryProcessed : 1;
            ULONG ProtectDelayLoad : 1;
            ULONG ReservedFlags3 : 2;
            ULONG DontCallForThreads : 1;
            ULONG ProcessAttachCalled : 1;
            ULONG ProcessAttachFailed : 1;
            ULONG CorDeferredValidate : 1;
            ULONG CorImage : 1;
            ULONG DontRelocate : 1;
            ULONG CorILOnly : 1;
            ULONG ChpeImage : 1;
            ULONG ChpeEmulatorImage : 1;
            ULONG ReservedFlags5 : 1;
            ULONG Redirected : 1;
            ULONG ReservedFlags6 : 2;
            ULONG CompatDatabaseProcessed : 1;
        };
    };
    USHORT ObsoleteLoadCount;
    USHORT TlsIndex;
    LIST_ENTRY64 HashLinks;
    ULONG TimeDateStamp;
    struct ACTIVATION_CONTEXT* POINTER_64 EntryPointActivationContext;
    VOID* POINTER_64 Lock;
    LDR_DDAG_NODE* POINTER_64 DdagNode; // FIXME: Too complex
    LIST_ENTRY64 NodeModuleLink;
    struct LDRP_LOAD_CONTEXT* POINTER_64 LoadContext;
    VOID* POINTER_64 ParentDllBase;
    VOID* POINTER_64 SwitchBackContext;
    RTL_BALANCED_NODE64 BaseAddressIndexNode;
    RTL_BALANCED_NODE64 MappingInfoIndexNode;
    ULONGLONG OriginalBase;
    LARGE_INTEGER LoadTime;
    ULONG BaseNameHashValue;
    LDR_DLL_LOAD_REASON LoadReason;
    ULONG ImplicitPathOptions;
    ULONG ReferenceCount;
    ULONG DependentLoadFlags;
    UCHAR SigningLevel;
    ULONG CheckSum;
    VOID* POINTER_64 ActivePatchImageBase;
    LDR_HOT_PATCH_STATE HotPatchState;
} LDR_DATA_TABLE_ENTRY64, *PLDR_DATA_TABLE_ENTRY64;

typedef struct _LDR_DATA_TABLE_ENTRY32 {
    LIST_ENTRY32 InLoadOrderModuleList;
    LIST_ENTRY32 InMemoryOrderModuleList;
    LIST_ENTRY32 InInitializationOrderModuleList;
    VOID* POINTER_32 DllBase;
    VOID* POINTER_32 EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING32 FullDllName;
    UNICODE_STRING32 BaseDllName;
    union {
        UCHAR FlagGroup[4];
        ULONG Flags;
        struct {
            ULONG PackagedBinary : 1;
            ULONG MarkedForRemoval : 1;
            ULONG ImageDll : 1;
            ULONG LoadNotificationsSent : 1;
            ULONG TelemetryEntryProcessed : 1;
            ULONG ProcessStaticImport : 1;
            ULONG InLegacyLists : 1;
            ULONG InIndexes : 1;
            ULONG ShimDll : 1;
            ULONG InExceptionTable : 1;
            ULONG ReservedFlags1 : 2;
            ULONG LoadInProgress : 1;
            ULONG LoadConfigProcessed : 1;
            ULONG EntryProcessed : 1;
            ULONG ProtectDelayLoad : 1;
            ULONG ReservedFlags3 : 2;
            ULONG DontCallForThreads : 1;
            ULONG ProcessAttachCalled : 1;
            ULONG ProcessAttachFailed : 1;
            ULONG CorDeferredValidate : 1;
            ULONG CorImage : 1;
            ULONG DontRelocate : 1;
            ULONG CorILOnly : 1;
            ULONG ChpeImage : 1;
            ULONG ChpeEmulatorImage : 1;
            ULONG ReservedFlags5 : 1;
            ULONG Redirected : 1;
            ULONG ReservedFlags6 : 2;
            ULONG CompatDatabaseProcessed : 1;
        };
    };
    USHORT ObsoleteLoadCount;
    USHORT TlsIndex;
    LIST_ENTRY32 HashLinks;
    ULONG TimeDateStamp;
    struct ACTIVATION_CONTEXT* POINTER_32 EntryPointActivationContext;
    VOID* POINTER_32 Lock;
    LDR_DDAG_NODE* POINTER_32 DdagNode; // FIXME: Too complex
    LIST_ENTRY32 NodeModuleLink;
    struct LDRP_LOAD_CONTEXT* POINTER_32 LoadContext;
    VOID* POINTER_32 ParentDllBase;
    VOID* POINTER_32 SwitchBackContext;
    RTL_BALANCED_NODE32 BaseAddressIndexNode;
    RTL_BALANCED_NODE32 MappingInfoIndexNode;
    ULONG OriginalBase;
    LARGE_INTEGER LoadTime;
    ULONG BaseNameHashValue;
    LDR_DLL_LOAD_REASON LoadReason;
    ULONG ImplicitPathOptions;
    ULONG ReferenceCount;
    ULONG DependentLoadFlags;
    UCHAR SigningLevel;
    ULONG CheckSum;
    VOID* POINTER_32 ActivePatchImageBase;
    LDR_HOT_PATCH_STATE HotPatchState;
} LDR_DATA_TABLE_ENTRY32, *PLDR_DATA_TABLE_ENTRY32;

#pragma endregion LDR_DATA_TABLE_ENTRY[64/32]

#pragma region PEB_LDR_DATA[64/32]

typedef struct _PEB_LDR_DATA {
    ULONG Length;
    BOOL Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID EntryInProgress;
    PVOID ShutdownInProgress;
    PVOID ShutdownThreadId;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB_LDR_DATA64 {
    ULONG Length;
    BOOL Initialized;
    VOID* POINTER_64 SsHandle;
    LIST_ENTRY64 InLoadOrderModuleList;
    LIST_ENTRY64 InMemoryOrderModuleList;
    LIST_ENTRY64 InInitializationOrderModuleList;
    VOID* POINTER_64 EntryInProgress;
    VOID* POINTER_64 ShutdownInProgress;
    VOID* POINTER_64 ShutdownThreadId;
} PEB_LDR_DATA64, *PPEB_LDR_DATA64;

typedef struct _PEB_LDR_DATA32 {
    ULONG Length;
    BOOL Initialized;
    VOID* POINTER_32 SsHandle;
    LIST_ENTRY32 InLoadOrderModuleList;
    LIST_ENTRY32 InMemoryOrderModuleList;
    LIST_ENTRY32 InInitializationOrderModuleList;
    VOID* POINTER_32 EntryInProgress;
    VOID* POINTER_32 ShutdownInProgress;
    VOID* POINTER_32 ShutdownThreadId;
} PEB_LDR_DATA32, *PPEB_LDR_DATA32;

#pragma endregion PEB_LDR_DATA[64/32]

#pragma region RTL_USER_PROCESS_PARAMETERS[64/32]

typedef struct _RTL_USER_PROCESS_PARAMETERS {
    ULONG MaximumLength;
    ULONG Length;
    ULONG Flags;
    ULONG DebugFlags;
    HANDLE ConsoleHandle;
    ULONG ConsoleFlags;
    HANDLE StandardInput;
    HANDLE StandardOutput;
    HANDLE StandardError;
    CURDIR CurrentDirectory;
    UNICODE_STRING DllPath;
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    LPWSTR Environment;
    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;
    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    UNICODE_STRING WindowTitle;
    UNICODE_STRING DesktopInfo;
    UNICODE_STRING ShellInfo;
    UNICODE_STRING RuntimeData;
    RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
    ULONG_PTR EnvironmentSize;
    ULONG_PTR EnvironmentVersion;
    PVOID PackageDependencyData;
    ULONG ProcessGroupId;
    ULONG LoaderThreads;
    UNICODE_STRING RedirectionDllName;
    UNICODE_STRING HeapPartitionName;
    PULONGLONG DefaultThreadpoolCpuSetMasks;
    ULONG DefaultThreadpoolCpuSetMaskCount;
    ULONG DefaultThreadpoolThreadMaximum;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _RTL_USER_PROCESS_PARAMETERS64 {
    ULONG MaximumLength;
    ULONG Length;
    ULONG Flags;
    ULONG DebugFlags;
    VOID* POINTER_64 ConsoleHandle;
    ULONG ConsoleFlags;
    VOID* POINTER_64 StandardInput;
    VOID* POINTER_64 StandardOutput;
    VOID* POINTER_64 StandardError;
    CURDIR64 CurrentDirectory;
    UNICODE_STRING64 DllPath;
    UNICODE_STRING64 ImagePathName;
    UNICODE_STRING64 CommandLine;
    WCHAR* POINTER_64 Environment;
    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;
    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    UNICODE_STRING64 WindowTitle;
    UNICODE_STRING64 DesktopInfo;
    UNICODE_STRING64 ShellInfo;
    UNICODE_STRING64 RuntimeData;
    RTL_DRIVE_LETTER_CURDIR64 CurrentDirectores[32];
    ULONGLONG EnvironmentSize;
    ULONGLONG EnvironmentVersion;
    VOID* POINTER_64 PackageDependencyData;
    ULONG ProcessGroupId;
    ULONG LoaderThreads;
    UNICODE_STRING64 RedirectionDllName;
    UNICODE_STRING64 HeapPartitionName;
    ULONGLONG* POINTER_64 DefaultThreadpoolCpuSetMasks;
    ULONG DefaultThreadpoolCpuSetMaskCount;
    ULONG DefaultThreadpoolThreadMaximum;
} RTL_USER_PROCESS_PARAMETERS64, *PRTL_USER_PROCESS_PARAMETERS64;

typedef struct _RTL_USER_PROCESS_PARAMETERS32 {
    ULONG MaximumLength;
    ULONG Length;
    ULONG Flags;
    ULONG DebugFlags;
    VOID* POINTER_32 ConsoleHandle;
    ULONG ConsoleFlags;
    VOID* POINTER_32 StandardInput;
    VOID* POINTER_32 StandardOutput;
    VOID* POINTER_32 StandardError;
    CURDIR32 CurrentDirectory;
    UNICODE_STRING32 DllPath;
    UNICODE_STRING32 ImagePathName;
    UNICODE_STRING32 CommandLine;
    WCHAR* POINTER_32 Environment;
    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;
    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    UNICODE_STRING32 WindowTitle;
    UNICODE_STRING32 DesktopInfo;
    UNICODE_STRING32 ShellInfo;
    UNICODE_STRING32 RuntimeData;
    RTL_DRIVE_LETTER_CURDIR32 CurrentDirectores[32];
    ULONG EnvironmentSize;
    ULONG EnvironmentVersion;
    VOID* POINTER_32 PackageDependencyData;
    ULONG ProcessGroupId;
    ULONG LoaderThreads;
    UNICODE_STRING32 RedirectionDllName;
    UNICODE_STRING32 HeapPartitionName;
    ULONGLONG* POINTER_32 DefaultThreadpoolCpuSetMasks;
    ULONG DefaultThreadpoolCpuSetMaskCount;
    ULONG DefaultThreadpoolThreadMaximum;
} RTL_USER_PROCESS_PARAMETERS32, *PRTL_USER_PROCESS_PARAMETERS32;

#pragma endregion RTL_USER_PROCESS_PARAMETERS[64/32]

#pragma region PEB[64/32]

typedef struct _PEB {
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    union {
        UCHAR BitField;
        struct {
            UCHAR ImageUsesLargePages : 1;
            UCHAR IsProtectedProcess : 1;
            UCHAR IsImageDynamicallyRelocated : 1;
            UCHAR SkipPatchingUser32Forwarders : 1;
            UCHAR IsPackagedProcess : 1;
            UCHAR IsAppContainer : 1;
            UCHAR IsProtectedProcessLight : 1;
            UCHAR IsLongPathAwareProcess : 1;
        };
    };
    #if _WIN64
    UCHAR Padding0[4];
    #endif
    HANDLE Mutant;
    HMODULE ImageBaseAddress;
    PPEB_LDR_DATA Ldr;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    PVOID SubSystemData;
    HANDLE ProcessHeap;
    PRTL_CRITICAL_SECTION FastPebLock;
    PSLIST_HEADER AtlThunkSListPtr;
    PVOID IFEOKey;
    union {
        ULONG CrossProcessFlags;
        struct {
            ULONG ProcessInJob : 1;
            ULONG ProcessInitializing : 1;
            ULONG ProcessUsingVEH : 1;
            ULONG ProcessUsingVCH : 1;
            ULONG ProcessUsingFTH : 1;
            ULONG ProcessPreviouslyThrottled : 1;
            ULONG ProcessCurrentlyThrottled : 1;
            ULONG ProcessImagesHotPatched : 1;
            ULONG ReservedBits0 : 24;
        };
    };
    #if _WIN64
    UCHAR Padding1[4];
    #endif
    union {
        PVOID KernelCallbackTable;
        PVOID UserSharedInfoPtr;
    };
    ULONG SystemReserved;
    ULONG AtlThunkSListPtr32;
    PVOID ApiSetMap;
    ULONG TlsExpansionCounter;
    #if _WIN64
    UCHAR Padding2[4];
    #endif
    PRTL_BITMAP TlsBitmap;
    ULONG TlsBitmapBits[2];
    PVOID ReadOnlySharedMemoryBase;
    PVOID SharedData;
    PVOID* ReadOnlyStaticServerData;
    PVOID AnsiCodePageData;
    PVOID OemCodePageData;
    PVOID UnicodeCaseTableData;
    ULONG NumberOfProcessors;
    ULONG NtGlobalFlag;
    LARGE_INTEGER CriticalSectionTimeout;
    ULONG_PTR HeapSegmentReserve;
    ULONG_PTR HeapSegmentCommit;
    ULONG_PTR HeapDeCommitTotalFreeThreshold;
    ULONG_PTR HeapDeCommitFreeBlockThreshold;
    ULONG NumberOfHeaps;
    ULONG MaximumNumberOfHeaps;
    PVOID ProcessHeaps;
    PVOID GdiSharedHandleTable;
    PVOID ProcessStarterHelper;
    ULONG GdiDCAttributeList;
    #if _WIN64
    UCHAR Padding3[4];
    #endif
    PRTL_CRITICAL_SECTION LoaderLock;
    ULONG OSMajorVersion;
    ULONG OSMinorVersion;
    USHORT OSBuildNumber;
    USHORT OSCSDVersion;
    ULONG OSPlatformId;
    ULONG ImageSubsystem;
    ULONG ImageSubsystemMajorVersion;
    ULONG ImageSubsystemMinorVersion;
    #if _WIN64
    UCHAR Padding4[4];
    #endif
    ULONG_PTR ActiveProcessAffinityMask;
    #if _WIN64
    ULONG GdiHandleBuffer[60];
    #else
    ULONG GdiHandleBuffer[34];
    #endif
    PVOID PostProcessInitRoutine;
    PRTL_BITMAP TlsExpansionBitmap;
    ULONG TlsExpansionBitmapBits[32];
    ULONG SessionId;
    #ifdef _WIN64
    UCHAR Padding5[4];
    #endif
    ULARGE_INTEGER AppCompatFlags;
    ULARGE_INTEGER AppCompatFlagsUser;
    PVOID pShimData;
    PVOID AppCompatInfo;
    UNICODE_STRING CSDVersion;
    PACTIVATION_CONTEXT_DATA ActivationContextData;
    PASSEMBLY_STORAGE_MAP ProcessAssemblyStorageMap;
    PACTIVATION_CONTEXT_DATA SystemDefaultActivationContextData;
    PASSEMBLY_STORAGE_MAP SystemAssemblyStorageMap;
    ULONG_PTR MinimumStackCommit;
    PVOID SparePointers[2];
    PVOID PatchLoaderData;
    struct CHPEV2_PROCESS_INFO* ChpeV2ProcessInfo;
    ULONG AppModelFeatureState;
    ULONG SpareUlongs[2];
    USHORT ActiveCodePage;
    USHORT OemCodePage;
    USHORT UseCaseMapping;
    USHORT UnusedNlsField;
    PVOID WerRegistrationData;
    PVOID WerShipAssertPtr;
    #ifdef _WIN64
    PVOID EcCodeBitMap;
    #else
    PVOID Spare;
    #endif
    PVOID pImageHeaderHash;
    union {
        ULONG TracingFlags;
        struct {
            ULONG HeapTracingEnabled : 1;
            ULONG CritSecTracingEnabled : 1;
            ULONG LibLoaderTracingEnabled : 1;
            ULONG SpareTracingBits : 29;
        };
    };
    #ifdef _WIN64
    UCHAR Padding6[4];
    #endif
    ULONGLONG CsrServerReadOnlySharedMemoryBase;
    ULONG_PTR TppWorkerpListLock;
    LIST_ENTRY TppWorkerpList;
    PVOID WaitOnAddressHashTable[128];
    PVOID TelemetryCoverageHeader;
    ULONG CloudFileFlags;
    ULONG CloudFileDiagFlags;
    CHAR PlaceholderCompatibilityMode;
    CHAR PlaceholderCompatibilityModeReserved[7];
    PLEAP_SECOND_DATA LeapSecondData;
    union {
        ULONG LeapSecondFlags;
        struct {
            ULONG SixtySecondEnabled : 1;
            ULONG Reserved : 31;
        };
    };
    ULONG NtGlobalFlag2;
    ULONGLONG ExtendedFeatureDisableMask;
} PEB, *PPEB;

typedef struct _PEB64 {
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    union {
        UCHAR BitField;
        struct {
            UCHAR ImageUsesLargePages : 1;
            UCHAR IsProtectedProcess : 1;
            UCHAR IsImageDynamicallyRelocated : 1;
            UCHAR SkipPatchingUser32Forwarders : 1;
            UCHAR IsPackagedProcess : 1;
            UCHAR IsAppContainer : 1;
            UCHAR IsProtectedProcessLight : 1;
            UCHAR IsLongPathAwareProcess : 1;
        };
    };
    #if _WIN64
    UCHAR Padding0[4];
    #endif
    VOID* POINTER_64 Mutant;
    VOID* POINTER_64 ImageBaseAddress;
    PEB_LDR_DATA64* POINTER_64 Ldr;
    PRTL_USER_PROCESS_PARAMETERS64 ProcessParameters;
    VOID* POINTER_64 SubSystemData;
    VOID* POINTER_64 ProcessHeap;
    RTL_CRITICAL_SECTION64* POINTER_64 FastPebLock;
    struct SLIST_HEADER* POINTER_64 AtlThunkSListPtr; // FIXME: SLIST_HEADER is depends on platform
    VOID* POINTER_64 IFEOKey;
    union {
        ULONG CrossProcessFlags;
        struct {
            ULONG ProcessInJob : 1;
            ULONG ProcessInitializing : 1;
            ULONG ProcessUsingVEH : 1;
            ULONG ProcessUsingVCH : 1;
            ULONG ProcessUsingFTH : 1;
            ULONG ProcessPreviouslyThrottled : 1;
            ULONG ProcessCurrentlyThrottled : 1;
            ULONG ProcessImagesHotPatched : 1;
            ULONG ReservedBits0 : 24;
        };
    };
    UCHAR Padding1[4];
    union {
        VOID* POINTER_64 KernelCallbackTable;
        VOID* POINTER_64 UserSharedInfoPtr;
    };
    ULONG SystemReserved;
    ULONG AtlThunkSListPtr32;
    VOID* POINTER_64 ApiSetMap;
    ULONG TlsExpansionCounter;
    UCHAR Padding2[4];
    RTL_BITMAP64* POINTER_64 TlsBitmap;
    ULONG TlsBitmapBits[2];
    VOID* POINTER_64 ReadOnlySharedMemoryBase;
    VOID* POINTER_64 SharedData;
    VOID* POINTER_64* POINTER_64 ReadOnlyStaticServerData;
    VOID* POINTER_64 AnsiCodePageData;
    VOID* POINTER_64 OemCodePageData;
    VOID* POINTER_64 UnicodeCaseTableData;
    ULONG NumberOfProcessors;
    ULONG NtGlobalFlag;
    LARGE_INTEGER CriticalSectionTimeout;
    ULONGLONG HeapSegmentReserve;
    ULONGLONG HeapSegmentCommit;
    ULONGLONG HeapDeCommitTotalFreeThreshold;
    ULONGLONG HeapDeCommitFreeBlockThreshold;
    ULONG NumberOfHeaps;
    ULONG MaximumNumberOfHeaps;
    VOID* POINTER_64 ProcessHeaps;
    VOID* POINTER_64 GdiSharedHandleTable;
    VOID* POINTER_64 ProcessStarterHelper;
    ULONG GdiDCAttributeList;
    UCHAR Padding3[4];
    RTL_CRITICAL_SECTION64* POINTER_64 LoaderLock;
    ULONG OSMajorVersion;
    ULONG OSMinorVersion;
    USHORT OSBuildNumber;
    USHORT OSCSDVersion;
    ULONG OSPlatformId;
    ULONG ImageSubsystem;
    ULONG ImageSubsystemMajorVersion;
    ULONG ImageSubsystemMinorVersion;
    UCHAR Padding4[4];
    ULONGLONG ActiveProcessAffinityMask;
    ULONG GdiHandleBuffer[60];
    VOID* POINTER_64 PostProcessInitRoutine;
    RTL_BITMAP64* POINTER_64 TlsExpansionBitmap;
    ULONG TlsExpansionBitmapBits[32];
    ULONG SessionId;
    UCHAR Padding5[4];
    ULARGE_INTEGER AppCompatFlags;
    ULARGE_INTEGER AppCompatFlagsUser;
    VOID* POINTER_64 pShimData;
    VOID* POINTER_64 AppCompatInfo;
    UNICODE_STRING64 CSDVersion;
    ACTIVATION_CONTEXT_DATA* POINTER_64 ActivationContextData;
    ASSEMBLY_STORAGE_MAP* POINTER_64 ProcessAssemblyStorageMap;
    ACTIVATION_CONTEXT_DATA* POINTER_64 SystemDefaultActivationContextData;
    ASSEMBLY_STORAGE_MAP* POINTER_64 SystemAssemblyStorageMap;
    ULONGLONG MinimumStackCommit;
    VOID* POINTER_64 SparePointers[2];
    VOID* POINTER_64 PatchLoaderData;
    struct CHPEV2_PROCESS_INFO* POINTER_64 ChpeV2ProcessInfo;
    ULONG AppModelFeatureState;
    ULONG SpareUlongs[2];
    USHORT ActiveCodePage;
    USHORT OemCodePage;
    USHORT UseCaseMapping;
    USHORT UnusedNlsField;
    VOID* POINTER_64 WerRegistrationData;
    VOID* POINTER_64 WerShipAssertPtr;
    VOID* POINTER_64 EcCodeBitMap;
    VOID* POINTER_64 pImageHeaderHash;
    union {
        ULONG TracingFlags;
        struct {
            ULONG HeapTracingEnabled : 1;
            ULONG CritSecTracingEnabled : 1;
            ULONG LibLoaderTracingEnabled : 1;
            ULONG SpareTracingBits : 29;
        };
    };
    UCHAR Padding6[4];
    ULONGLONG CsrServerReadOnlySharedMemoryBase;
    ULONGLONG TppWorkerpListLock;
    LIST_ENTRY64 TppWorkerpList;
    VOID* POINTER_64 WaitOnAddressHashTable[128];
    VOID* POINTER_64 TelemetryCoverageHeader;
    ULONG CloudFileFlags;
    ULONG CloudFileDiagFlags;
    CHAR PlaceholderCompatibilityMode;
    CHAR PlaceholderCompatibilityModeReserved[7];
    LEAP_SECOND_DATA* POINTER_64 LeapSecondData;
    union {
        ULONG LeapSecondFlags;
        struct {
            ULONG SixtySecondEnabled : 1;
            ULONG Reserved : 31;
        };
    };
    ULONG NtGlobalFlag2;
    ULONGLONG ExtendedFeatureDisableMask;
} PEB64, *PPEB64;

typedef struct _PEB32 {
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    union {
        UCHAR BitField;
        struct {
            UCHAR ImageUsesLargePages : 1;
            UCHAR IsProtectedProcess : 1;
            UCHAR IsImageDynamicallyRelocated : 1;
            UCHAR SkipPatchingUser32Forwarders : 1;
            UCHAR IsPackagedProcess : 1;
            UCHAR IsAppContainer : 1;
            UCHAR IsProtectedProcessLight : 1;
            UCHAR IsLongPathAwareProcess : 1;
        };
    };
    VOID* POINTER_32 Mutant;
    VOID* POINTER_32 ImageBaseAddress;
    PEB_LDR_DATA32* POINTER_32 Ldr;
    RTL_USER_PROCESS_PARAMETERS32* POINTER_32 ProcessParameters;
    VOID* POINTER_32 SubSystemData;
    VOID* POINTER_32 ProcessHeap;
    RTL_CRITICAL_SECTION32* POINTER_32 FastPebLock;
    struct SLIST_HEADER* POINTER_32 AtlThunkSListPtr; // FIXME: SLIST_HEADER is depends on platform
    VOID* POINTER_32 IFEOKey;
    union {
        ULONG CrossProcessFlags;
        struct {
            ULONG ProcessInJob : 1;
            ULONG ProcessInitializing : 1;
            ULONG ProcessUsingVEH : 1;
            ULONG ProcessUsingVCH : 1;
            ULONG ProcessUsingFTH : 1;
            ULONG ProcessPreviouslyThrottled : 1;
            ULONG ProcessCurrentlyThrottled : 1;
            ULONG ProcessImagesHotPatched : 1;
            ULONG ReservedBits0 : 24;
        };
    };
    union {
        VOID* POINTER_32 KernelCallbackTable;
        VOID* POINTER_32 UserSharedInfoPtr;
    };
    ULONG SystemReserved;
    ULONG AtlThunkSListPtr32;
    VOID* POINTER_32 ApiSetMap;
    ULONG TlsExpansionCounter;
    RTL_BITMAP32* POINTER_32 TlsBitmap;
    ULONG TlsBitmapBits[2];
    VOID* POINTER_32 ReadOnlySharedMemoryBase;
    VOID* POINTER_32 SharedData;
    VOID* POINTER_32* POINTER_32 ReadOnlyStaticServerData;
    VOID* POINTER_32 AnsiCodePageData;
    VOID* POINTER_32 OemCodePageData;
    VOID* POINTER_32 UnicodeCaseTableData;
    ULONG NumberOfProcessors;
    ULONG NtGlobalFlag;
    LARGE_INTEGER CriticalSectionTimeout;
    ULONG HeapSegmentReserve;
    ULONG HeapSegmentCommit;
    ULONG HeapDeCommitTotalFreeThreshold;
    ULONG HeapDeCommitFreeBlockThreshold;
    ULONG NumberOfHeaps;
    ULONG MaximumNumberOfHeaps;
    VOID* POINTER_32 ProcessHeaps;
    VOID* POINTER_32 GdiSharedHandleTable;
    VOID* POINTER_32 ProcessStarterHelper;
    ULONG GdiDCAttributeList;
    RTL_CRITICAL_SECTION32* POINTER_32 LoaderLock;
    ULONG OSMajorVersion;
    ULONG OSMinorVersion;
    USHORT OSBuildNumber;
    USHORT OSCSDVersion;
    ULONG OSPlatformId;
    ULONG ImageSubsystem;
    ULONG ImageSubsystemMajorVersion;
    ULONG ImageSubsystemMinorVersion;
    ULONG ActiveProcessAffinityMask;
    ULONG GdiHandleBuffer[34];
    VOID* POINTER_32 PostProcessInitRoutine;
    RTL_BITMAP32* POINTER_32 TlsExpansionBitmap;
    ULONG TlsExpansionBitmapBits[32];
    ULONG SessionId;
    ULARGE_INTEGER AppCompatFlags;
    ULARGE_INTEGER AppCompatFlagsUser;
    VOID* POINTER_32 pShimData;
    VOID* POINTER_32 AppCompatInfo;
    UNICODE_STRING32 CSDVersion;
    ACTIVATION_CONTEXT_DATA* POINTER_32 ActivationContextData;
    ASSEMBLY_STORAGE_MAP32* POINTER_32 ProcessAssemblyStorageMap;
    ACTIVATION_CONTEXT_DATA* POINTER_32 SystemDefaultActivationContextData;
    ASSEMBLY_STORAGE_MAP32* POINTER_32 SystemAssemblyStorageMap;
    ULONG MinimumStackCommit;
    VOID* POINTER_32 SparePointers[2];
    VOID* POINTER_32 PatchLoaderData;
    struct CHPEV2_PROCESS_INFO* POINTER_32 ChpeV2ProcessInfo;
    ULONG AppModelFeatureState;
    ULONG SpareUlongs[2];
    USHORT ActiveCodePage;
    USHORT OemCodePage;
    USHORT UseCaseMapping;
    USHORT UnusedNlsField;
    VOID* POINTER_32 WerRegistrationData;
    VOID* POINTER_32 WerShipAssertPtr;
    VOID* POINTER_32 Spare;
    VOID* POINTER_32 pImageHeaderHash;
    union {
        ULONG TracingFlags;
        struct {
            ULONG HeapTracingEnabled : 1;
            ULONG CritSecTracingEnabled : 1;
            ULONG LibLoaderTracingEnabled : 1;
            ULONG SpareTracingBits : 29;
        };
    };
    ULONGLONG CsrServerReadOnlySharedMemoryBase;
    ULONG TppWorkerpListLock;
    LIST_ENTRY32 TppWorkerpList;
    VOID* POINTER_32 WaitOnAddressHashTable[128];
    VOID* POINTER_32 TelemetryCoverageHeader;
    ULONG CloudFileFlags;
    ULONG CloudFileDiagFlags;
    CHAR PlaceholderCompatibilityMode;
    CHAR PlaceholderCompatibilityModeReserved[7];
    LEAP_SECOND_DATA* POINTER_32 LeapSecondData;
    union {
        ULONG LeapSecondFlags;
        struct {
            ULONG SixtySecondEnabled : 1;
            ULONG Reserved : 31;
        };
    };
    ULONG NtGlobalFlag2;
    ULONGLONG ExtendedFeatureDisableMask;
} PEB32, *PPEB32;

#pragma endregion PEB[64/32]

#pragma region ACTIVATION_CONTEXT_STACK[_FRAME][64/32]

typedef struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME RTL_ACTIVATION_CONTEXT_STACK_FRAME, *PRTL_ACTIVATION_CONTEXT_STACK_FRAME;
typedef struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME64 RTL_ACTIVATION_CONTEXT_STACK_FRAME64, *PRTL_ACTIVATION_CONTEXT_STACK_FRAME64;
typedef struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME32 RTL_ACTIVATION_CONTEXT_STACK_FRAME32, *PRTL_ACTIVATION_CONTEXT_STACK_FRAME32;

struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME {
    struct RTL_ACTIVATION_CONTEXT_STACK_FRAME* Previous;
    struct ACTIVATION_CONTEXT* ActivationContext;
    DWORD Flags;
};

struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME64 {
    struct RTL_ACTIVATION_CONTEXT_STACK_FRAME* POINTER_64 Previous;
    struct ACTIVATION_CONTEXT* POINTER_64 ActivationContext;
    ULONG Flags;
};

struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME32 {
    struct RTL_ACTIVATION_CONTEXT_STACK_FRAME* POINTER_32 Previous;
    struct ACTIVATION_CONTEXT* POINTER_32 ActivationContext;
    ULONG Flags;
};

typedef struct _ACTIVATION_CONTEXT_STACK {
    PRTL_ACTIVATION_CONTEXT_STACK_FRAME ActiveFrame;
    LIST_ENTRY FrameListCache;
    ULONG Flags;
    ULONG NextCookieSequenceNumber;
    ULONG StackId;
} ACTIVATION_CONTEXT_STACK, *PACTIVATION_CONTEXT_STACK;

typedef struct _ACTIVATION_CONTEXT_STACK64 {
    RTL_ACTIVATION_CONTEXT_STACK_FRAME64* POINTER_64 ActiveFrame;
    LIST_ENTRY64 FrameListCache;
    ULONG Flags;
    ULONG NextCookieSequenceNumber;
    ULONG StackId;
} ACTIVATION_CONTEXT_STACK64, *PACTIVATION_CONTEXT_STACK64;

typedef struct _ACTIVATION_CONTEXT_STACK32 {
    RTL_ACTIVATION_CONTEXT_STACK_FRAME32* POINTER_32 ActiveFrame;
    LIST_ENTRY32 FrameListCache;
    ULONG Flags;
    ULONG NextCookieSequenceNumber;
    ULONG StackId;
} ACTIVATION_CONTEXT_STACK32, *PACTIVATION_CONTEXT_STACK32;

#pragma endregion ACTIVATION_CONTEXT_STACK[_FRAME][64/32]

#pragma region GDI_TEB_BATCH[64/32]

typedef struct _GDI_TEB_BATCH {
    struct {
        ULONG Offset : 31;
        BOOL HasRenderingCommand : 1;
    };
    ULONG_PTR HDC;
    ULONG Buffer[310];
} GDI_TEB_BATCH, *PGDI_TEB_BATCH;

typedef struct _GDI_TEB_BATCH64 {
    struct {
        ULONG Offset : 31;
        BOOL HasRenderingCommand : 1;
    };
    ULONGLONG HDC;
    ULONG Buffer[310];
} GDI_TEB_BATCH64, *PGDI_TEB_BATCH64;

typedef struct _GDI_TEB_BATCH32 {
    struct {
        ULONG Offset : 31;
        BOOL HasRenderingCommand : 1;
    };
    ULONG HDC;
    ULONG Buffer[310];
} GDI_TEB_BATCH32, *PGDI_TEB_BATCH32;

#pragma endregion GDI_TEB_BATCH[64/32]

#pragma region TEB_ACTIVE_FRAME[_CONTEXT][64/32]

typedef struct _TEB_ACTIVE_FRAME_CONTEXT {
    ULONG Flags;
    UCHAR Padding[4];
    PCHAR FrameName;
} TEB_ACTIVE_FRAME_CONTEXT, *PTEB_ACTIVE_FRAME_CONTEXT;

typedef struct _TEB_ACTIVE_FRAME_CONTEXT64 {
    ULONG Flags;
    UCHAR Padding[4];
    CHAR* POINTER_64 FrameName;
} TEB_ACTIVE_FRAME_CONTEXT64, *PTEB_ACTIVE_FRAME_CONTEXT64;

typedef struct _TEB_ACTIVE_FRAME_CONTEXT32 {
    ULONG Flags;
    UCHAR Padding[4];
    CHAR* POINTER_32 FrameName;
} TEB_ACTIVE_FRAME_CONTEXT32, *PTEB_ACTIVE_FRAME_CONTEXT32;

typedef struct _TEB_ACTIVE_FRAME TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;
typedef struct _TEB_ACTIVE_FRAME64 TEB_ACTIVE_FRAME64, *PTEB_ACTIVE_FRAME64;
typedef struct _TEB_ACTIVE_FRAME32 TEB_ACTIVE_FRAME32, *PTEB_ACTIVE_FRAME32;

struct _TEB_ACTIVE_FRAME {
    DWORD Flags;
    #if _WIN64
    UCHAR Padding[4];
    #endif
    struct TEB_ACTIVE_FRAME* Previous;
    PTEB_ACTIVE_FRAME_CONTEXT Context;
};

struct _TEB_ACTIVE_FRAME64 {
    DWORD Flags;
    UCHAR Padding[4];
    struct TEB_ACTIVE_FRAME64* Previous;
    TEB_ACTIVE_FRAME_CONTEXT64* POINTER_64 Context;
};

struct _TEB_ACTIVE_FRAME32 {
    DWORD Flags;
    struct TEB_ACTIVE_FRAME32* Previous;
    TEB_ACTIVE_FRAME_CONTEXT32* POINTER_32 Context;
};

#pragma endregion TEB_ACTIVE_FRAME[_CONTEXT][64/32]

#pragma region TEB[64/32]

typedef struct _TEB {
    NT_TIB NtTib;
    LPWSTR EnvironmentPointer;
    CLIENT_ID ClientId;
    PVOID ActiveRpcHandle;
    PVOID ThreadLocalStoragePointer;
    PPEB ProcessEnvironmentBlock;
    ULONG LastErrorValue;
    ULONG CountOfOwnedCriticalSections;
    PVOID CsrClientThread;
    PVOID Win32ThreadInfo;
    ULONG User32Reserved[26];
    ULONG UserReserved[5];
    PVOID WOW32Reserved;
    ULONG CurrentLocale;
    ULONG FpSoftwareStatusRegister;
    PVOID ReservedForDebuggerInstrumentation[16];
    #ifdef _WIN64
    PVOID SystemReserved1[30];
    #else
    PVOID SystemReserved1[26];
    #endif
    CHAR PlaceholderCompatibilityMode;
    UCHAR PlaceholderHydrationAlwaysExplicit;
    CHAR PlaceholderReserved[10];
    ULONG ProxiedProcessId;
    ACTIVATION_CONTEXT_STACK _ActivationStack;
    UCHAR WorkingOnBehalfTicket[8];
    LONG ExceptionCode;
    #ifdef _WIN64
    UCHAR Padding0[4];
    #endif
    PACTIVATION_CONTEXT_STACK ActivationContextStackPointer;
    PVOID InstrumentationCallbackSp;
    PVOID InstrumentationCallbackPreviousPc;
    PVOID InstrumentationCallbackPreviousSp;
    #ifdef _WIN64
    ULONG TxFsContext;
    BOOLEAN InstrumentationCallbackDisabled;
    UCHAR UnalignedLoadStoreExceptions;
    UCHAR Padding1[2];
    #else
    UCHAR InstrumentationCallbackDisabled;
    UCHAR SpareBytes[23];
    DWORD TxFsContext;
    #endif
    GDI_TEB_BATCH GdiTebBatch;
    CLIENT_ID RealClientId;
    PVOID GdiCachedProcessHandle;
    ULONG GdiClientPID;
    ULONG GdiClientTID;
    PVOID GdiThreadLocalInfo;
    ULONG_PTR Win32ClientInfo[62];
    PVOID glDispatchTable[233];
    ULONG_PTR glReserved1[29];
    PVOID glReserved2;
    PVOID glSectionInfo;
    PVOID glSection;
    PVOID glTable;
    PVOID glCurrentRC;
    PVOID glContext;
    ULONG LastStatusValue;
    #ifdef _WIN64
    UCHAR Padding2[4];
    #endif
    UNICODE_STRING StaticUnicodeString;
    WCHAR StaticUnicodeBuffer[261];
    #ifdef _WIN64
    UCHAR Padding3[6];
    #endif
    PVOID DeallocationStack;
    PVOID TlsSlots[64];
    LIST_ENTRY TlsLinks;
    PVOID Vdm;
    PVOID ReservedForNtRpc;
    PVOID DbgSsReserved[2];
    ULONG HardErrorMode;
    #ifdef _WIN64
    UCHAR Padding4[4];
    PVOID Instrumentation[11];
    #else
    PVOID Instrumentation[9];
    #endif
    GUID ActivityId;
    PVOID SubProcessTag;
    PVOID PerflibData;
    PVOID EtwTraceData;
    PVOID WinSockData;
    ULONG GdiBatchCount;
    union {
        PROCESSOR_NUMBER CurrentIdealProcessor;
        union {
            DWORD IdealProcessorValue;
            struct {
                UCHAR ReservedPad0;
                UCHAR ReservedPad1;
                UCHAR ReservedPad2;
                UCHAR IdealProcessor;
            };
        };
    };
    ULONG GuaranteedStackBytes;
    #ifdef _WIN64
    UCHAR Padding5[4];
    #endif
    PVOID ReservedForPerf;
    PVOID ReservedForOle;
    ULONG WaitingOnLoaderLock;
    #ifdef _WIN64
    UCHAR Padding6[4];
    #endif
    PVOID SavedPriorityState;
    ULONG_PTR ReservedForCodeCoverage;
    PVOID ThreadPoolData;
    PVOID TlsExpansionSlots;
    #ifdef _WIN64
    struct CHPEV2_CPUAREA_INFO* POINTER_64 ChpeV2CpuAreaInfo;
    PVOID Unused;
    #endif
    ULONG MuiGeneration;
    ULONG IsImpersonating;
    PVOID NlsCache;
    PVOID pShimData;
    ULONG HeapData;
    #ifdef _WIN64
    UCHAR Padding7[4];
    #endif
    PVOID CurrentTransactionHandle;
    PTEB_ACTIVE_FRAME ActiveFrame;
    PVOID FlsData;
    PVOID PreferredLanguages;
    PVOID UserPrefLanguages;
    PVOID MergedPrefLanguages;
    ULONG MuiImpersonation;
    union {
        USHORT CrossTebFlags;
        struct {
            USHORT SpareCrossTebBits : 16;
        };
    };
    union {
        USHORT SameTebFlags;
        struct {
            USHORT SafeThunkCall : 1;
            USHORT InDebugPrint : 1;
            USHORT HasFiberData : 1;
            USHORT SkipThreadAttach : 1;
            USHORT WerInShipAssertCode : 1;
            USHORT RanProcessInit : 1;
            USHORT ClonedThread : 1;
            USHORT SuppressDebugMsg : 1;
            USHORT DisableUserStackWalk : 1;
            USHORT RtlExceptionAttached : 1;
            USHORT InitialThread : 1;
            USHORT SessionAware : 1;
            USHORT LoadOwner : 1;
            USHORT LoaderWorker : 1;
            USHORT SkipLoaderInit : 1;
            USHORT SkipFileAPIBrokering : 1;
        };
    };
    PVOID TxnScopeEnterCallback;
    PVOID TxnScopeExitCallback;
    PVOID TxnScopeContext;
    ULONG LockCount;
    LONG WowTebOffset;
    PVOID ResourceRetValue;
    PVOID ReservedForWdf;
    ULONGLONG ReservedForCrt;
    GUID EffectiveContainerId;
    ULONGLONG LastSleepCounter;
    ULONG SpinCallCount;
    #ifdef _WIN64
    UCHAR Padding8[4];
    #endif
    ULONGLONG ExtendedFeatureDisableMask;
} TEB, *PTEB;

typedef struct _TEB64 {
    NT_TIB64 NtTib;
    WCHAR* POINTER_64 EnvironmentPointer;
    CLIENT_ID64 ClientId;
    VOID* POINTER_64 ActiveRpcHandle;
    VOID* POINTER_64 ThreadLocalStoragePointer;
    PEB64* POINTER_64 ProcessEnvironmentBlock;
    ULONG LastErrorValue;
    ULONG CountOfOwnedCriticalSections;
    VOID* POINTER_64 CsrClientThread;
    VOID* POINTER_64 Win32ThreadInfo;
    ULONG User32Reserved[26];
    ULONG UserReserved[5];
    VOID* POINTER_64 WOW32Reserved;
    ULONG CurrentLocale;
    ULONG FpSoftwareStatusRegister;
    VOID* POINTER_64 ReservedForDebuggerInstrumentation[16];
    VOID* POINTER_64 SystemReserved1[30];
    CHAR PlaceholderCompatibilityMode;
    UCHAR PlaceholderHydrationAlwaysExplicit;
    CHAR PlaceholderReserved[10];
    ULONG ProxiedProcessId;
    ACTIVATION_CONTEXT_STACK64 _ActivationStack;
    UCHAR WorkingOnBehalfTicket[8];
    LONG ExceptionCode;
    UCHAR Padding0[4];
    ACTIVATION_CONTEXT_STACK64* POINTER_64 ActivationContextStackPointer;
    VOID* POINTER_64 InstrumentationCallbackSp;
    VOID* POINTER_64 InstrumentationCallbackPreviousPc;
    VOID* POINTER_64 InstrumentationCallbackPreviousSp;
    ULONG TxFsContext;
    BOOLEAN InstrumentationCallbackDisabled;
    UCHAR UnalignedLoadStoreExceptions;
    UCHAR Padding1[2];
    GDI_TEB_BATCH64 GdiTebBatch;
    CLIENT_ID64 RealClientId;
    VOID* POINTER_64 GdiCachedProcessHandle;
    ULONG GdiClientPID;
    ULONG GdiClientTID;
    VOID* POINTER_64 GdiThreadLocalInfo;
    ULONGLONG Win32ClientInfo[62];
    VOID* POINTER_64 glDispatchTable[233];
    ULONGLONG glReserved1[29];
    VOID* POINTER_64 glReserved2;
    VOID* POINTER_64 glSectionInfo;
    VOID* POINTER_64 glSection;
    VOID* POINTER_64 glTable;
    VOID* POINTER_64 glCurrentRC;
    VOID* POINTER_64 glContext;
    ULONG LastStatusValue;
    UCHAR Padding2[4];
    UNICODE_STRING64 StaticUnicodeString;
    WCHAR StaticUnicodeBuffer[261];
    UCHAR Padding3[6];
    VOID* POINTER_64 DeallocationStack;
    VOID* POINTER_64 TlsSlots[64];
    LIST_ENTRY64 TlsLinks;
    VOID* POINTER_64 Vdm;
    VOID* POINTER_64 ReservedForNtRpc;
    VOID* POINTER_64 DbgSsReserved[2];
    ULONG HardErrorMode;
    UCHAR Padding4[4];
    VOID* POINTER_64 Instrumentation[11];
    GUID ActivityId;
    VOID* POINTER_64 SubProcessTag;
    VOID* POINTER_64 PerflibData;
    VOID* POINTER_64 EtwTraceData;
    VOID* POINTER_64 WinSockData;
    ULONG GdiBatchCount;
    union {
        PROCESSOR_NUMBER CurrentIdealProcessor;
        union {
            DWORD IdealProcessorValue;
            struct {
                UCHAR ReservedPad0;
                UCHAR ReservedPad1;
                UCHAR ReservedPad2;
                UCHAR IdealProcessor;
            };
        };
    };
    ULONG GuaranteedStackBytes;
    UCHAR Padding5[4];
    VOID* POINTER_64 ReservedForPerf;
    VOID* POINTER_64 ReservedForOle;
    ULONG WaitingOnLoaderLock;
    UCHAR Padding6[4];
    VOID* POINTER_64 SavedPriorityState;
    ULONGLONG ReservedForCodeCoverage;
    VOID* POINTER_64 ThreadPoolData;
    VOID* POINTER_64 TlsExpansionSlots;
    struct CHPEV2_CPUAREA_INFO* POINTER_64 ChpeV2CpuAreaInfo;
    VOID* POINTER_64 Unused;
    ULONG MuiGeneration;
    ULONG IsImpersonating;
    VOID* POINTER_64 NlsCache;
    VOID* POINTER_64 pShimData;
    ULONG HeapData;
    UCHAR Padding7[4];
    VOID* POINTER_64 CurrentTransactionHandle;
    TEB_ACTIVE_FRAME64* POINTER_64 ActiveFrame;
    VOID* POINTER_64 FlsData;
    VOID* POINTER_64 PreferredLanguages;
    VOID* POINTER_64 UserPrefLanguages;
    VOID* POINTER_64 MergedPrefLanguages;
    ULONG MuiImpersonation;
    union {
        USHORT CrossTebFlags;
        struct {
            USHORT SpareCrossTebBits : 16;
        };
    };
    union {
        USHORT SameTebFlags;
        struct {
            USHORT SafeThunkCall : 1;
            USHORT InDebugPrint : 1;
            USHORT HasFiberData : 1;
            USHORT SkipThreadAttach : 1;
            USHORT WerInShipAssertCode : 1;
            USHORT RanProcessInit : 1;
            USHORT ClonedThread : 1;
            USHORT SuppressDebugMsg : 1;
            USHORT DisableUserStackWalk : 1;
            USHORT RtlExceptionAttached : 1;
            USHORT InitialThread : 1;
            USHORT SessionAware : 1;
            USHORT LoadOwner : 1;
            USHORT LoaderWorker : 1;
            USHORT SkipLoaderInit : 1;
            USHORT SkipFileAPIBrokering : 1;
        };
    };
    VOID* POINTER_64 TxnScopeEnterCallback;
    VOID* POINTER_64 TxnScopeExitCallback;
    VOID* POINTER_64 TxnScopeContext;
    ULONG LockCount;
    LONG WowTebOffset;
    VOID* POINTER_64 ResourceRetValue;
    VOID* POINTER_64 ReservedForWdf;
    ULONGLONG ReservedForCrt;
    GUID EffectiveContainerId;
    ULONGLONG LastSleepCounter;
    ULONG SpinCallCount;
    UCHAR Padding8[4];
    ULONGLONG ExtendedFeatureDisableMask;
} TEB64, *PTEB64;

typedef struct _TEB32 {
    NT_TIB32 NtTib;
    WCHAR* POINTER_32 EnvironmentPointer;
    CLIENT_ID32 ClientId;
    VOID* POINTER_32 ActiveRpcHandle;
    VOID* POINTER_32 ThreadLocalStoragePointer;
    PEB32* POINTER_32 ProcessEnvironmentBlock;
    ULONG LastErrorValue;
    ULONG CountOfOwnedCriticalSections;
    VOID* POINTER_32 CsrClientThread;
    VOID* POINTER_32 Win32ThreadInfo;
    ULONG User32Reserved[26];
    ULONG UserReserved[5];
    VOID* POINTER_32 WOW32Reserved;
    ULONG CurrentLocale;
    ULONG FpSoftwareStatusRegister;
    VOID* POINTER_32 ReservedForDebuggerInstrumentation[16];
    VOID* POINTER_32 SystemReserved1[26];
    CHAR PlaceholderCompatibilityMode;
    UCHAR PlaceholderHydrationAlwaysExplicit;
    CHAR PlaceholderReserved[10];
    ULONG ProxiedProcessId;
    ACTIVATION_CONTEXT_STACK32 _ActivationStack;
    UCHAR WorkingOnBehalfTicket[8];
    LONG ExceptionCode;
    ACTIVATION_CONTEXT_STACK32* POINTER_32 ActivationContextStackPointer;
    VOID* POINTER_32 InstrumentationCallbackSp;
    VOID* POINTER_32 InstrumentationCallbackPreviousPc;
    VOID* POINTER_32 InstrumentationCallbackPreviousSp;
    UCHAR InstrumentationCallbackDisabled;
    UCHAR SpareBytes[23];
    ULONG TxFsContext;
    GDI_TEB_BATCH32 GdiTebBatch;
    CLIENT_ID32 RealClientId;
    VOID* POINTER_32 GdiCachedProcessHandle;
    ULONG GdiClientPID;
    ULONG GdiClientTID;
    VOID* POINTER_32 GdiThreadLocalInfo;
    ULONG Win32ClientInfo[62];
    VOID* POINTER_32 glDispatchTable[233];
    ULONG glReserved1[29];
    VOID* POINTER_32 glReserved2;
    VOID* POINTER_32 glSectionInfo;
    VOID* POINTER_32 glSection;
    VOID* POINTER_32 glTable;
    VOID* POINTER_32 glCurrentRC;
    VOID* POINTER_32 glContext;
    ULONG LastStatusValue;
    UNICODE_STRING32 StaticUnicodeString;
    WCHAR StaticUnicodeBuffer[261];
    VOID* POINTER_32 DeallocationStack;
    VOID* POINTER_32 TlsSlots[64];
    LIST_ENTRY32 TlsLinks;
    VOID* POINTER_32 Vdm;
    VOID* POINTER_32 ReservedForNtRpc;
    VOID* POINTER_32 DbgSsReserved[2];
    ULONG HardErrorMode;
    VOID* POINTER_32 Instrumentation[9];
    GUID ActivityId;
    VOID* POINTER_32 SubProcessTag;
    VOID* POINTER_32 PerflibData;
    VOID* POINTER_32 EtwTraceData;
    VOID* POINTER_32 WinSockData;
    ULONG GdiBatchCount;
    union {
        PROCESSOR_NUMBER CurrentIdealProcessor;
        union {
            DWORD IdealProcessorValue;
            struct {
                UCHAR ReservedPad0;
                UCHAR ReservedPad1;
                UCHAR ReservedPad2;
                UCHAR IdealProcessor;
            };
        };
    };
    ULONG GuaranteedStackBytes;
    VOID* POINTER_32 ReservedForPerf;
    VOID* POINTER_32 ReservedForOle;
    ULONG WaitingOnLoaderLock;
    VOID* POINTER_32 SavedPriorityState;
    ULONG ReservedForCodeCoverage;
    VOID* POINTER_32 ThreadPoolData;
    VOID* POINTER_32 TlsExpansionSlots;
    ULONG MuiGeneration;
    ULONG IsImpersonating;
    VOID* POINTER_32 NlsCache;
    VOID* POINTER_32 pShimData;
    ULONG HeapData;
    VOID* POINTER_32 CurrentTransactionHandle;
    TEB_ACTIVE_FRAME32* POINTER_32 ActiveFrame;
    VOID* POINTER_32 FlsData;
    VOID* POINTER_32 PreferredLanguages;
    VOID* POINTER_32 UserPrefLanguages;
    VOID* POINTER_32 MergedPrefLanguages;
    ULONG MuiImpersonation;
    union {
        USHORT CrossTebFlags;
        struct {
            USHORT SpareCrossTebBits : 16;
        };
    };
    union {
        USHORT SameTebFlags;
        struct {
            USHORT SafeThunkCall : 1;
            USHORT InDebugPrint : 1;
            USHORT HasFiberData : 1;
            USHORT SkipThreadAttach : 1;
            USHORT WerInShipAssertCode : 1;
            USHORT RanProcessInit : 1;
            USHORT ClonedThread : 1;
            USHORT SuppressDebugMsg : 1;
            USHORT DisableUserStackWalk : 1;
            USHORT RtlExceptionAttached : 1;
            USHORT InitialThread : 1;
            USHORT SessionAware : 1;
            USHORT LoadOwner : 1;
            USHORT LoaderWorker : 1;
            USHORT SkipLoaderInit : 1;
            USHORT SkipFileAPIBrokering : 1;
        };
    };
    VOID* POINTER_32 TxnScopeEnterCallback;
    VOID* POINTER_32 TxnScopeExitCallback;
    VOID* POINTER_32 TxnScopeContext;
    ULONG LockCount;
    LONG WowTebOffset;
    VOID* POINTER_32 ResourceRetValue;
    VOID* POINTER_32 ReservedForWdf;
    ULONGLONG ReservedForCrt;
    GUID EffectiveContainerId;
    ULONGLONG LastSleepCounter;
    ULONG SpinCallCount;
    ULONGLONG ExtendedFeatureDisableMask;
} TEB32, *PTEB32;

#pragma endregion TEB[64/32]
