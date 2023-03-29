#include "Include\NTARProc.h"

#include <UserEnv.h>

#include "Include\NTANT.h"
#include "Include\NTAStr.h"
#include "Include\NTAProc.h"
#include "Include\NTASys.h"

#pragma comment(lib, "UserEnv.lib")
#pragma comment(lib, "WIE_Kernel32.lib")

#if !defined(_WINTEXPORTS_)
static PFNCreateProcessInternalW pfnCreateProcessInternalW = NULL;
#endif

_Check_return_
HANDLE NTAPI RProc_Create(_In_opt_ HANDLE TokenHandle, _In_opt_ PCWSTR ApplicationName, _Inout_opt_ LPWSTR CommandLine, _In_ BOOL InheritHandles, _In_opt_ PCWSTR CurrentDirectory, _In_opt_ LPSTARTUPINFOW StartupInfo)
{
    HANDLE hProc = NULL;
    PROCESS_INFORMATION pi;
    DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_DEFAULT_ERROR_MODE;
    PVOID pEnv = NULL;

    #if !defined(_WINTEXPORTS_)
    if (!pfnCreateProcessInternalW) {
        pfnCreateProcessInternalW = (PFNCreateProcessInternalW)Sys_LoadAPI(SysDllNameKernel32, "CreateProcessInternalW");
        if (!pfnCreateProcessInternalW) {
            EH_SetLastNTError(WIE_GetLastStatus());
            return NULL;
        }
    }
    #endif

    if (TokenHandle) {
        if (CreateEnvironmentBlock(&pEnv, TokenHandle, FALSE)) {
            dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
        }
    }
    LPSTARTUPINFOW psi;
    if (StartupInfo) {
        psi = StartupInfo;
    } else {
        STARTUPINFOW si = { sizeof(si), NULL, L"winsta0\\default" };
        psi = &si;
    }

    if (
    #if defined(_WINTEXPORTS_)
    CreateProcessInternalW
    #else
    pfnCreateProcessInternalW
    #endif
    (TokenHandle, ApplicationName, CommandLine, NULL, NULL, InheritHandles, dwCreationFlags, pEnv, CurrentDirectory, psi, &pi, NULL)) {
        NtClose(pi.hThread);
        hProc = pi.hProcess;
    }
    if (pEnv) {
        DestroyEnvironmentBlock(pEnv);
    }

    return hProc;
}

BOOL NTAPI RProc_EnumDlls64(_In_ HANDLE ProcessHandle, _In_ RPROC_DLLENUMPROC64 DllEnumProc, LPARAM Param)
{
    NTSTATUS                    lStatus;
    PROCESS_BASIC_INFORMATION   stProcInfo;
    PPEB64                      pPEB;
    PPEB_LDR_DATA64             pLdr;
    LDR_DATA_TABLE_ENTRY64      *pHead, *pNode, stCurrentEntry;

    lStatus = NtQueryInformationProcess(ProcessHandle, ProcessBasicInformation, &stProcInfo, sizeof(stProcInfo), NULL);
    if (!NT_SUCCESS(lStatus)) {
        goto Label_0;
    } else if (!stProcInfo.PebBaseAddress) {
        lStatus = STATUS_INVALID_ADDRESS;
        goto Label_0;
    }

    pPEB = (PVOID)stProcInfo.PebBaseAddress;
    lStatus = NtReadVirtualMemory(ProcessHandle, &pPEB->Ldr, &pLdr, sizeof(pLdr), NULL);
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
        pNode = CONTAINING_RECORD(stCurrentEntry.InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY64, InLoadOrderModuleList);
    } while (pNode != pHead);
    return TRUE;

Label_0:
    WIE_SetLastStatus(lStatus);
    return FALSE;
}

