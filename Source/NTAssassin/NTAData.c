#include "include\NTAssassin\NTAssassin.h"

_Check_return_ PVOID WINAPIV Data_StructCombineEx(_In_ UINT GroupCount, _In_ UINT Size, ...) {
    va_list args;
    SIZE_T  i, u, uStructCount = 0;
    PVOID   pBuffer, pDst, pSrc;
    va_start(args, Size);
    for (i = 0; i < GroupCount; i++) {
        pSrc = va_arg(args, PVOID);
        u = va_arg(args, UINT);
        if (pSrc) {
            uStructCount += u;
        }
    }
    pBuffer = Mem_HeapAlloc(uStructCount * Size);
    if (pBuffer) {
        pDst = pBuffer;
        va_start(args, Size);
        for (i = 0; i < GroupCount; i++) {
            pSrc = va_arg(args, PVOID);
            uStructCount = va_arg(args, UINT);
            if (pSrc) {
                RtlMoveMemory(pDst, pSrc, uStructCount * Size);
                pDst = MOVE_PTR(pDst, uStructCount * Size, VOID);
            }
        }
    }
    return pBuffer;
}