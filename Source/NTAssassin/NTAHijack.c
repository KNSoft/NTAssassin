#include "include\NTAssassin\NTAssassin.h"

PVOID NTAPI Hijack_LoadProcAddr_InitParamEx(PVOID Buffer, SIZE_T BufferSize, PCWSTR LibName, PCSTR ProcName, PVOID** ProcAddrPointer) {
    SIZE_T  uSize, iCcb, uDelta;
    WORD    wProcOrdinal;
    PVOID   lpStruct, lpTemp;
    PDWORD  lpdwSize;

    lpStruct = (PVOID)BYTE_ALIGN((ULONG_PTR)Buffer, STRING_ALIGNMENT);
    lpTemp = lpStruct;
    uSize = BufferSize;

    // uSize
    uDelta = sizeof(UINT);
    if (uSize < uDelta)
        return NULL;
    lpdwSize = lpTemp;
    lpTemp = MOVE_PTR(lpTemp, uDelta, VOID);
    uSize -= uDelta;

    // szLibName
    iCcb = Str_CopyExW((PWSTR)lpTemp, uSize >> 1, LibName) << 1;
    uDelta = iCcb + sizeof(WCHAR);
    if (uSize < uDelta)
        return NULL;
    lpTemp = MOVE_PTR(lpTemp, uDelta, VOID);
    uSize -= uDelta;

    // wProcOrdinal
    uDelta = sizeof(WORD);
    if (uSize < uDelta)
        return NULL;
    wProcOrdinal = (UINT_PTR)ProcName <= MAXWORD ? LOWORD(ProcName) : 0;
    *(LPWORD)lpTemp = wProcOrdinal;
    lpTemp = MOVE_PTR(lpTemp, uDelta, VOID);
    uSize -= uDelta;

    // szProcName
    if (!wProcOrdinal) {
        lpTemp = (PVOID)BYTE_ALIGN((ULONG_PTR)lpTemp, STRING_ALIGNMENT);
        if (ProcName) {
            // Fixme: ProcName won't be NULL here
        #pragma warning(disable: 6387)
            iCcb = Str_CopyExA((LPSTR)lpTemp, uSize, ProcName);
        #pragma warning(default: 6387)
        } else {
            ((LPSTR)lpTemp)[0] = '\0';
            iCcb = 0;
        }
        uDelta = iCcb + sizeof(CHAR);
        if (uSize < uDelta)
            return NULL;
        lpTemp = MOVE_PTR(lpTemp, uDelta, VOID);
        uSize -= uDelta;
    }

    // lpProc
    if (ProcName) {
        uDelta = sizeof(LPVOID);
        if (uSize < uDelta)
            return NULL;
        *(LPVOID*)lpTemp = NULL;
        *ProcAddrPointer = (LPVOID*)lpTemp;
        uSize -= uDelta;
    }

    // Done
    *lpdwSize = (DWORD)(BufferSize - uSize);
    return lpStruct;
}

NTSTATUS NTAPI Hijack_CreateThread(HANDLE ProcessHandle, PHIJACK_CREATETHREAD HijackThread, DWORD Timeout) {
    NTSTATUS    lStatus;
    RPROC_MAP   ProcMap, ParamMap;
    PVOID       lpParam;
    HANDLE      hThread;
    BOOL        b32Proc;
    PVOID       pProc;
    SIZE_T      usProcSize;

    // Map code and parameter to remote
    if (!IsWow64Process(ProcessHandle, &b32Proc))
        return STATUS_ACCESS_DENIED;
    if (b32Proc) {
        pProc = HijackThread->Proc32;
        usProcSize = HijackThread->ProcSize32;
    } else {
        pProc = HijackThread->Proc64;
        usProcSize = HijackThread->ProcSize64;
    }
    RProc_InitMap(&ProcMap, pProc, usProcSize, PAGE_EXECUTE_READ);
    lStatus = RProc_MemMap(ProcessHandle, &ProcMap);
    if (!NT_SUCCESS(lStatus))
        goto Label_0;
    if (HijackThread->Param) {
        RProc_InitMap(&ParamMap, HijackThread->Param, HijackThread->ParamSize, PAGE_READWRITE);
        lStatus = RProc_MemMap(ProcessHandle, &ParamMap);
        if (!NT_SUCCESS(lStatus))
            goto Label_1;
        lpParam = ParamMap.Remote;
    } else
        lpParam = NULL;

    // Create remote thread and wait
    lStatus = RProc_CreateThread(ProcessHandle, ProcMap.Remote, lpParam, FALSE, &hThread);
    if (!NT_SUCCESS(lStatus))
        goto Label_2;
    lStatus = Proc_WaitForObject(hThread, Timeout);
    if (!NT_SUCCESS(lStatus))
        goto Label_3;

    // Receive return values and parameters
    lStatus = Proc_GetThreadExitCode(hThread, &HijackThread->ExitCode);
    if (!NT_SUCCESS(lStatus))
        goto Label_3;
    if (lpParam)
        lStatus = RProc_MemReadEx(ProcessHandle, lpParam, HijackThread->Param, HijackThread->ParamSize);

    // Cleanup and exit
Label_3:
    NtClose(hThread);
Label_2:
    if (lpParam)
        RProc_MemUnmap(ProcessHandle, &ParamMap);
Label_1:
    RProc_MemUnmap(ProcessHandle, &ProcMap);
Label_0:
    return lStatus;
}

