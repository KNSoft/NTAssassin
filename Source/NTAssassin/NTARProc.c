#include "NTAssassin.h"

HANDLE NTAPI RProc_Open(DWORD DesiredAccess, DWORD ProcessId) {
    HANDLE              hProc;
    OBJECT_ATTRIBUTES   stObjectAttr;
    CLIENT_ID           stClientId;
    NT_InitObject(&stObjectAttr, NULL, NULL, 0);
    stClientId.UniqueProcess = ProcessId;
    stClientId.UniqueThread = 0;
    return NT_SUCCESS(NtOpenProcess(&hProc, DesiredAccess, &stObjectAttr, &stClientId)) ? hProc : NULL;
}

HANDLE NTAPI RProc_OpenThread(DWORD DesiredAccess, DWORD ThreadId) {
    HANDLE              hThread;
    OBJECT_ATTRIBUTES   stObjectAttr;
    CLIENT_ID           stClientId;
    NT_InitObject(&stObjectAttr, NULL, NULL, 0);
    stClientId.UniqueProcess = 0;
    stClientId.UniqueThread = ThreadId;
    return NT_SUCCESS(NtOpenThread(&hThread, DesiredAccess, &stObjectAttr, &stClientId)) ? hThread : NULL;
}

NTSTATUS NTAPI RProc_AdjustPrivilege(HANDLE ProcessHandle, PROC_LM_SE_NAMES Privilege, BOOL EnableState) {
    HANDLE              hToken;
    TOKEN_PRIVILEGES    stTokenPrivilege;
    NTSTATUS            lStatus;
    lStatus = NtOpenProcessToken(ProcessHandle, TOKEN_ADJUST_PRIVILEGES, &hToken);
    if (NT_SUCCESS(lStatus)) {
        stTokenPrivilege.PrivilegeCount = 1;
        stTokenPrivilege.Privileges[0].Luid.HighPart = 0;
        stTokenPrivilege.Privileges[0].Luid.LowPart = Privilege;
        stTokenPrivilege.Privileges[0].Attributes = EnableState ? SE_PRIVILEGE_ENABLED : SE_PRIVILEGE_REMOVED;
        lStatus = NtAdjustPrivilegesToken(hToken, FALSE, &stTokenPrivilege, sizeof(stTokenPrivilege), NULL, NULL);
    }
    return lStatus;
}

UINT NTAPI RProc_GetFullImageNameEx(HANDLE ProcessHandle, PWSTR FilePath, UINT FilePathCch) {
    if (NT_GetKUSD()->NtMajorVersion >= 6) {
        BYTE    stString[sizeof(UNICODE_STRING) + MAX_PATH * sizeof(WCHAR)];
        if (NT_SUCCESS(NtQueryInformationProcess(ProcessHandle, ProcessImageFileNameWin32, &stString, sizeof(stString), NULL)) &&
            ((PUNICODE_STRING)stString)->Length < FilePathCch)
            return Str_CchCopyExW(FilePath, FilePathCch, ((PUNICODE_STRING)stString)->Buffer);
    } else {
        PROCESS_BASIC_INFORMATION   stProcInfo;
        PVOID                       Address;
        USHORT                      uLength;
        stProcInfo.PebBaseAddress = NULL;
        if (NT_SUCCESS(NtQueryInformationProcess(ProcessHandle, ProcessBasicInformation, &stProcInfo, sizeof(stProcInfo), NULL)) &&
            stProcInfo.PebBaseAddress &&
            NT_SUCCESS(RProc_MemRead(ProcessHandle, &stProcInfo.PebBaseAddress->ProcessParameters, &Address)) &&
            NT_SUCCESS(RProc_MemRead(ProcessHandle, &((PRTL_USER_PROCESS_PARAMETERS)Address)->ImagePathName.Length, &uLength)) &&
            uLength < FilePathCch &&
            NT_SUCCESS(RProc_MemRead(ProcessHandle, &((PRTL_USER_PROCESS_PARAMETERS)Address)->ImagePathName.Buffer, &Address)) &&
            NT_SUCCESS(RProc_MemReadEx(ProcessHandle, Address, FilePath, uLength * sizeof(WCHAR)))
            ) {
            *(FilePath + uLength) = '\0';
            return uLength;
        }
    }
    return 0;
}

typedef struct _RPROC_TRANSLATEADDRESS {
    PVOID   Address;
    LPWSTR  String;
    UINT    Cch;
    UINT    CchOutput;
} RPROC_TRANSLATEADDRESS, * PRPROC_TRANSLATEADDRESS;

BOOL CALLBACK RProc_TranslateAddress_EnumloadedModulesCallback(PCWSTR ModuleName, DWORD64 ModuleBase, ULONG ModuleSize, PVOID UserContext) {
    PRPROC_TRANSLATEADDRESS lpst = (PRPROC_TRANSLATEADDRESS)UserContext;
    if ((DWORD_PTR)lpst->Address >= ModuleBase && (DWORD_PTR)lpst->Address < ModuleBase + ModuleSize) {
        PCWSTR  lpStr = ModuleName;
        while (*lpStr++ != '\0');
        while (*--lpStr != '\\' && lpStr >= ModuleName);
        lpst->CchOutput = wnsprintfW(lpst->String, lpst->Cch, L"%s!%p", lpStr + 1, lpst->Address);
        return FALSE;
    } else
        return TRUE;
}

