#include "include\NTAssassin\NTAssassin.h"

PLDR_DATA_TABLE_ENTRY NTAPI Proc_EnumDlls(PROC_DLLENUMPROC DllEnumProc, LPARAM Param) {
    PLDR_DATA_TABLE_ENTRY pHead, pNode;
    pHead = CONTAINING_RECORD(NT_GetPEB()->Ldr->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);
    pNode = pHead;
    while (DllEnumProc(pNode, Param)) {
        pNode = CONTAINING_RECORD(pNode->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);
        if (pNode == pHead)
            return NULL;
    }
    return pNode;
}

BOOL CALLBACK Proc_GetDllByName_EnumDllProc(PLDR_DATA_TABLE_ENTRY lpstDll, LPARAM lParam) {
    PUNICODE_STRING lpstDllName = (PUNICODE_STRING)lParam;
    return !(
        lpstDll->DllBase &&
        lpstDll->BaseDllName.Length == lpstDllName->Length &&
        Str_IEqualW(lpstDll->BaseDllName.Buffer, lpstDllName->Buffer)
        );
}

PLDR_DATA_TABLE_ENTRY NTAPI Proc_GetDllByName(PWSTR DllName) {
    UNICODE_STRING stDllName;
    stDllName.Buffer = DllName;
    stDllName.Length = (USHORT)Str_SizeW(DllName);
    return Proc_EnumDlls(Proc_GetDllByName_EnumDllProc, (LPARAM)&stDllName);
}

BOOL CALLBACK Proc_GetDllByHandle_EnumDllProc(PLDR_DATA_TABLE_ENTRY lpstDll, LPARAM lParam) {
    return lpstDll->DllBase != (HMODULE)lParam;
}

PLDR_DATA_TABLE_ENTRY NTAPI Proc_GetDllByHandle(HMODULE DllHandle) {
    return Proc_EnumDlls(Proc_GetDllByHandle_EnumDllProc, (LPARAM)DllHandle);
}

HMODULE NTAPI Proc_GetDllHandleByName(PWSTR DllName) {
    PLDR_DATA_TABLE_ENTRY lpstDll = Proc_GetDllByName(DllName);
    return lpstDll ? lpstDll->DllBase : NULL;
}

BOOL CALLBACK Proc_GetDllByAddr_EnumDllProc(PLDR_DATA_TABLE_ENTRY DllLdrEntry, LPARAM Param) {
    UINT_PTR Address = Param;
    return !(
        DllLdrEntry->DllBase &&
        Address >= (UINT_PTR)DllLdrEntry->DllBase &&
        Address <= (UINT_PTR)DllLdrEntry->DllBase + DllLdrEntry->SizeOfImage
        );
}

PLDR_DATA_TABLE_ENTRY NTAPI Proc_GetDllByAddr(PVOID Address) {
    return Proc_EnumDlls(Proc_GetDllByAddr_EnumDllProc, (LPARAM)Address);
}

HMODULE NTAPI Proc_LoadDll(PCWSTR LibName, BOOL DontResolveRef) {
    HMODULE         hDll;
    ULONG           DllCharacteristics;
    UNICODE_STRING  stLibName;
    Str_InitW(&stLibName, (PWSTR)LibName);
    return NT_SUCCESS(LdrLoadDll(NULL, DontResolveRef ? (DllCharacteristics = IMAGE_FILE_EXECUTABLE_IMAGE, &DllCharacteristics) : NULL, &stLibName, &hDll)) ? hDll : NULL;
}

PVOID NTAPI Proc_GetProcAddr(HMODULE Module, PCSTR ProcName) {
    PANSI_STRING    lpstProcName;
    ULONG           ulProcOrd;
    PVOID           lpProc;
    if ((UINT_PTR)ProcName > MAXWORD) {
        ANSI_STRING stProcName;
        stProcName.Length = (USHORT)Str_LenA(ProcName);
        stProcName.MaximumLength = stProcName.Length + sizeof(CHAR);
        stProcName.Buffer = (PCHAR)ProcName;
        lpstProcName = &stProcName;
        ulProcOrd = 0;
    } else {
        lpstProcName = NULL;
        ulProcOrd = (ULONG)(ULONG_PTR)ProcName;
    }
    return NT_SUCCESS(LdrGetProcedureAddress(Module, lpstProcName, ulProcOrd, &lpProc)) ? lpProc : NULL;
}

PVOID NTAPI Proc_LoadProcAddr(PCWSTR LibName, PCSTR ProcName) {
    HMODULE hDll = Proc_LoadDll(LibName, FALSE);
    return hDll ? Proc_GetProcAddr(hDll, ProcName) : NULL;
}

NTSTATUS NTAPI Proc_WaitForObject(HANDLE Object, DWORD Milliseconds) {
    LARGE_INTEGER   stLI;
    return NtWaitForSingleObject(
        Object,
        FALSE,
        Milliseconds == INFINITE ? NULL : (stLI.QuadPart = Milliseconds * -10000LL, &stLI)
    );
}

NTSTATUS NTAPI Proc_DelayExec(DWORD Milliseconds) {
    LARGE_INTEGER   stLI;
    stLI.QuadPart = Milliseconds == INFINITE ? MININT64 : Milliseconds * -10000LL;
    return NtDelayExecution(FALSE, &stLI);
}

NTSTATUS NTAPI Proc_GetThreadExitCode(HANDLE ThreadHandle, PDWORD ExitCode) {
    THREAD_BASIC_INFORMATION    stThreadInfo;
    NTSTATUS                    lStatus;
    lStatus = NtQueryInformationThread(ThreadHandle, ThreadBasicInformation, &stThreadInfo, sizeof(stThreadInfo), NULL);
    if (NT_SUCCESS(lStatus))
        *ExitCode = stThreadInfo.ExitStatus;
    return lStatus;
}