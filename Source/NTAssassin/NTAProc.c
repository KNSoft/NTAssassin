#include "include\NTAssassin\NTAssassin.h"

PLDR_DATA_TABLE_ENTRY NTAPI Proc_EnumDlls(_In_ PROC_DLLENUMPROC DllEnumProc, LPARAM Param) {
    PLDR_DATA_TABLE_ENTRY pHead, pNode;
    pHead = CONTAINING_RECORD(NT_GetPEB()->Ldr->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);
    pNode = pHead;
    while (DllEnumProc(pNode, Param)) {
        pNode = CONTAINING_RECORD(pNode->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);
        if (pNode == pHead) {
            return NULL;
        }
    }
    return pNode;
}

BOOL CALLBACK Proc_GetDllByName_EnumDllProc(PLDR_DATA_TABLE_ENTRY pstDll, LPARAM lParam) {
    PUNICODE_STRING pstDllName = (PUNICODE_STRING)lParam;
    return !(
        pstDll->DllBase &&
        pstDll->BaseDllName.Length == pstDllName->Length &&
        Str_IEqualW(pstDll->BaseDllName.Buffer, pstDllName->Buffer)
        );
}

PLDR_DATA_TABLE_ENTRY NTAPI Proc_GetDllByName(_In_z_ PCWSTR DllName) {
    UNICODE_STRING stDllName;
    stDllName.Buffer = (PWSTR)DllName;
    stDllName.Length = (USHORT)Str_SizeW(DllName);
    return Proc_EnumDlls(Proc_GetDllByName_EnumDllProc, (LPARAM)&stDllName);
}

BOOL CALLBACK Proc_GetDllByHandle_EnumDllProc(PLDR_DATA_TABLE_ENTRY pstDll, LPARAM lParam) {
    return pstDll->DllBase != (HMODULE)lParam;
}

PLDR_DATA_TABLE_ENTRY NTAPI Proc_GetDllByHandle(_In_ HMODULE DllHandle) {
    return Proc_EnumDlls(Proc_GetDllByHandle_EnumDllProc, (LPARAM)DllHandle);
}

BOOL CALLBACK Proc_GetDllByAddr_EnumDllProc(PLDR_DATA_TABLE_ENTRY DllLdrEntry, LPARAM Param) {
    UINT_PTR Address = Param;
    return !(
        DllLdrEntry->DllBase &&
        Address >= (UINT_PTR)DllLdrEntry->DllBase &&
        Address <= (UINT_PTR)DllLdrEntry->DllBase + DllLdrEntry->SizeOfImage
        );
}

PLDR_DATA_TABLE_ENTRY NTAPI Proc_GetDllByAddr(_In_ PVOID Address) {
    return Proc_EnumDlls(Proc_GetDllByAddr_EnumDllProc, (LPARAM)Address);
}

HMODULE NTAPI Proc_GetDllHandleByName(_In_z_ PCWSTR DllName) {
    PLDR_DATA_TABLE_ENTRY pstDll = Proc_GetDllByName(DllName);
    return pstDll ? pstDll->DllBase : NULL;
}

HMODULE NTAPI Proc_LoadDll(_In_z_ PCWSTR LibName, BOOL DontResolveRef) {
    HMODULE         hDll;
    ULONG           DllCharacteristics;
    UNICODE_STRING  stLibName;
    Str_InitW(&stLibName, (PWSTR)LibName);
    return NT_SUCCESS(LdrLoadDll(NULL, DontResolveRef ? (DllCharacteristics = IMAGE_FILE_EXECUTABLE_IMAGE, &DllCharacteristics) : NULL, &stLibName, &hDll)) ? hDll : NULL;
}

PVOID NTAPI Proc_GetProcAddr(_In_ HMODULE Module, _In_z_ PCSTR ProcName) {
    PANSI_STRING    pstProcName;
    ULONG           ulProcOrd;
    PVOID           pProc;
    if ((UINT_PTR)ProcName > MAXWORD) {
        ANSI_STRING stProcName;
        stProcName.Length = (USHORT)Str_LenA(ProcName);
        stProcName.MaximumLength = stProcName.Length + sizeof(CHAR);
        stProcName.Buffer = (PCHAR)ProcName;
        pstProcName = &stProcName;
        ulProcOrd = 0;
    } else {
        pstProcName = NULL;
        ulProcOrd = (ULONG)(ULONG_PTR)ProcName;
    }
    return NT_SUCCESS(LdrGetProcedureAddress(Module, pstProcName, ulProcOrd, &pProc)) ? pProc : NULL;
}

PVOID NTAPI Proc_LoadProcAddr(_In_z_ PCWSTR LibName, _In_z_ PCSTR ProcName) {
    HMODULE hDll = Proc_LoadDll(LibName, FALSE);
    return hDll ? Proc_GetProcAddr(hDll, ProcName) : NULL;
}

BOOL NTAPI Proc_WaitForObject(HANDLE Object, DWORD Milliseconds) {
    LARGE_INTEGER stLI;
    NTSTATUS lStatus = NtWaitForSingleObject(Object, FALSE, Milliseconds == INFINITE ? NULL : (stLI.QuadPart = Milliseconds * -10000LL, &stLI));
    if (NT_SUCCESS(lStatus)) {
        return TRUE;
    } else {
        NT_SetLastStatus(lStatus);
        return FALSE;
    }
}

VOID NTAPI Proc_DelayExec(DWORD Milliseconds) {
    LARGE_INTEGER stLI;
    stLI.QuadPart = Milliseconds == INFINITE ? MININT64 : Milliseconds * -10000LL;
    NtDelayExecution(FALSE, &stLI);
}

_Success_(return != FALSE)
BOOL NTAPI Proc_GetThreadExitCode(HANDLE ThreadHandle, _Out_ PDWORD ExitCode) {
    THREAD_BASIC_INFORMATION    stThreadInfo;
    NTSTATUS                    lStatus;
    lStatus = NtQueryInformationThread(ThreadHandle, ThreadBasicInformation, &stThreadInfo, sizeof(stThreadInfo), NULL);
    if (NT_SUCCESS(lStatus)) {
        *ExitCode = stThreadInfo.ExitStatus;
        return TRUE;
    } else {
        NT_SetLastStatus(lStatus);
        return FALSE;
    }
}