UINT NTAPI RProc_TranslateAddressEx(HANDLE ProcessHandle, PVOID Address, PWSTR OutputString, UINT OutputStringCch) {
    RPROC_TRANSLATEADDRESS  st = { Address, OutputString, OutputStringCch, 0 };
    return EnumerateLoadedModulesExW(ProcessHandle, RProc_TranslateAddress_EnumloadedModulesCallback, &st) ?
        (st.CchOutput ? st.CchOutput : wnsprintfW(OutputString, OutputStringCch, L"%p", Address)) :
        0;
}

NTSTATUS NTAPI RProc_ReadMemStringExW(HANDLE ProcessHandle, PVOID Address, PWSTR OutputString, UINT OutputStringCch) {
    UINT        i = 0;
    PWCHAR      lpSrcChar = Address, lpDstChar = OutputString;
    NTSTATUS    lStatus;
    do {
        lStatus = RProc_MemReadEx(ProcessHandle, lpSrcChar, lpDstChar, sizeof(WCHAR));
        if (!NT_SUCCESS(lStatus)) {
            *lpDstChar = '\0';
            return lStatus;
        }
        if (*lpDstChar == '\0')
            return STATUS_SUCCESS;
        i++;
        lpSrcChar++;
        lpDstChar++;
    } while (i < OutputStringCch - 1);
    *lpDstChar = '\0';
    return STATUS_PARTIAL_COPY;
}

NTSTATUS NTAPI RProc_ReadMemStringExA(HANDLE ProcessHandle, PVOID Address, PSTR OutputString, UINT OutputStringCch) {
    UINT        i = 0;
    PCHAR       lpSrcChar = Address, lpDstChar = OutputString;
    NTSTATUS    lStatus;
    do {
        lStatus = RProc_MemReadEx(ProcessHandle, lpSrcChar, lpDstChar, sizeof(CHAR));
        if (!NT_SUCCESS(lStatus)) {
            *lpDstChar = '\0';
            return lStatus;
        }
        if (*lpDstChar == '\0')
            return STATUS_SUCCESS;
        i++;
        lpSrcChar++;
        lpDstChar++;
    } while (i < OutputStringCch - 1);
    *lpDstChar = '\0';
    return STATUS_PARTIAL_COPY;
}

VOID NTAPI RProc_InitMap(PRPROC_MAP RemoteMemMap, PVOID LocalAddress, SIZE_T LocalSize, ULONG RemotePageProtect) {
    RemoteMemMap->Local = LocalAddress;
    RemoteMemMap->LocalSize = LocalSize;
    RemoteMemMap->Remote = NULL;
    RemoteMemMap->RemotePageProtect = RemotePageProtect;
}

NTSTATUS NTAPI RProc_MemMap(HANDLE ProcessHandle, PRPROC_MAP RemoteMemMap) {
    NTSTATUS    lStatus;
    ULONG       ulProtect = 0;

    // Allocate remote memory
    RemoteMemMap->RemoteSize = RemoteMemMap->LocalSize;
    lStatus = NtAllocateVirtualMemory(ProcessHandle, &RemoteMemMap->Remote, 0, &RemoteMemMap->RemoteSize, MEM_COMMIT, PAGE_READWRITE);
    if (!NT_SUCCESS(lStatus))
        goto Label_0;

    // Write data to remote memory
    lStatus = RProc_MemWriteEx(ProcessHandle, RemoteMemMap->Remote, RemoteMemMap->Local, RemoteMemMap->LocalSize);
    if (!NT_SUCCESS(lStatus))
        goto Label_1;

    // Set final page protections
    lStatus = NtProtectVirtualMemory(ProcessHandle, &RemoteMemMap->Remote, &RemoteMemMap->RemoteSize, RemoteMemMap->RemotePageProtect, &ulProtect);
    if (!NT_SUCCESS(lStatus))
        goto Label_1;

    // Flush cache if needed
    if (RemoteMemMap->RemotePageProtect & PAGE_EXECUTE ||
        RemoteMemMap->RemotePageProtect & PAGE_EXECUTE_READ ||
        RemoteMemMap->RemotePageProtect & PAGE_EXECUTE_READWRITE ||
        RemoteMemMap->RemotePageProtect & PAGE_EXECUTE_WRITECOPY ||
        RemoteMemMap->RemotePageProtect & PAGE_GRAPHICS_EXECUTE ||
        RemoteMemMap->RemotePageProtect & PAGE_GRAPHICS_EXECUTE_READ ||
        RemoteMemMap->RemotePageProtect & PAGE_GRAPHICS_EXECUTE_READWRITE)
        NtFlushInstructionCache(ProcessHandle, RemoteMemMap->Remote, RemoteMemMap->RemoteSize);

    // Done
    return STATUS_SUCCESS;

    // Cleanup and exit
Label_1:
    NtFreeVirtualMemory(ProcessHandle, &RemoteMemMap->Remote, &RemoteMemMap->RemoteSize, MEM_DECOMMIT);
Label_0:
    return lStatus;
}