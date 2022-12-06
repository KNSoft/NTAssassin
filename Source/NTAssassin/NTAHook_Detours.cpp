// Use native instructions and system calls instead

EXTERN_C_START

#define GetCurrentProcess MS_GetCurrentProcess
#define GetCurrentThreadId MS_GetCurrentThreadId
#define VirtualAlloc MS_VirtualAlloc
#define VirtualQueryEx MS_VirtualQueryEx
#define VirtualQuery MS_VirtualQuery
#define VirtualProtect MS_VirtualProtect
#define VirtualFree MS_VirtualFree
#define memset MS_memset
#define GetThreadContext MS_GetThreadContext
#define SetThreadContext MS_SetThreadContext
#define SuspendThread MS_SuspendThread
#define ResumeThread MS_ResumeThread
#define FlushInstructionCache MS_FlushInstructionCache
#define SetLastError MS_SetLastError
#define GetLastError MS_GetLastError

#include "include\NTAssassin\NTADef.h"
#include "include\NTAssassin\NTAEH.h"
#include "include\NTAssassin\NTAMem.h"

#undef GetCurrentProcess
#undef GetCurrentThreadId
#undef VirtualAlloc
#undef VirtualQueryEx
#undef VirtualQuery
#undef VirtualProtect
#undef VirtualFree
#undef memset
#undef FlushInstructionCache
#undef GetThreadContext
#undef SetThreadContext
#undef SuspendThread
#undef ResumeThread
#undef SetLastError
#undef GetLastError

#define GetCurrentProcess() CURRENT_PROCESS_HANDLE
#define GetCurrentThread() CURRENT_THREAD_HANDLE
#define GetCurrentThreadId() NT_GetCurrentTID()
#define VirtualAlloc NTAHookIntl_VirtualAlloc
#define VirtualQueryEx NTAHookIntl_VirtualQueryEx
#define VirtualQuery(lpAddress, lpBuffer, dwLength) VirtualQueryEx(CURRENT_PROCESS_HANDLE, lpAddress, lpBuffer, dwLength)
#define VirtualProtectEx NTAHookIntl_VirtualProtectEx
#define VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect) VirtualProtectEx(CURRENT_PROCESS_HANDLE, lpAddress, dwSize, flNewProtect, lpflOldProtect)
#define VirtualFree NTAHookIntl_VirtualFree
#define FlushInstructionCache NTAHookIntl_FlushInstructionCache
#define GetThreadContext NTAHookIntl_GetThreadContext
#define SetThreadContext NTAHookIntl_SetThreadContext
#define SuspendThread NTAHookIntl_SuspendThread
#define ResumeThread NTAHookIntl_ResumeThread
#define SetLastError EH_SetLastError
#define GetLastError EH_GetLastError
#define memset(Destination, Fill, Length) RtlFillMemory(Destination, Length, Fill) 

#define GetModuleHandleW(NULL) NT_GetImageBase()

_Ret_maybenull_ _Post_writable_byte_size_(dwSize)
static LPVOID WINAPI NTAHookIntl_VirtualAlloc(_In_opt_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD flAllocationType, _In_ DWORD flProtect)
{
    NTSTATUS lStatus = NtAllocateVirtualMemory(CURRENT_PROCESS_HANDLE, &lpAddress, 0, &dwSize, flAllocationType, flProtect);
    if (!NT_SUCCESS(lStatus)) {
        EH_SetLastNTError(lStatus);
        return NULL;
    }
    return lpAddress;
}

static SIZE_T WINAPI NTAHookIntl_VirtualQueryEx(_In_ HANDLE hProcess, _In_opt_ LPCVOID lpAddress, _Out_writes_bytes_to_(dwLength, return) PMEMORY_BASIC_INFORMATION lpBuffer, _In_ SIZE_T dwLength)
{
    NTSTATUS lStatus;
    SIZE_T ResultLength = 0;
    lStatus = NtQueryVirtualMemory(hProcess, (LPVOID)lpAddress, MemoryBasicInformation, lpBuffer, dwLength, &ResultLength);
    if (!NT_SUCCESS(lStatus)) {
        EH_SetLastNTError(lStatus);
    }
    return ResultLength;
}

