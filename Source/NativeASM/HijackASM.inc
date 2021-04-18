INCLUDELIB OLDNAMES
IFDEF _DEBUG
    IFDEF _DLL
        INCLUDELIB msvcrtd.lib
        INCLUDELIB vcruntimed.lib
        INCLUDELIB ucrtd.lib
    ELSE
        INCLUDELIB libcmtd.lib
        INCLUDELIB libvcruntimed.lib
        INCLUDELIB libucrtd.lib
    ENDIF
ELSE
    IFDEF _DLL
        INCLUDELIB msvcrt.lib
        INCLUDELIB vcruntime.lib
        INCLUDELIB ucrt.lib
    ELSE
        INCLUDELIB libcmt.lib
        INCLUDELIB libvcruntime.lib
        INCLUDELIB libucrt.lib
    ENDIF
ENDIF

HCPC_X86_STDCALL equ 0
HCPC_X86_FASTCALL equ 0x1
HCPC_X86_CDECL equ 0x2
HCPC_X86_VECTORCALL equ 0x4
HCPC_X64_MS equ 0
HCPC_X64_GCC equ 0x100

STATUS_NOT_IMPLEMENTED equ 0C0000002h

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