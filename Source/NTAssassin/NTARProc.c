#include "include\NTAssassin\NTARProc.h"
#include "include\NTAssassin\NTANT.h"
#include "include\NTAssassin\NTAStr.h"

BOOL NTAPI RProc_EnumDlls(_In_ HANDLE ProcessHandle, _In_ RPROC_DLLENUMPROC DllEnumProc, LPARAM Param) {
    NTSTATUS                    lStatus;
    PROCESS_BASIC_INFORMATION   stProcInfo;
    PPEB_LDR_DATA               pLdr;
    LDR_DATA_TABLE_ENTRY        *pHead, *pNode, stCurrentEntry;
    stProcInfo.PebBaseAddress = NULL;
    lStatus = NtQueryInformationProcess(ProcessHandle, ProcessBasicInformation, &stProcInfo, sizeof(stProcInfo), NULL);
    if (!NT_SUCCESS(lStatus) || !stProcInfo.PebBaseAddress) {
        goto Label_0;
    }
    lStatus = NtReadVirtualMemory(ProcessHandle, &stProcInfo.PebBaseAddress->Ldr, &pLdr, sizeof(pLdr), NULL);
    if (!NT_SUCCESS(lStatus)) {
        goto Label_0;
    }
    lStatus = NtReadVirtualMemory(ProcessHandle, &pLdr->InLoadOrderModuleList.Flink, &pHead, sizeof(pHead), NULL);
    if (!NT_SUCCESS(lStatus)) {
        goto Label_0;
    }
    pNode = pHead;
    do {
        lStatus = NtReadVirtualMemory(ProcessHandle, pNode, &stCurrentEntry, sizeof(stCurrentEntry), NULL);
        if (!NT_SUCCESS(lStatus)) {
            goto Label_0;
        }
        if (!DllEnumProc(ProcessHandle, &stCurrentEntry, Param)) {
            break;
        }
        pNode = CONTAINING_RECORD(stCurrentEntry.InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);
    } while (pNode != pHead);
    return TRUE;
Label_0:
    NT_SetLastStatus(lStatus);
    return FALSE;
}

_Success_(return != FALSE)
BOOL NTAPI RProc_CreateThread(_In_ HANDLE ProcessHandle, _In_ LPTHREAD_START_ROUTINE StartAddress, _In_opt_ __drv_aliasesMem PVOID Param, BOOL CreateSuspended, _Out_opt_ PHANDLE ThreadHandle) {
    NTSTATUS lStatus = RtlCreateUserThread(ProcessHandle, NULL, CreateSuspended, 0, 0, 0, StartAddress, Param, ThreadHandle, NULL);
    if (NT_SUCCESS(lStatus)) {
        return TRUE;
    } else {
        NT_SetLastStatus(lStatus);
        return FALSE;
    }
}

_Success_(return > 0)
SIZE_T NTAPI RProc_ReadMemory(_In_ HANDLE ProcessHandle, _In_ PVOID BaseAddress, _Out_writes_bytes_(Size) PVOID Buffer, _In_ SIZE_T Size) {
    SIZE_T sBytesWritten;
    NTSTATUS lStatus = NtReadVirtualMemory(ProcessHandle, BaseAddress, Buffer, Size, &sBytesWritten);
    if (NT_SUCCESS(lStatus)) {
        return sBytesWritten;
    } else {
        NT_SetLastStatus(lStatus);
        return 0;
    }
}

_Success_(return > 0)
SIZE_T NTAPI RProc_WriteMemory(_In_ HANDLE ProcessHandle, _In_ PVOID BaseAddress, _In_reads_bytes_(Size) LPCVOID Buffer, _In_ SIZE_T Size) {
    SIZE_T sBytesRead;
    NTSTATUS lStatus = NtWriteVirtualMemory(ProcessHandle, BaseAddress, (PVOID)Buffer, Size, &sBytesRead);
    if (NT_SUCCESS(lStatus)) {
        return sBytesRead;
    } else {
        NT_SetLastStatus(lStatus);
        return 0;
    }
}

HANDLE NTAPI RProc_Open(DWORD DesiredAccess, DWORD ProcessId) {
    HANDLE              hProc = NULL;
    OBJECT_ATTRIBUTES   stObjectAttr;
    CLIENT_ID           stClientId;
    NTSTATUS            lStatus;
    NT_InitObject(&stObjectAttr, NULL, NULL, 0);
    stClientId.UniqueProcess = ProcessId;
    stClientId.UniqueThread = 0;
    lStatus = NtOpenProcess(&hProc, DesiredAccess, &stObjectAttr, &stClientId);
    if (!NT_SUCCESS(lStatus)) {
        NT_SetLastStatus(lStatus);
    }
    return hProc;
}