BOOL NTAPI RProc_EnumDlls32(_In_ HANDLE ProcessHandle, _In_ RPROC_DLLENUMPROC32 DllEnumProc, LPARAM Param)
{
    NTSTATUS                    lStatus;
    PVOID                       p;
    LDR_DATA_TABLE_ENTRY32      *pHead, *pNode, stCurrentEntry;

    #ifdef _WIN64
    if (!RProc_GetWow64PEB(ProcessHandle, (PPEB32*)&p)) {
        return FALSE;
    }
    #else
    PROCESS_BASIC_INFORMATION stProcInfo;
    lStatus = NtQueryInformationProcess(ProcessHandle, ProcessBasicInformation, &stProcInfo, sizeof(stProcInfo), NULL);
    if (!NT_SUCCESS(lStatus)) {
        goto Label_0;
    }
    p = stProcInfo.PebBaseAddress;
    #endif

    lStatus = NtReadVirtualMemory(ProcessHandle, Add2Ptr(p, UFIELD_OFFSET(PEB32, Ldr)), &p, sizeof(p), NULL);
    if (!NT_SUCCESS(lStatus)) {
        goto Label_0;
    }
    p = PURGE_PTR32(p);
    lStatus = NtReadVirtualMemory(ProcessHandle, Add2Ptr(p, UFIELD_OFFSET(PEB_LDR_DATA32, InLoadOrderModuleList.Flink)), &pHead, sizeof(pHead), NULL);
    if (!NT_SUCCESS(lStatus)) {
        goto Label_0;
    }
    pHead = PURGE_PTR32(pHead);
    pNode = pHead;
    do {
        lStatus = NtReadVirtualMemory(ProcessHandle, pNode, &stCurrentEntry, sizeof(stCurrentEntry), NULL);
        if (!NT_SUCCESS(lStatus)) {
            goto Label_0;
        }
        if (!DllEnumProc(ProcessHandle, &stCurrentEntry, Param)) {
            break;
        }
        pNode = CONTAINING_RECORD((DWORD_PTR)stCurrentEntry.InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY32, InLoadOrderModuleList);
    } while (pNode != pHead);
    return TRUE;

Label_0:
    WIE_SetLastStatus(lStatus);
    return FALSE;
    }

_Success_(return != FALSE)
BOOL NTAPI RProc_CreateThread(_In_ HANDLE ProcessHandle, _In_ LPTHREAD_START_ROUTINE StartAddress, _In_opt_ __drv_aliasesMem PVOID Param, BOOL CreateSuspended, _Out_opt_ PHANDLE ThreadHandle)
{
    NTSTATUS lStatus = RtlCreateUserThread(ProcessHandle, NULL, CreateSuspended, 0, 0, 0, StartAddress, Param, ThreadHandle, NULL);
    if (NT_SUCCESS(lStatus)) {
        return TRUE;
    } else {
        WIE_SetLastStatus(lStatus);
        return FALSE;
    }
}

_Success_(return > 0)
SIZE_T NTAPI RProc_ReadMemory(_In_ HANDLE ProcessHandle, _In_ PVOID BaseAddress, _Out_writes_bytes_(Size) PVOID Buffer, _In_ SIZE_T Size)
{
    SIZE_T sBytesWritten;
    NTSTATUS lStatus = NtReadVirtualMemory(ProcessHandle, BaseAddress, Buffer, Size, &sBytesWritten);
    if (NT_SUCCESS(lStatus)) {
        return sBytesWritten;
    } else {
        WIE_SetLastStatus(lStatus);
        return 0;
    }
}

_Success_(return > 0)
SIZE_T NTAPI RProc_WriteMemory(_In_ HANDLE ProcessHandle, _In_ PVOID BaseAddress, _In_reads_bytes_(Size) LPCVOID Buffer, _In_ SIZE_T Size)
{
    SIZE_T sBytesRead;
    NTSTATUS lStatus = NtWriteVirtualMemory(ProcessHandle, BaseAddress, (PVOID)Buffer, Size, &sBytesRead);
    if (NT_SUCCESS(lStatus)) {
        return sBytesRead;
    } else {
        WIE_SetLastStatus(lStatus);
        return 0;
    }
}

HANDLE NTAPI RProc_Open(DWORD DesiredAccess, DWORD ProcessId)
{
    HANDLE              hProc = NULL;
    OBJECT_ATTRIBUTES   stObjectAttr;
    CLIENT_ID           stClientId;
    NTSTATUS            lStatus;
    NT_InitObject(&stObjectAttr, NULL, NULL, 0);
    stClientId.UniqueProcess = (HANDLE)(DWORD_PTR)ProcessId;
    stClientId.UniqueThread = 0;
    lStatus = NtOpenProcess(&hProc, DesiredAccess, &stObjectAttr, &stClientId);
    if (!NT_SUCCESS(lStatus)) {
        WIE_SetLastStatus(lStatus);
    }
    return hProc;
}

HANDLE NTAPI RProc_OpenThread(DWORD DesiredAccess, DWORD ThreadId)
{
    HANDLE              hThread = NULL;
    OBJECT_ATTRIBUTES   stObjectAttr;
    CLIENT_ID           stClientId;
    NTSTATUS            lStatus;
    NT_InitObject(&stObjectAttr, NULL, NULL, 0);
    stClientId.UniqueProcess = 0;
    stClientId.UniqueThread = (HANDLE)(DWORD_PTR)ThreadId;
    lStatus = NtOpenThread(&hThread, DesiredAccess, &stObjectAttr, &stClientId);
    if (!NT_SUCCESS(lStatus)) {
        WIE_SetLastStatus(lStatus);
    }
    return hThread;
}

