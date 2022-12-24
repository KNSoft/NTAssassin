STATUS_NOT_IMPLEMENTED equ 0C0000002h

CC_FASTCALL equ 0
CC_CDECL equ 1
CC_MSCPASCAL equ 2
CC_PASCAL equ 2
CC_MACPASCAL equ 3
CC_STDCALL equ 4
CC_FPFASTCALL equ 5
CC_SYSCALL equ 6
CC_MPWCDECL equ 7
CC_MPWPASCAL equ 8
CC_MAX equ 9

HIJACK_CALLPROCHEADER STRUCT
    Procedure       DWORD ?
    Padding0        DWORD ?
    CallConvention  DWORD ?
    RetValue        DWORD ? 
    Padding1        DWORD ? 
    LastError       DWORD ?
    LastStatus      DWORD ?
    ExceptionCode   DWORD ?
    ParamCount      DWORD ?
HIJACK_CALLPROCHEADER ENDS

HIJACK_CALLPROCPARAM STRUCT
    _Address  DWORD  ?
    Padding0  DWORD  ?
    _Size     DWORD  ?
    Padding1  DWORD  ?
    _Out      DWORD  ?
HIJACK_CALLPROCPARAM ENDS