HANDLE NTAPI RProc_OpenThread(DWORD DesiredAccess, DWORD ThreadId) {
    HANDLE              hThread = NULL;
    OBJECT_ATTRIBUTES   stObjectAttr;
    CLIENT_ID           stClientId;
    NTSTATUS            lStatus;
    NT_InitObject(&stObjectAttr, NULL, NULL, 0);
    stClientId.UniqueProcess = 0;
    stClientId.UniqueThread = ThreadId;
    lStatus = NtOpenThread(&hThread, DesiredAccess, &stObjectAttr, &stClientId);
    if (!NT_SUCCESS(lStatus)) {
        NT_SetLastStatus(lStatus);
    }
    return hThread;
}

BOOL NTAPI RProc_AdjustPrivilege(HANDLE ProcessHandle, SE_PRIVILEGE Privilege, BOOL EnableState) {
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
        if (NT_SUCCESS(lStatus)) {
            return TRUE;
        }
    }
    NT_SetLastStatus(lStatus);
    return FALSE;
}

_Success_(return > 0)
UINT NTAPI RProc_GetFullImageNameEx(HANDLE ProcessHandle, _Out_writes_z_(FilePathCch) PWSTR FilePath, _In_ UINT FilePathCch) {
    BYTE stString[sizeof(UNICODE_STRING) + MAX_PATH * sizeof(WCHAR)];
    NTSTATUS lStatus = NtQueryInformationProcess(ProcessHandle, ProcessImageFileNameWin32, &stString, sizeof(stString), NULL);
    if (NT_SUCCESS(lStatus)) {
        USHORT usLen = ((PUNICODE_STRING)stString)->Length;
        if (usLen < FilePathCch * sizeof(WCHAR)) {
            RtlMoveMemory(FilePath, ((PUNICODE_STRING)stString)->Buffer, usLen);
            usLen >>= 1;
            FilePath[usLen] = L'\0';
            return usLen;
        } else {
            lStatus = STATUS_BUFFER_TOO_SMALL;
        }
    }
    NT_SetLastStatus(lStatus);
    return 0;
}

typedef struct _RPROC_TRANSLATEADDRESS {
    PVOID   Address;
    PWSTR   String;
    UINT    Cch;
    UINT    CchOutput;
} RPROC_TRANSLATEADDRESS, *PRPROC_TRANSLATEADDRESS;

static BOOL CALLBACK RProc_TranslateAddress_EnumDllProc(HANDLE ProcessHandle, PLDR_DATA_TABLE_ENTRY DllLdrEntry, LPARAM Param) {
    PRPROC_TRANSLATEADDRESS pst = (PRPROC_TRANSLATEADDRESS)Param;
    WCHAR                   szDllName[MAX_PATH];
    SIZE_T                  sNameBytes, sReadBytes;
    if ((DWORD_PTR)pst->Address >= (DWORD_PTR)DllLdrEntry->DllBase &&
        (DWORD_PTR)pst->Address < (DWORD_PTR)DllLdrEntry->DllBase + (DWORD_PTR)DllLdrEntry->SizeOfImage) {
        sNameBytes = DllLdrEntry->BaseDllName.Length;
        if (sNameBytes >= sizeof(szDllName)) {
            sNameBytes = sizeof(szDllName) - sizeof(WCHAR);
        }
        if (NT_SUCCESS(NtReadVirtualMemory(ProcessHandle, DllLdrEntry->BaseDllName.Buffer, szDllName, sNameBytes, &sReadBytes))) {
            szDllName[sReadBytes / sizeof(WCHAR)] = '\0';
            pst->CchOutput = Str_PrintfExW(pst->String, pst->Cch, L"%s!%p", szDllName, pst->Address);
        }
        return FALSE;
    } else {
        return TRUE;
    }
}

_Success_(return > 0)
UINT NTAPI RProc_TranslateAddressEx(HANDLE ProcessHandle, _In_ PVOID Address, _Out_writes_z_(OutputStringCch) PWSTR OutputString, _In_ UINT OutputStringCch) {
    RPROC_TRANSLATEADDRESS st = { Address, OutputString, OutputStringCch, 0 };
    return RProc_EnumDlls(ProcessHandle, RProc_TranslateAddress_EnumDllProc, (LPARAM)&st) ?
        (st.CchOutput ? st.CchOutput : Str_PrintfExW(OutputString, OutputStringCch, L"%p", Address)) :
        0;
}

#pragma warning(disable: 6054)
_Success_(return > 0)
UINT NTAPI RProc_ReadMemStringExW(HANDLE ProcessHandle, _In_ PVOID Address, _Out_writes_z_(OutputStringCch) PWSTR OutputString, _In_ UINT OutputStringCch) {
    UINT    i = 0;
    PWCHAR  pSrcChar = Address, pDstChar = OutputString;
    do {
        if (!RProc_ReadMemory(ProcessHandle, pSrcChar, pDstChar, sizeof(WCHAR))) {
            return 0;
        }
        if (*pDstChar == L'\0') {
            return (UINT)(((UINT_PTR)pDstChar - (UINT_PTR)OutputString) >> 1);
        }
        i++;
        pSrcChar++;
        pDstChar++;
    } while (i < OutputStringCch - 1);
    return 0;
}

