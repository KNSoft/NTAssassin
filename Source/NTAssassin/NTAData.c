#include "NTAssassin\NTAssassin.h" 

// uGroupCount, uSize, lpst1, u1, lpst2, u2, ...
PVOID WINAPIV Data_StructCombineEx(UINT GroupCount, UINT Size, ...) {
    va_list args;
    UINT    i, u, uStructCount = 0;
    PVOID   lpBuffer, lpDst, lpSrc;
    va_start(args, Size);
    for (i = 0; i < GroupCount; i++) {
        lpSrc = va_arg(args, PVOID);
        u = va_arg(args, UINT);
        if (lpSrc)
            uStructCount += u;
    }
    lpBuffer = Mem_HeapAlloc(uStructCount * (SIZE_T)Size);
    if (lpBuffer) {
        lpDst = lpBuffer;
        va_start(args, Size);
        for (i = 0; i < GroupCount; i++) {
            lpSrc = va_arg(args, PVOID);
            uStructCount = va_arg(args, UINT);
            if (lpSrc) {
                RtlMoveMemory(lpDst, lpSrc, uStructCount * (SIZE_T)Size);
                lpDst = MOVE_PTR(lpDst, uStructCount * (SIZE_T)Size, VOID);
            }
        }
    }
    return lpBuffer;
}