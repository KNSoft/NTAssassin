.686P
.XMM
.model flat, stdcall

include HijackASM.inc

.code
assume fs:nothing

; DWORD WINAPI Hijack_CallProc_InjectThread_x86(LPVOID lParam)
Hijack_CallProc_InjectThread_x86 PROC USES ebx edi esi lParam
    ; edi point to HIJACK_CALLPROCHEADER
    xor eax, eax
    mov     edi, lParam
    assume  edi:ptr HIJACK_CALLPROCHEADER
    ; Support stdcall(CC_STDCALL) only
    .if [edi].CallConvention != CC_STDCALL
        mov     eax, STATUS_NOT_IMPLEMENTED
        ret
    .endif

    ; esi point to HIJACK_CALLPROCPARAM array, ebx point to random parameters
    lea     esi, [edi + sizeof HIJACK_CALLPROCHEADER]
    assume  esi:ptr HIJACK_CALLPROCPARAM
    mov     ecx, [edi].ParamCount
    mov     eax, sizeof HIJACK_CALLPROCPARAM
    mul     ecx
    lea     ebx, [esi + eax]
    
    ; Enum HIJACK_CALLPROCPARAM
@@:
    mov     eax, [esi]._Size
    .if eax && eax != -1
        ; edx = address to random parameter
        mov     edx, ebx
        ; Align size of random parameter to 4
        add     eax, 3
        and     eax, -4
        ; ebx point to the next random parameter
        add     ebx, eax
    .else
        mov     edx, [esi]._Address
    .endif
    ; Push parameter
    push    edx
    add     esi, sizeof HIJACK_CALLPROCPARAM
    loop    @b

    ; Clear LastError, LastStatus and ExceptionCode
    xor     eax, eax
    mov     fs:[34h], eax
    mov     fs:[0BF4h], eax
    mov     fs:[1A4h], eax
    ; Call procedure
    call    [edi].Procedure
    ; Write RetValue, LastError, LastStatus and ExceptionCode
    mov     [edi].RetValue, eax
    mov     eax, fs:[34h]
    mov     [edi].LastError, eax
    mov     eax, fs:[0BF4h]
    mov     [edi].LastStatus, eax
    mov     eax, fs:[1A4h]
    mov     [edi].ExceptionCode, eax
    assume  edi:nothing, esi:nothing
    ; Return
    xor     eax, eax
    ret
Hijack_CallProc_InjectThread_x86 ENDP

END