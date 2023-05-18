#pragma once

typedef union _WIE_CPU_INFO {
    struct {
        int Eax;
        int Ebx;
        int Ecx;
        int Edx;
    };

    /* F_Leaf_SubLeaf, aka F_EAX_ECX */

    /* 00-03: Basic CPUID Information */
    struct {
        /* Eax */
        unsigned long MaxInputValue;
        /* Ebx, Ecx, Edx */
        unsigned long VendorIdString[3];
    } F00_00;
    struct {
        /*
         * Eax: Intel CPU version information
         *
         *  31 28 27                20 19               16 15 14 13            12 11        8 7     4 3           0
         * +-----+--------------------+-------------------+-----+----------------+-----------+-------+-------------+
         * |     | Extended Family ID | Extended Model ID |     | Processor Type | Family ID | Model | Stepping ID |
         * +-----+--------------------+-------------------+-----+----------------+-----------+-------+-------------+
         *
         */
        struct {
            unsigned long SteppingId : 4;
            unsigned long Model : 4;
            unsigned long FamilyId : 4;
            unsigned long ProcessorType : 2;
            unsigned long ReservedBits0 : 2;
            unsigned long ExtendedModelId : 4;
            unsigned long ExtendedFamilyId : 8;
            unsigned long ReservedBits1 : 4;
        } VersionInfo;
        /* Ebx: Additional information */
        struct {
            unsigned long BrandIndex : 8;
            unsigned long CLFlushLineSize : 8;
            unsigned long MaxNumberOfAddressableId : 8;
            unsigned long InitialAPICId : 8;
        } AdditionalInfo;
        /* Ecx, Edx: Feature information */
        struct {
            /* Ecx */
            struct {
                unsigned long SSE3 : 1;                /* 00 SSE3 */
                unsigned long PCLMULQDQ : 1;           /* 01 PCLMULQDQ */
                unsigned long DTES64 : 1;              /* 02 64-bit DS Area */
                unsigned long MONITOR : 1;             /* 03 MONITOR/MWAIT */
                unsigned long DS_CPL : 1;              /* 04 CPL Qualified Debug Store */
                unsigned long VMX : 1;                 /* 05 Virtual Machine Extensions */
                unsigned long SMX : 1;                 /* 06 Safer Mode Extensions */
                unsigned long EIST : 1;                /* 07 Enhanced Intel SpeedStep® technology */
                unsigned long TM2 : 1;                 /* 08 Thermal Monitor 2 */
                unsigned long SSSE3 : 1;               /* 09 SSSE3 */
                unsigned long CNXT_ID : 1;             /* 10 L1 Context ID*/
                unsigned long SDBG : 1;                /* 11 IA32_DEBUG_INTERFACE MSR */
                unsigned long FMA : 1;                 /* 12 FMA */
                unsigned long CMPXCHG16B : 1;          /* 13 CMPXCHG16B Available */
                unsigned long xTPRUpdateControl : 1;   /* 14 xTPR Update Control */
                unsigned long PDCM : 1;                /* 15 Perfmon and Debug Capability */
                unsigned long ReservedBits0 : 1;       /* 16 Reserved */
                unsigned long PCID : 1;                /* 17 Process-context identifiers */
                unsigned long DCA : 1;                 /* 18 Direct Cache Access */
                unsigned long SSE4_1 : 1;              /* 19 SSE4.1*/
                unsigned long SSE4_2 : 1;              /* 20 SSE4.2*/
                unsigned long x2APIC : 1;              /* 21 x2APIC */
                unsigned long MOVBE : 1;               /* 22 MOVBE */
                unsigned long POPCNT : 1;              /* 23 POPCNT */
                unsigned long TSC_Deadline : 1;        /* 24 TSC deadline */
                unsigned long AESNI : 1;               /* 25 AESNI */
                unsigned long XSAVE : 1;               /* 26 XSAVE */
                unsigned long OSXSAVE : 1;             /* 27 CR4.OSXSAVE */
                unsigned long AVX : 1;                 /* 28 AVX */
                unsigned long F16C : 1;                /* 29 16-bit floating-point conversion */
                unsigned long RDRAND : 1;              /* 30 RDRAND */
                unsigned long NotUsedBits0 : 1;        /* 31 Always 0 */
            };
            /* Edx */
            struct {
                unsigned long FPU : 1;                 /* 00 x87 FPU */
                unsigned long VME : 1;                 /* 01 Virtual 8086 mode enhancements */
                unsigned long DE : 1;                  /* 02 Debugging Extensions */
                unsigned long PSE : 1;                 /* 03 Page Size Extension */
                unsigned long TSC : 1;                 /* 04 Time Stamp Counter */
                unsigned long MSR : 1;                 /* 05 Model Specific Registers RDMSR and WRMSR Instructions */
                unsigned long PAE : 1;                 /* 06 Physical Address Extension */
                unsigned long MCE : 1;                 /* 07 Machine Check Exception */
                unsigned long CX8 : 1;                 /* 08 CMPXCHG8B Instruction */
                unsigned long APIC : 1;                /* 09 APIC On-Chip */
                unsigned long ReservedBits1 : 1;       /* 10 Reserved */
                unsigned long SEP : 1;                 /* 11 SYSENTER and SYSEXIT Instructions */
                unsigned long MTRR : 1;                /* 12 Memory Type Range Registers */
                unsigned long PGE : 1;                 /* 13 Page Global Bit */
                unsigned long MCA : 1;                 /* 14 Machine Check Architecture */
                unsigned long CMOV : 1;                /* 15 Conditional Move Instructions */
                unsigned long PAT : 1;                 /* 16 Page Attribute Table */
                unsigned long PSE_36 : 1;              /* 17 36-Bit Page Size Extension */
                unsigned long PSN : 1;                 /* 18 Processor Serial Number */
                unsigned long CLFSH : 1;               /* 19 CLFLUSH Instruction */
                unsigned long ReservedBits2 : 1;       /* 20 Reserved */
                unsigned long DS : 1;                  /* 21 Debug Store */
                unsigned long ACPI : 1;                /* 22 Thermal Monitor and Software Controlled Clock Facilities */
                unsigned long MMX : 1;                 /* 23 Intel MMX Technology */
                unsigned long FXSR : 1;                /* 24 FXSAVE and FXRSTOR Instructions */
                unsigned long SSE : 1;                 /* 25 SSE */
                unsigned long SSE2 : 1;                /* 26 SSE2 */
                unsigned long SS : 1;                  /* 27 Self Snoop */
                unsigned long HTT : 1;                 /* 28 Max APIC IDs reserved field is Valid */
                unsigned long TM : 1;                  /* 29 Thermal Monitor */
                unsigned long ReservedBits3 : 1;       /* 30 Reserved */
                unsigned long PBE : 1;                 /* 31 Pending Break Enable */
            };
        } FeatureInfo;
    } F01_00;

    /* 07 Structured Extended Feature Flags Enumeration Leaf */
    struct {
        /* Eax */
        unsigned long MaxInputValue;
        /* Ebx, Ecx, Edx: Feature Flags */
        struct {
            /* Ebx */
            struct {
                unsigned long FSGSBASE : 1;             /* 00 FSGSBASE */
                unsigned long IA32_TSC_ADJUST : 1;      /* 01 IA32_TSC_ADJUST MSR */
                unsigned long SGX : 1;                  /* 02 Intel® Software Guard Extensions */
                unsigned long BMI1 : 1;                 /* 03 BMI1 */
                unsigned long HLE : 1;                  /* 04 HLE */
                unsigned long AVX2 : 1;                 /* 05 AVX2 */
                unsigned long FDP_EXCPTN_ONLY : 1;      /* 06 FDP_EXCPTN_ONLY */
                unsigned long SMEP : 1;                 /* 07 SMEP */
                unsigned long BMI2 : 1;                 /* 08 BMI2 */
                unsigned long ERMS : 1;                 /* 09 Enhanced Fast Strings REP MOVB/STOB */
                unsigned long INVPCID : 1;              /* 10 INVPCID */
                unsigned long RTM : 1;                  /* 11 RTM */
                unsigned long RDT_M : 1;                /* 12 Intel® Resource Director Technology (Intel® RDT) Monitoring */
                unsigned long DeprecatesFPUCSDS : 1;    /* 13 Deprecates FPU CS and FPU DS */
                unsigned long MPX : 1;                  /* 14 Intel® Memory Protection Extensions */
                unsigned long RDT_A : 1;                /* 15 Intel® Resource Director Technology (Intel® RDT) Allocation */
                unsigned long AVX512F : 1;              /* 16 AVX512F */
                unsigned long AVX512DQ : 1;             /* 17 AVX512DQ */
                unsigned long RDSEED : 1;               /* 18 RDSEED */
                unsigned long ADX : 1;                  /* 19 ADX */
                unsigned long SMAP : 1;                 /* 20 Supervisor-Mode Access Prevention */
                unsigned long AVX512_IFMA : 1;          /* 21 AVX512_IFMA */
                unsigned long ReservedBits0 : 1;        /* 22 Reserved */
                unsigned long CLFLUSHOPT : 1;           /* 23 CLFLUSHOPT */
                unsigned long CLWB : 1;                 /* 24 CLWB */
                unsigned long PT : 1;                   /* 25 Intel Processor Trace */
                unsigned long AVX512PF : 1;             /* 26 AVX512PF */
                unsigned long AVX512ER : 1;             /* 27 AVX512ER */
                unsigned long AVX512CD : 1;             /* 28 AVX512CD */
                unsigned long SHA : 1;                  /* 29 SHA */
                unsigned long AVX512BW : 1;             /* 30 AVX512BW */
                unsigned long AVX512VL : 1;             /* 31 AVX512VL */
            };
            /* Ecx */
            struct {
                unsigned long PREFETCHWT1 : 1;          /* 00 PREFETCHWT1 */
                unsigned long AVX512_VBMI : 1;          /* 01 AVX512_VBMI */
                unsigned long UMIP : 1;                 /* 02 User-mode instruction prevention */
                unsigned long PKU : 1;                  /* 03 Protection keys for user-mode pages */
                unsigned long OSPKE : 1;                /* 04 CR4.PKE */
                unsigned long WAITPKG : 1;              /* 05 WAITPKG */
                unsigned long AVX512_VBMI2 : 1;         /* 06 AVX512_VBMI2 */
                unsigned long CET_SS : 1;               /* 07 CET shadow stack */
                unsigned long GFNI : 1;                 /* 08 GFNI */
                unsigned long VAES : 1;                 /* 09 VAES */
                unsigned long VPCLMULQDQ : 1;           /* 10 VPCLMULQDQ */
                unsigned long AVX512_VNNI : 1;          /* 11 AVX512_VNNI */
                unsigned long AVX512_BITALG : 1;        /* 12 AVX512_BITALG */
                unsigned long TME_EN : 1;               /* 13 TME_EN */
                unsigned long AVX512_VPOPCNTDQ : 1;     /* 14 AVX512_VPOPCNTDQ */
                unsigned long ReservedBits1 : 1;        /* 15 Reserved */
                unsigned long LA57 : 1;                 /* 16 57-bit linear addresses and five-level paging */
                unsigned long MAWAU : 5;                /* 17-21 57-bit linear addresses and five-level paging */
                unsigned long RDPID : 1;                /* 22 RDPID and IA32_TSC_AUX */
                unsigned long KL : 1;                   /* 23 Key Locker */
                unsigned long ReservedBits2 : 1;        /* 24 Reserved */
                unsigned long CLDEMOTE : 1;             /* 25 Cache line demote */
                unsigned long ReservedBits3 : 1;        /* 26 Reserved */
                unsigned long MOVDIRI : 1;              /* 27 MOVDIRI */
                unsigned long MOVDIR64B : 1;            /* 28 MOVDIR64B */
                unsigned long ENQCMD : 1;               /* 29 Enqueue Stores */
                unsigned long SGX_LC : 1;               /* 30 SGX Launch Configuration */
                unsigned long PKS : 1;                  /* 31 Protection keys for supervisor-mode pages */
            };
            /* Edx */
            struct {
                unsigned long ReservedBits4 : 1;            /* 00 Reserved */
                unsigned long SGX_KEYS : 1;                 /* 01 Attestation Services for Intel® SGX */
                unsigned long AVX512_4VNNIW : 1;            /* 02 AVX512_4VNNIW */
                unsigned long AVX512_4FMAPS : 1;            /* 03 AVX512_4FMAPS */
                unsigned long FSRM : 1;                     /* 04 Fast Short REP MOV */
                unsigned long UINTR : 1;                    /* 05 User interrupts */
                unsigned long ReservedBits5 : 2;            /* 06-07 Reserved */
                unsigned long AVX512_VP2INTERSECT : 1;      /* 08 AVX512_VP2INTERSECT */
                unsigned long SRBDS_CTRL : 1;               /* 09 SRBDS_CTRL */
                unsigned long MD_CLEAR : 1;                 /* 10 MD_CLEAR */
                unsigned long RTM_ALWAYS_ABORT : 1;         /* 11 RTM_ALWAYS_ABORT */
                unsigned long ReservedBits6 : 1;            /* 12 Reserved */
                unsigned long RTM_FORCE_ABORT : 1;          /* 13 RTM_FORCE_ABORT */
                unsigned long SERIALIZE : 1;                /* 14 SERIALIZE */
                unsigned long Hybrid : 1;                   /* 15 Hybrid */
                unsigned long TSXLDTRK : 1;                 /* 16 Intel TSX suspend/resume of load address tracking */
                unsigned long ReservedBits7 : 1;            /* 17 Reserved */
                unsigned long PCONFIG : 1;                  /* 18 PCONFIG */
                unsigned long ArchitecturalLBRs : 1;        /* 19 Architectural LBRs */
                unsigned long CET_IBT : 1;                  /* 20 CET indirect branch tracking */
                unsigned long ReservedBits8 : 1;            /* 21 Reserved */
                unsigned long AMX_BF16 : 1;                 /* 22 Tile computational operations on bfloat16 numbers */
                unsigned long AVX512_FP16 : 1;              /* 23 AVX512_FP16 */
                unsigned long AMX_TILE : 1;                 /* 24 Tile architecture */
                unsigned long AMX_INT8 : 1;                 /* 25 Tile computational operations on 8-bit integers */
                unsigned long Enum_IBRS_IBPB : 1;           /* 26 Enumerates for IBRS and IBPB */
                unsigned long Enum_STIBP : 1;               /* 27 Enumerates for STIBP */
                unsigned long Enum_L1D_FLUSH : 1;           /* 28 Enumerates for L1D_FLUSH */
                unsigned long Enum_ARCH_CAPABILITIES : 1;   /* 29 Enumerates for the IA32_ARCH_CAPABILITIES MSR */
                unsigned long Enum_CORE_CAPABILITIES : 1;   /* 30 Enumerates for the IA32_CORE_CAPABILITIES MSR */
                unsigned long Enum_SSBD : 1;                /* 31 Enumerates for SSBD */
            };
        } FeatureFlags;
    } F07_00;

    union {
        /* 0 if the sub-leaf index, 1, is invalid */
        struct {
            unsigned long EaxValidity;
            unsigned long EbxValidity;
            unsigned long EcxValidity;
            unsigned long EdxValidity;
        };
        /* Eax, Ebx, Ecx, Edx: Feature Flags */
        struct {
            /* Eax */
            struct {
                unsigned long ReservedBits0 : 4;    /* 00-03 Reserved */
                unsigned long AVX_VNNI : 1;         /* 04 AVX-VNNI */
                unsigned long AVX512_BF16 : 1;      /* 05 AVX512_BF16 */
                unsigned long ReservedBits1 : 4;    /* 06-09 Reserved */
                unsigned long FZRM : 1;             /* 10 Fast zero-length REP MOVSB */
                unsigned long FSRS : 1;             /* 11 Fast short REP STOSB */
                unsigned long FSRCS : 1;            /* 12 Fast short REP CMPSB/SCASB */
                unsigned long ReservedBits2 : 9;    /* 13-21 Reserved */
                unsigned long HRESET : 1;           /* 22 HRESET */
                unsigned long ReservedBits3 : 9;    /* 23-31 Reserved */
            };
            /* Ebx */
            struct {
                unsigned long Enum_PPIN : 1;        /* 00 Enumerates the presence of the IA32_PPIN and IA32_PPIN_CTL MSRs */
                unsigned long ReservedBits4 : 31;   /* 01-31 Reserved */
            };
            /* Ecx */
            unsigned long ReservedBits5;
            /* Edx */
            struct {
                unsigned long ReservedBits6 : 18;   /* 00-17 Reserved */
                unsigned long CET_SSS : 1;          /* 18 CET_SSS */
                unsigned long ReservedBits7 : 13;   /* 19-31 Reserved */
            };
        };
    } F07_01;

} WIE_CPU_INFO, *PWIE_CPU_INFO;