NTSTATUS NTAPI Hijack_LoadProcAddr(HANDLE ProcessHandle, PWSTR LibName, PSTR ProcName, PVOID* ProcAddr, DWORD Timeout) {
    NTSTATUS                lStatus;
    BYTE                    Buffer[HIJACK_LOADPROCADDR_MAXPARAMBUFFERSIZE];
    LPVOID                  lpParam, *lpProc;
    HIJACK_CREATETHREAD     stThread;

    // Initialize parameter
    lpParam = Hijack_LoadProcAddr_InitParam(Buffer, LibName, ProcName, &lpProc);
    if (!lpParam)
        return STATUS_INVALID_PARAMETER;

    // Create remote thread
    stThread.Proc32 = (LPTHREAD_START_ROUTINE)SYM_Hijack_LoadProcAddr_InjectThread_x86;
    stThread.ProcSize32 = sizeof(SYM_Hijack_LoadProcAddr_InjectThread_x86);
    stThread.Proc64 = (LPTHREAD_START_ROUTINE)SYM_Hijack_LoadProcAddr_InjectThread_x64;
    stThread.ProcSize64 = sizeof(SYM_Hijack_LoadProcAddr_InjectThread_x64);
    stThread.Param = lpParam;
    stThread.ParamSize = *(PUINT)lpParam;
    lStatus = Hijack_CreateThread(ProcessHandle, &stThread, Timeout);
    if (NT_SUCCESS(lStatus)) {
        lStatus = stThread.ExitCode;
        if (NT_SUCCESS(lStatus) && ProcAddr)
            *ProcAddr = *lpProc;
    }
    return lStatus;
}

