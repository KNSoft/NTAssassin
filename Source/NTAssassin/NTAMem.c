#include "NTAssassin.h"

PVOID NTAPI Mem_PageAlloc(SIZE_T Size, ULONG Protect) {
    PVOID   lpBase = NULL;
    SIZE_T  sSize = Size;
    NtAllocateVirtualMemory(CURRENT_PROCESS_HANDLE, &lpBase, 0, &sSize, MEM_COMMIT | MEM_RESERVE, Protect);
    return lpBase;
}

NTSTATUS NTAPI Mem_PageProt(PVOID BaseAddress, SIZE_T Size, ULONG Protect, PULONG OldProtect) {
    PVOID   lpBase = BaseAddress;
    SIZE_T  sSize = Size;
    return NtProtectVirtualMemory(CURRENT_PROCESS_HANDLE, &lpBase, &sSize, Protect, OldProtect);
}

NTSTATUS NTAPI Mem_PageFree(PVOID BaseAddress, SIZE_T Size) {
    PVOID   lpBase = BaseAddress;
    SIZE_T  uSize = Size;
    return NtFreeVirtualMemory(CURRENT_PROCESS_HANDLE, &lpBase, &uSize, MEM_RELEASE);
}