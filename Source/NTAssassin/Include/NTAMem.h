#pragma once

#include "NTAssassin_Base.h"

/// <summary>
/// Allocates heap memory
/// </summary>
/// <seealso cref="RtlAllocateHeap"/>
/// <returns>Both of last error and last status will set if failed</returns>
#define Mem_HeapAllocEx(Flags, Size) RtlAllocateHeap(CURRENT_PROCESS_HEAP, Flags, Size)
#define Mem_HeapAlloc(Size) RtlAllocateHeap(CURRENT_PROCESS_HEAP, 0, Size)

/// <summary>
/// Frees heap memory
/// </summary>
/// <seealso cref="RtlFreeHeap"/>
#define Mem_HeapFree(BaseAddress) RtlFreeHeap(CURRENT_PROCESS_HEAP, 0, BaseAddress)

/// <summary>
/// Allocates memory pages
/// </summary>
/// <seealso cref="VirtualAlloc"/>
/// <return>Error code storaged in last STATUS</return>
NTA_API
_Must_inspect_result_
__drv_allocatesMem(Mem)
_Post_writable_byte_size_(Size)
PVOID NTAPI Mem_PageAlloc(_In_ SIZE_T Size, _In_ ULONG Protect);

/// <summary>
/// Changes the protection of memory pages
/// </summary>
/// <seealso cref="VirtualProtect"/>
/// <return>Error code storaged in last STATUS</return>
NTA_API BOOL NTAPI Mem_PageProt(_In_ PVOID BaseAddress, _In_ SIZE_T Size, ULONG Protect, _Out_ PULONG OldProtect);

/// <summary>
/// Frees memory pages
/// </summary>
/// <seealso cref="VirtualFree"/>
/// <return>Error code storaged in last STATUS</return>
NTA_API BOOL NTAPI Mem_PageFree(__drv_freesMem(Mem) _Frees_ptr_ _In_ PVOID BaseAddress);

// Alias of "Mem_HeapAlloc"
#define Mem_Alloc Mem_HeapAlloc

// Alias of "Mem_HeapFree"
#define Mem_Free Mem_HeapFree
