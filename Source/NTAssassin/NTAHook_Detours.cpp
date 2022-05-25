// Use native instructions and system calls instead

#define GetCurrentProcess MS_GetCurrentProcess
#define GetCurrentThreadId MS_GetCurrentThreadId
#define VirtualAlloc MS_VirtualAlloc
#define VirtualQuery MS_VirtualQuery
#define VirtualProtect MS_VirtualProtect
#define VirtualFree MS_VirtualFree
#define memset MS_memset
#define GetThreadContext MS_GetThreadContext
#define SetThreadContext MS_SetThreadContext
#define ResumeThread MS_ResumeThread
#define FlushInstructionCache MS_FlushInstructionCache
#define SetLastError MS_SetLastError
#define GetLastError MS_GetLastError

#include "include\NTAssassin\NTAssassin.h"

#undef GetCurrentProcess
#undef GetCurrentThreadId
#undef VirtualAlloc
#undef VirtualQuery
#undef VirtualProtect
#undef VirtualFree
#undef memset
#undef FlushInstructionCache
#undef GetThreadContext
#undef SetThreadContext
#undef ResumeThread
#undef SetLastError
#undef GetLastError

#define GetCurrentProcess() CURRENT_PROCESS_HANDLE
#define GetCurrentThreadId() CURRENT_THREAD_ID
#define VirtualAlloc NTAHookIntl_VirtualAlloc
#define VirtualQuery NTAHookIntl_VirtualQuery
#define VirtualProtect NTAHookIntl_VirtualProtect
#define VirtualFree NTAHookIntl_VirtualFree
#define memset(_Dst, _Val, _Size) RtlFillMemory(_Dst, _Size, _Val)
#define FlushInstructionCache NTAHookIntl_FlushInstructionCache
#define GetThreadContext NTAHookIntl_GetThreadContext
#define SetThreadContext NTAHookIntl_SetThreadContext
#define ResumeThread NTAHookIntl_ResumeThread
#define SetLastError NT_SetLastError
#define GetLastError NT_GetLastError

#define GetModuleHandleW(NULL) IMAGE_BASE

_Ret_maybenull_ _Post_writable_byte_size_(dwSize)
LPVOID WINAPI NTAHookIntl_VirtualAlloc(_In_opt_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD flAllocationType, _In_ DWORD flProtect) {
    NTSTATUS lStatus = NtAllocateVirtualMemory(CURRENT_PROCESS_HANDLE, &lpAddress, 0, &dwSize, flAllocationType, flProtect);
    if (!NT_SUCCESS(lStatus)) {
        NT_SetLastNTError(lStatus);
        return NULL;
    }
    return lpAddress;
}

SIZE_T WINAPI NTAHookIntl_VirtualQuery(_In_opt_ LPCVOID lpAddress, _Out_writes_bytes_to_(dwLength, return) PMEMORY_BASIC_INFORMATION lpBuffer, _In_ SIZE_T dwLength) {
    NTSTATUS lStatus;
    SIZE_T ResultLength = 0;
    lStatus = NtQueryVirtualMemory(CURRENT_PROCESS_HANDLE, (LPVOID)lpAddress, MemoryBasicInformation, lpBuffer, dwLength, &ResultLength);
    if (!NT_SUCCESS(lStatus)) {
        NT_SetLastNTError(lStatus);
    }
    return ResultLength;
}

_Success_(return != FALSE)
BOOL WINAPI NTAHookIntl_VirtualProtect(_In_ LPVOID lpAddress, _In_  SIZE_T dwSize, _In_  DWORD flNewProtect, PDWORD lpflOldProtect) {
    NTSTATUS lStatus = NtProtectVirtualMemory(CURRENT_PROCESS_HANDLE, &lpAddress, &dwSize, flNewProtect, lpflOldProtect);
    if (!NT_SUCCESS(lStatus)) {
        NT_SetLastNTError(lStatus);
        return FALSE;
    }
    return TRUE;
}

_When_(((dwFreeType & (MEM_RELEASE | MEM_DECOMMIT))) == (MEM_RELEASE | MEM_DECOMMIT),
    __drv_reportError("Passing both MEM_RELEASE and MEM_DECOMMIT to VirtualFree is not allowed. This results in the failure of this call"))
    _When_(dwFreeType == 0,
        __drv_reportError("Passing zero as the dwFreeType parameter to VirtualFree is not allowed. This results in the failure of this call"))
    _When_(((dwFreeType & MEM_RELEASE)) != 0 && dwSize != 0,
        __drv_reportError("Passing MEM_RELEASE and a non-zero dwSize parameter to VirtualFree is not allowed. This results in the failure of this call"))
    _Success_(return != FALSE)
    BOOL WINAPI NTAHookIntl_VirtualFree(LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD dwFreeType) {
    NTSTATUS lStatus;
    if (!(dwSize) || !(dwFreeType & MEM_RELEASE)) {
        lStatus = NtFreeVirtualMemory(CURRENT_PROCESS_HANDLE, &lpAddress, &dwSize, dwFreeType);
        if (!NT_SUCCESS(lStatus)) {
            NT_SetLastNTError(lStatus);
            return FALSE;
        }
        return TRUE;
    }
    NT_SetLastNTError(STATUS_INVALID_PARAMETER);
    return FALSE;
}

BOOL WINAPI NTAHookIntl_FlushInstructionCache(_In_ HANDLE hProcess, _In_reads_bytes_opt_(dwSize) LPCVOID lpBaseAddress, _In_ SIZE_T dwSize) {
    NTSTATUS lStatus = NtFlushInstructionCache(hProcess, (PVOID)lpBaseAddress, dwSize);
    if (!NT_SUCCESS(lStatus)) {
        NT_SetLastNTError(lStatus);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI NTAHookIntl_GetThreadContext(_In_ HANDLE hThread, _Inout_ LPCONTEXT lpContext) {
    NTSTATUS lStatus = NtGetContextThread(hThread, lpContext);
    if (!NT_SUCCESS(lStatus)) {
        NT_SetLastNTError(lStatus);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI NTAHookIntl_SetThreadContext(_In_ HANDLE hThread, _In_ CONST CONTEXT* lpContext) {
    NTSTATUS lStatus = NtSetContextThread(hThread, (PCONTEXT)lpContext);
    if (!NT_SUCCESS(lStatus)) {
        NT_SetLastNTError(lStatus);
        return FALSE;
    }
    return TRUE;
}

DWORD WINAPI NTAHookIntl_ResumeThread(_In_ HANDLE hThread) {
    ULONG PreviousResumeCount;
    NTSTATUS lStatus = NtResumeThread(hThread, &PreviousResumeCount);
    if (!NT_SUCCESS(lStatus)) {
        NT_SetLastNTError(lStatus);
        return -1;
    }
    return PreviousResumeCount;
}

// Hook by using Detours
#pragma warning(disable: 26495)

#include "3rdParty\src\Detours\detours.cpp"
#include "3rdParty\src\Detours\disasm.cpp"
#include "3rdParty\src\Detours\modules.cpp"

#pragma warning(default: 26495)