NTSTATUS NTAPI Hijack_CallProc(HANDLE ProcessHandle, PHIJACK_CALLPROCHEADER HijackCallProc, PHIJACK_CALLPROCPARAM HijackParams, DWORD Timeout) {
    NTSTATUS                lStatus;
    SIZE_T                  usTotalSize, usPageSize;
    UINT                    i, uParamCount;
    HIJACK_CALLPROCHEADER   stCallProcRet;
    HIJACK_CALLPROCPARAM*   pParam, *pRemoteParam, stMemParam;
    PVOID                   pRemoteBuffer, pRemoteRandomParam, pTemp;
    BOOL                    b32Proc;
    LPTHREAD_START_ROUTINE  pProc;
    SIZE_T                  usProcSize;
    RPROC_MAP               ProcMap;
    HANDLE                  hThread;

    // Calculate size of remote buffer and allocate memory
    uParamCount = HijackCallProc->ParamCount;
    usTotalSize = sizeof(HIJACK_CALLPROCHEADER) + sizeof(HIJACK_CALLPROCPARAM) * uParamCount;
    pParam = HijackParams;
    for (i = 0; i < uParamCount; i++)
        usTotalSize += BYTE_ALIGN(pParam[i].Size, STRING_ALIGNMENT);
    pRemoteBuffer = NULL;
    usPageSize = usTotalSize;
    lStatus = NtAllocateVirtualMemory(ProcessHandle, &pRemoteBuffer, 0, &usPageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!NT_SUCCESS(lStatus))
        goto Label_0;

    // Write buffer:
    // HIJACK_CALLPROCHEADER
    // MEMORY_RANGE_ENTRY[HIJACK_CALLPROCHEADER.ParamCount]
    // Random Parameters..., align to STRING_ALIGNMENT
    lStatus = NtWriteVirtualMemory(ProcessHandle, pRemoteBuffer, HijackCallProc, sizeof(HIJACK_CALLPROCHEADER), NULL);
    if (!NT_SUCCESS(lStatus))
        goto Label_1;
    pRemoteParam = MOVE_PTR(pRemoteBuffer, sizeof(HIJACK_CALLPROCHEADER), HIJACK_CALLPROCPARAM);
    pTemp = pRemoteRandomParam = MOVE_PTR(pRemoteParam, sizeof(HIJACK_CALLPROCPARAM) * uParamCount, VOID);
    for (i = uParamCount; i > 0; i--) {
        stMemParam.Size = pParam[i - 1].Size;
        if (stMemParam.Size) {
            lStatus = NtWriteVirtualMemory(ProcessHandle, pTemp, (PVOID)pParam[i - 1].Address, (SIZE_T)stMemParam.Size, NULL);
            if (!NT_SUCCESS(lStatus))
                goto Label_1;
            stMemParam.Address = (DWORD_PTR)pTemp;
            pTemp = MOVE_PTR(pTemp, BYTE_ALIGN(stMemParam.Size, STRING_ALIGNMENT), VOID);
        } else
            stMemParam.Address = pParam[i - 1].Address;
        lStatus = NtWriteVirtualMemory(ProcessHandle, pRemoteParam, &stMemParam, sizeof(stMemParam), NULL);
        if (!NT_SUCCESS(lStatus))
            goto Label_1;
        pRemoteParam++;
    }

    // Create remote thread
    if (!IsWow64Process(ProcessHandle, &b32Proc))
        return STATUS_ACCESS_DENIED;
    if (b32Proc) {
        pProc = (LPTHREAD_START_ROUTINE)SYM_Hijack_CallProc_InjectThread_x86;
        usProcSize = sizeof(SYM_Hijack_CallProc_InjectThread_x86);
    } else {
        pProc = (LPTHREAD_START_ROUTINE)SYM_Hijack_CallProc_InjectThread_x64;
        usProcSize = sizeof(SYM_Hijack_CallProc_InjectThread_x64);
    }
    RProc_InitMap(&ProcMap, pProc, usProcSize, PAGE_EXECUTE_READ);
    lStatus = RProc_MemMap(ProcessHandle, &ProcMap);
    if (!NT_SUCCESS(lStatus))
        goto Label_1;
    lStatus = RProc_CreateThread(ProcessHandle, ProcMap.Remote, pRemoteBuffer, FALSE, &hThread);
    if (!NT_SUCCESS(lStatus))
        goto Label_2;
    lStatus = Proc_WaitForObject(hThread, Timeout);
    if (!NT_SUCCESS(lStatus))
        goto Label_3;

    // Get return values and parameters
    lStatus = NtReadVirtualMemory(ProcessHandle, pRemoteBuffer, &stCallProcRet, sizeof(stCallProcRet), NULL);
    if (!NT_SUCCESS(lStatus))
        goto Label_3;
    HijackCallProc->RetValue = stCallProcRet.RetValue;
    HijackCallProc->LastError = stCallProcRet.LastError;
    HijackCallProc->LastStatus = stCallProcRet.LastStatus;
    HijackCallProc->ExceptionCode = stCallProcRet.ExceptionCode;
    for (i = uParamCount; i > 0; i--) {
        if (pParam[i - 1].Size) {
            if (pParam[i - 1].Out) {
                lStatus = NtReadVirtualMemory(ProcessHandle, pRemoteRandomParam, (PVOID)pParam[i - 1].Address, (SIZE_T)pParam[i - 1].Size, NULL);
                if (!NT_SUCCESS(lStatus))
                    goto Label_3;
            }
            pRemoteRandomParam = MOVE_PTR(pRemoteRandomParam, BYTE_ALIGN(pParam[i - 1].Size, STRING_ALIGNMENT), VOID);
        }
    }

Label_3:
    NtClose(hThread);
Label_2:
    RProc_MemUnmap(ProcessHandle, &ProcMap);
Label_1:
    NtFreeVirtualMemory(ProcessHandle, &pRemoteBuffer, &usPageSize, MEM_RELEASE);
Label_0:
    return lStatus;
}