BOOL NTAPI RProc_AdjustPrivilege(_In_ HANDLE ProcessHandle, _In_ ULONG Privilege, _In_ DWORD Attributes)
{
    HANDLE      hToken;
    NTSTATUS    Status;
    Status = NtOpenProcessToken(ProcessHandle, TOKEN_ADJUST_PRIVILEGES, &hToken);
    if (NT_SUCCESS(Status)) {
        if (NT_AdjustPrivilege(hToken, Privilege, Attributes)) {
            return TRUE;
        }
    } else {
        WIE_SetLastStatus(Status);
    }
    return FALSE;
}

_Success_(return > 0)
UINT NTAPI RProc_GetFullImageNameEx(HANDLE ProcessHandle, _Out_writes_z_(FilePathCch) PWSTR FilePath, _In_ UINT FilePathCch)
{
    BYTE stString[sizeof(UNICODE_STRING) + MAX_PATH * sizeof(WCHAR)];
    NTSTATUS lStatus = NtQueryInformationProcess(ProcessHandle, ProcessImageFileNameWin32, &stString, sizeof(stString), NULL);
    if (NT_SUCCESS(lStatus)) {
        USHORT usLen = ((PUNICODE_STRING)stString)->Length;
        if (usLen < FilePathCch * sizeof(WCHAR)) {
            RtlCopyMemory(FilePath, ((PUNICODE_STRING)stString)->Buffer, usLen);
            usLen >>= 1;
            FilePath[usLen] = L'\0';
            return usLen;
        } else {
            lStatus = STATUS_BUFFER_TOO_SMALL;
        }
    }
    WIE_SetLastStatus(lStatus);
    return 0;
}

typedef struct _RPROC_TRANSLATEADDRESS64 {
    ULONGLONG   Address;
    PWSTR       String;
    UINT        Cch;
    UINT        CchOutput;
} RPROC_TRANSLATEADDRESS64, *PRPROC_TRANSLATEADDRESS64;

typedef struct _RPROC_TRANSLATEADDRESS32 {
    ULONG   Address;
    PWSTR   String;
    UINT    Cch;
    UINT    CchOutput;
} RPROC_TRANSLATEADDRESS32, *PRPROC_TRANSLATEADDRESS32;

static BOOL CALLBACK RProc_TranslateAddress_EnumDllProc64(HANDLE ProcessHandle, PLDR_DATA_TABLE_ENTRY64 DllLdrEntry, LPARAM Param)
{
    PRPROC_TRANSLATEADDRESS64 pst = (PRPROC_TRANSLATEADDRESS64)Param;
    WCHAR                   szDllName[MAX_PATH];
    SIZE_T                  sNameBytes, sReadBytes;
    if (pst->Address >= (ULONGLONG)DllLdrEntry->DllBase &&
        pst->Address < (ULONGLONG)DllLdrEntry->DllBase + DllLdrEntry->SizeOfImage) {
        sNameBytes = DllLdrEntry->BaseDllName.Length;
        if (sNameBytes >= sizeof(szDllName)) {
            sNameBytes = sizeof(szDllName) - sizeof(WCHAR);
        }
        if (NT_SUCCESS(NtReadVirtualMemory(ProcessHandle, (PVOID)(PVOID64)DllLdrEntry->BaseDllName.Buffer, szDllName, sNameBytes, &sReadBytes))) {
            szDllName[sReadBytes / sizeof(WCHAR)] = '\0';
            pst->CchOutput = Str_PrintfExW(pst->String, pst->Cch, L"%s!%016llX", szDllName, pst->Address);
        }
        return FALSE;
    } else {
        return TRUE;
    }
}

static BOOL CALLBACK RProc_TranslateAddress_EnumDllProc32(HANDLE ProcessHandle, PLDR_DATA_TABLE_ENTRY32 DllLdrEntry, LPARAM Param)
{
    PRPROC_TRANSLATEADDRESS32 pst = (PRPROC_TRANSLATEADDRESS32)Param;
    WCHAR                   szDllName[MAX_PATH];
    SIZE_T                  sNameBytes, sReadBytes;
    if (pst->Address >= (ULONG)DllLdrEntry->DllBase &&
        pst->Address < (ULONG)DllLdrEntry->DllBase + DllLdrEntry->SizeOfImage) {
        sNameBytes = DllLdrEntry->BaseDllName.Length;
        if (sNameBytes >= sizeof(szDllName)) {
            sNameBytes = sizeof(szDllName) - sizeof(WCHAR);
        }
        if (NT_SUCCESS(NtReadVirtualMemory(ProcessHandle, (PVOID)(ULONG_PTR)DllLdrEntry->BaseDllName.Buffer, szDllName, sNameBytes, &sReadBytes))) {
            szDllName[sReadBytes / sizeof(WCHAR)] = '\0';
            pst->CchOutput = Str_PrintfExW(pst->String, pst->Cch, L"%s!%08X", szDllName, pst->Address);
        }
        return FALSE;
    } else {
        return TRUE;
    }
}

