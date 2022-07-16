include HijackASM.inc

.code

Hijack_CallProc_InjectThread_x64 PROC USES rbx rdi rsi
    ; Shadow space for x64
    sub     rsp, 40

    ; rdi point to HIJACK_CALLPROCHEADER
    mov     rdi, rcx
    ; Support stdcall(CC_STDCALL) only
    cmp     dword ptr [rdi + 8], CC_STDCALL  ; [rdi].CallConvention
    je      @f
    mov     eax, STATUS_NOT_IMPLEMENTED
    ret
@@:

    ; rsi point to HIJACK_CALLPROCPARAM array, rbx point to random parameters
    lea     rsi, [rdi + sizeof HIJACK_CALLPROCHEADER]
    mov     r10d, [rdi + 32] ;[rdi].ParamCount
    mov     rax, sizeof HIJACK_CALLPROCPARAM
    mul     r10d
    lea     rbx, [rsi + rax]
    
    ; Enum HIJACK_CALLPROCPARAM
LN_ENUMPARAM:
    or      r10d, r10d
    je      LN_CALLPROC
    mov     rax, [rsi + 8] ; [rsi]._Size
    or      rax, rax
    je      LN02
    cmp     rax, -1
    je      LN02
    ; rdx = address to random parameter
    mov     r11, rbx
    ; Align size of random parameter to 4
    add     rax, 3
    and     rax, -4
    ; rbx point to the next random parameter
    add     rbx, rax
    xor     rax, rax
    jmp     LN03
LN02:
    mov     r11, [rsi] ; [rsi]._Address
LN03:
    ; Push parameter, rax = -1 if parameter is float
    cmp     r10d, 4
    jne     LN_P3
    or      rax, rax
    je      @f
    movq    xmm3, r11
    jmp     LN_PUSHPARAM
@@:
    mov     r9, r11
    jmp     LN_PUSHPARAM
LN_P3:
    cmp     r10d, 3
    jne     LN_P2
    or      rax, rax
    je      @f
    movq    xmm2, r11
    jmp     LN_PUSHPARAM
@@:
    mov     r8, r11
    jmp     LN_PUSHPARAM
LN_P2:
    cmp     r10d, 2
    jne     LN_P1
    or      rax, rax
    je      @f
    movq    xmm1, r11
    jmp     LN_PUSHPARAM
@@:
    mov     rdx, r11
    jmp     LN_PUSHPARAM
LN_P1:
    cmp     r10d, 1
    jne     LN_PX
    or      rax, rax
    je      @f
    movq    xmm0, r11
    jmp     LN_PUSHPARAM
@@:
    mov     rcx, r11
    jmp     LN_PUSHPARAM
LN_PX:
    push    r11
LN_PUSHPARAM:
    add     rsi, sizeof HIJACK_CALLPROCPARAM
    dec     r10d
    jmp     LN_ENUMPARAM

LN_CALLPROC:
    ; Clear LastError, LastStatus and ExceptionCode
    xor     eax, eax
    mov     gs:[68h], eax
    mov     gs:[1248h], eax
    mov     gs:[2C0h], eax
    ; Call procedure
    call    qword ptr [rdi] ; [rdi].Procedure
    ; Write RetValue, LastError, LastStatus and ExceptionCode
    mov     [rdi + 12], rax ; [rdi].RetValue
    mov     eax, gs:[68h]
    mov     [rdi + 20], eax ; [rdi].LastError
    mov     eax, gs:[1248h]
    mov     [rdi + 24], eax ; [rdi].LastStatus
    mov     eax, gs:[2C0h]
    mov     [rdi + 28], eax ; [rdi].ExceptionCode
    ; Return
    xor     eax, eax
    add     rsp, 40
    ret
Hijack_CallProc_InjectThread_x64 ENDP

END