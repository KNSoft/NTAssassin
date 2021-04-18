#include "NTAssassin.h" 

// uGroupCount, uSize, lpst1, u1, lpst2, u2, ...
PVOID WINAPIV Data_StructCombineEx(UINT uGroupCount, UINT uSize, ...) {
    va_list args;
    UINT    i, u, uStructCount = 0;
    PVOID   lpBuffer, lpDst, lpSrc;
    va_start(args, uSize);
    for (i = 0; i < uGroupCount; i++) {
        lpSrc = va_arg(args, PVOID);
        u = va_arg(args, UINT);
        if (lpSrc)
            uStructCount += u;
    }
    lpBuffer = Mem_HeapAlloc(uStructCount * (SIZE_T)uSize);
    if (lpBuffer) {
        lpDst = lpBuffer;
        va_start(args, uSize);
        for (i = 0; i < uGroupCount; i++) {
            lpSrc = va_arg(args, PVOID);
            uStructCount = va_arg(args, UINT);
            if (lpSrc) {
                RtlCopyMemory(lpDst, lpSrc, uStructCount * (SIZE_T)uSize);
                lpDst = ADD_OFFSET(lpDst, uStructCount * (SIZE_T)uSize, VOID);
            }
        }
    }
    return lpBuffer;
}