#include "Include\NTAMem.h"

#include "Include\NTAEH.h"

_Must_inspect_result_
__drv_allocatesMem(Mem)
_Post_writable_byte_size_(Size)
PVOID NTAPI Mem_PageAlloc(_In_ SIZE_T Size, _In_ ULONG Protect)
{
    PVOID       pBase = NULL;
    SIZE_T      sSize = Size;
    NTSTATUS    lStatus = NtAllocateVirtualMemory(CURRENT_PROCESS_HANDLE, &pBase, 0, &sSize, MEM_COMMIT | MEM_RESERVE, Protect);
    if (NT_SUCCESS(lStatus))
    {
        return pBase;
    } else
    {
        WIE_SetLastStatus(lStatus);
        return NULL;
    }
}

BOOL NTAPI Mem_PageProt(_In_ PVOID BaseAddress, _In_ SIZE_T Size, ULONG Protect, _Out_ PULONG OldProtect)
{
    PVOID       pBase = BaseAddress;
    SIZE_T      sSize = Size;
    NTSTATUS    lStatus = NtProtectVirtualMemory(CURRENT_PROCESS_HANDLE, &pBase, &sSize, Protect, OldProtect);
    if (NT_SUCCESS(lStatus))
    {
        return TRUE;
    } else
    {
        WIE_SetLastStatus(lStatus);
        return FALSE;
    }
}

BOOL NTAPI Mem_PageFree(__drv_freesMem(Mem) _Frees_ptr_ _In_ PVOID BaseAddress)
{
    PVOID       pBase = BaseAddress;
    SIZE_T      uSize = 0;
    NTSTATUS    lStatus = NtFreeVirtualMemory(CURRENT_PROCESS_HANDLE, &pBase, &uSize, MEM_RELEASE);
    if (NT_SUCCESS(lStatus))
    {
        return TRUE;
    } else
    {
        WIE_SetLastStatus(lStatus);
        return FALSE;
    }
}