_Success_(return > 0)
UINT NTAPI RProc_TranslateAddressEx(HANDLE ProcessHandle, _In_ ULONGLONG Address, _Out_writes_z_(OutputStringCch) PWSTR OutputString, _In_ UINT OutputStringCch)
{
    UINT uRet = 0;
    BOOL b32Proc;
    #ifdef _WIN64
    if (!RProc_IsWow64(ProcessHandle, &b32Proc)) {
        return 0;
    }
    #else
    b32Proc = TRUE;
    #endif

    if (!b32Proc) {
        RPROC_TRANSLATEADDRESS64 st = { Address, OutputString, OutputStringCch, 0 };
        uRet = RProc_EnumDlls64(ProcessHandle, RProc_TranslateAddress_EnumDllProc64, (LPARAM)&st) ?
            (st.CchOutput ? st.CchOutput : Str_PrintfExW(OutputString, OutputStringCch, L"%016llX", Address)) :
            0;
    } else {
        RPROC_TRANSLATEADDRESS32 st = { (ULONG)Address, OutputString, OutputStringCch, 0 };
        uRet = RProc_EnumDlls32(ProcessHandle, RProc_TranslateAddress_EnumDllProc32, (LPARAM)&st) ?
            (st.CchOutput ? st.CchOutput : Str_PrintfExW(OutputString, OutputStringCch, L"%08X", (ULONG)Address)) :
            0;
    }
    return uRet;
    }

#pragma warning(disable: __WARNING_MISSING_ZERO_TERMINATION2)
_Success_(return > 0)
UINT NTAPI RProc_ReadMemStringExW(HANDLE ProcessHandle, _In_ PVOID Address, _Out_writes_z_(OutputStringCch) PWSTR OutputString, _In_ UINT OutputStringCch)
{
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
UINT NTAPI RProc_ReadMemStringExA(HANDLE ProcessHandle, _In_ PVOID Address, _Out_writes_z_(OutputStringCch) PSTR OutputString, _In_ UINT OutputStringCch)
{
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
#pragma warning(default: __WARNING_MISSING_ZERO_TERMINATION2)

VOID NTAPI RProc_InitMap(_Out_ PRPROC_MAP RemoteMemMap, _In_ PVOID LocalAddress, _In_ SIZE_T LocalSize, _In_opt_ PVOID RemoteAddress, _In_ ULONG RemotePageProtect)
{
    RemoteMemMap->Local = LocalAddress;
    RemoteMemMap->LocalSize = LocalSize;
    RemoteMemMap->Remote = RemoteAddress;
    RemoteMemMap->RemotePageProtect = RemotePageProtect;
}

_Success_(return != FALSE)
BOOL NTAPI RProc_MemMap(HANDLE ProcessHandle, _Inout_ PRPROC_MAP RemoteMemMap)
{
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

BOOL NTAPI RProc_MemUnmap(HANDLE ProcessHandle, _In_ PRPROC_MAP RemoteMemMap)
{
    PVOID       pBase = RemoteMemMap->Remote;
    SIZE_T      uSize = 0;
    NTSTATUS    lStatus = NtFreeVirtualMemory(ProcessHandle, &pBase, &uSize, MEM_RELEASE);
    if (NT_SUCCESS(lStatus)) {
        return TRUE;
    } else {
        WIE_SetLastStatus(lStatus);
        return FALSE;
    }
}

_Success_(return != FALSE)
BOOL NTAPI RProc_GetWow64PEB(_In_ HANDLE hProcess, _Out_ PPEB32 * Wow64PEB)
{
    NTSTATUS lStatus = NtQueryInformationProcess(hProcess, ProcessWow64Information, Wow64PEB, sizeof(*Wow64PEB), NULL);
    if (NT_SUCCESS(lStatus)) {
        return TRUE;
    } else {
        WIE_SetLastStatus(lStatus);
        return FALSE;
    }
}

_Success_(return != FALSE)
BOOL NTAPI RProc_IsWow64(_In_ HANDLE hProcess, _Out_ PBOOL Wow64Process)
{
    PPEB32 Wow64PEB;
    if (RProc_GetWow64PEB(hProcess, &Wow64PEB)) {
        *Wow64Process = Wow64PEB != NULL;
        return TRUE;
    } else {
        return FALSE;
    }
}