_Success_(return > 0)
UINT NTAPI RProc_ReadMemStringExA(HANDLE ProcessHandle, _In_ PVOID Address, _Out_writes_z_(OutputStringCch) PSTR OutputString, _In_ UINT OutputStringCch) {
    UINT    i = 0;
    PCHAR   pSrcChar = Address, pDstChar = OutputString;
    do {
        if (!RProc_ReadMemory(ProcessHandle, pSrcChar, pDstChar, sizeof(CHAR))) {
            return 0;
        }
        if (*pDstChar == '\0') {
            return (UINT)((UINT_PTR)pDstChar - (UINT_PTR)OutputString);
        }
        i++;
        pSrcChar++;
        pDstChar++;
    } while (i < OutputStringCch - 1);
    return 0;
}
#pragma warning(default: 6054)

VOID NTAPI RProc_InitMap(_Out_ PRPROC_MAP RemoteMemMap, _In_ PVOID LocalAddress, _In_ SIZE_T LocalSize, _In_opt_ PVOID RemoteAddress, _In_ ULONG RemotePageProtect) {
    RemoteMemMap->Local = LocalAddress;
    RemoteMemMap->LocalSize = LocalSize;
    RemoteMemMap->Remote = RemoteAddress;
    RemoteMemMap->RemotePageProtect = RemotePageProtect;
}

_Success_(return != FALSE)
BOOL NTAPI RProc_MemMap(HANDLE ProcessHandle, _Inout_ PRPROC_MAP RemoteMemMap) {
    NTSTATUS    lStatus;
    ULONG       ulProtect = 0;

    // Allocate remote memory
    RemoteMemMap->RemoteSize = RemoteMemMap->LocalSize;
    lStatus = NtAllocateVirtualMemory(ProcessHandle, &RemoteMemMap->Remote, 0, &RemoteMemMap->RemoteSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!NT_SUCCESS(lStatus)) {
        goto Label_0;
    }

    // Write data to remote memory
    lStatus = NtWriteVirtualMemory(ProcessHandle, RemoteMemMap->Remote, RemoteMemMap->Local, RemoteMemMap->LocalSize, NULL);
    if (!NT_SUCCESS(lStatus)) {
        goto Label_0;
    }

    // Set final page protections
    lStatus = NtProtectVirtualMemory(ProcessHandle, &RemoteMemMap->Remote, &RemoteMemMap->RemoteSize, RemoteMemMap->RemotePageProtect, &ulProtect);
    if (!NT_SUCCESS(lStatus)) {
        goto Label_1;
    }

    // Flush cache if needed
    if (RemoteMemMap->RemotePageProtect & (
        PAGE_EXECUTE |
        PAGE_EXECUTE_READ |
        PAGE_EXECUTE_READWRITE |
        PAGE_EXECUTE_WRITECOPY |
        PAGE_GRAPHICS_EXECUTE |
        PAGE_GRAPHICS_EXECUTE_READ |
        PAGE_GRAPHICS_EXECUTE_READWRITE)) {
        NtFlushInstructionCache(ProcessHandle, RemoteMemMap->Remote, RemoteMemMap->RemoteSize);
    }
    return TRUE;

Label_1:
    RemoteMemMap->RemoteSize = 0;
    NtFreeVirtualMemory(ProcessHandle, &RemoteMemMap->Remote, &RemoteMemMap->RemoteSize, MEM_RELEASE);
Label_0:
    return FALSE;
}

BOOL NTAPI RProc_MemUnmap(HANDLE ProcessHandle, _In_ PRPROC_MAP RemoteMemMap) {
    PVOID       pBase = RemoteMemMap->Remote;
    SIZE_T      uSize = 0;
    NTSTATUS    lStatus = NtFreeVirtualMemory(ProcessHandle, &pBase, &uSize, MEM_RELEASE);
    if (NT_SUCCESS(lStatus)) {
        return TRUE;
    } else {
        NT_SetLastStatus(lStatus);
        return FALSE;
    }
}

_Success_(return != FALSE)
BOOL NTAPI RProc_IsWow64(_In_ HANDLE hProcess, _Out_ PBOOL Wow64Process) {
    ULONG_PTR pwi;
    NTSTATUS lStatus = NtQueryInformationProcess(hProcess, ProcessWow64Information, &pwi, sizeof(pwi), NULL);
    if (NT_SUCCESS(lStatus)) {
        *Wow64Process = (pwi != 0);
        return TRUE;
    } else {
        NT_SetLastStatus(lStatus);
        return FALSE;
    }
}