_Success_(return != FALSE)
static BOOL WINAPI NTAHookIntl_VirtualProtectEx(_In_ HANDLE hProcess, _In_ LPVOID lpAddress, _In_  SIZE_T dwSize, _In_  DWORD flNewProtect, PDWORD lpflOldProtect)
{
    NTSTATUS lStatus = NtProtectVirtualMemory(hProcess, &lpAddress, &dwSize, flNewProtect, lpflOldProtect);
    if (!NT_SUCCESS(lStatus)) {
        EH_SetLastNTError(lStatus);
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
    static BOOL WINAPI NTAHookIntl_VirtualFree(LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD dwFreeType)
{
    NTSTATUS lStatus;
    if (!(dwSize) || !(dwFreeType & MEM_RELEASE)) {
        lStatus = NtFreeVirtualMemory(CURRENT_PROCESS_HANDLE, &lpAddress, &dwSize, dwFreeType);
        if (!NT_SUCCESS(lStatus)) {
            EH_SetLastNTError(lStatus);
            return FALSE;
        }
        return TRUE;
    }
    EH_SetLastNTError(STATUS_INVALID_PARAMETER);
    return FALSE;
}

static BOOL WINAPI NTAHookIntl_FlushInstructionCache(_In_ HANDLE hProcess, _In_reads_bytes_opt_(dwSize) LPCVOID lpBaseAddress, _In_ SIZE_T dwSize)
{
    NTSTATUS lStatus = NtFlushInstructionCache(hProcess, (PVOID)lpBaseAddress, dwSize);
    if (!NT_SUCCESS(lStatus)) {
        EH_SetLastNTError(lStatus);
        return FALSE;
    }
    return TRUE;
}

static BOOL WINAPI NTAHookIntl_GetThreadContext(_In_ HANDLE hThread, _Inout_ LPCONTEXT lpContext)
{
    NTSTATUS lStatus = NtGetContextThread(hThread, lpContext);
    if (!NT_SUCCESS(lStatus)) {
        EH_SetLastNTError(lStatus);
        return FALSE;
    }
    return TRUE;
}

static BOOL WINAPI NTAHookIntl_SetThreadContext(_In_ HANDLE hThread, _In_ CONST CONTEXT * lpContext)
{
    NTSTATUS lStatus = NtSetContextThread(hThread, (PCONTEXT)lpContext);
    if (!NT_SUCCESS(lStatus)) {
        EH_SetLastNTError(lStatus);
        return FALSE;
    }
    return TRUE;
}

static DWORD WINAPI NTAHookIntl_ResumeThread(_In_ HANDLE hThread)
{
    ULONG PreviousResumeCount;
    NTSTATUS lStatus = NtResumeThread(hThread, &PreviousResumeCount);
    if (!NT_SUCCESS(lStatus)) {
        EH_SetLastNTError(lStatus);
        return -1;
    }
    return PreviousResumeCount;
}

static DWORD WINAPI NTAHookIntl_SuspendThread(_In_ HANDLE hThread)
{
    ULONG PreviousSuspendCount;
    NTSTATUS lStatus = NtSuspendThread(hThread, &PreviousSuspendCount);
    if (!NT_SUCCESS(lStatus)) {
        EH_SetLastNTError(lStatus);
        return -1;
    }
    return PreviousSuspendCount;
}

int __cdecl _CrtDbgReport(
        _In_       int         _ReportType,
        _In_opt_z_ char const* _FileName,
        _In_       int         _Linenumber,
        _In_opt_z_ char const* _ModuleName,
        _In_opt_z_ char const* _Format,
        ...)
{
    return 0;
}

EXTERN_C_END

void* __cdecl operator new(size_t _Size)
{
    return Mem_Alloc(_Size);
}
void __cdecl operator delete(void* _P, size_t _Size) noexcept
{
    Mem_Free(_P);
}

// Hook by using Detours

// Rewrite detours functions
EXTERN_C ULONG WINAPI DetourGetModuleSize(_In_opt_ HMODULE hModule)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    if (hModule == NULL) {
        pDosHeader = (PIMAGE_DOS_HEADER)GetModuleHandleW(NULL);
    }
    return MOVE_PTR(pDosHeader, pDosHeader->e_lfanew, IMAGE_NT_HEADERS)->OptionalHeader.SizeOfImage;
}



#pragma warning(disable: 26495)
#include "3rdParty\src\Detours\detours.cpp"
#include "3rdParty\src\Detours\disasm.cpp"
#pragma warning(default: 26495)

#pragma comment(linker, "/MERGE:.detourd=.data")
#pragma comment(linker, "/MERGE:.detourc=.rdata")
