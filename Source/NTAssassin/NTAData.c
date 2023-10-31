#include "Include\NTAData.h"

#include "Include\NTAMem.h"

_Check_return_ PVOID WINAPIV Data_StructCombineEx(_In_ UINT GroupCount, _In_ UINT Size, ...)
{
    va_list args;
    SIZE_T  i, u, uStructCount = 0;
    PVOID   pBuffer, pDst, pSrc;
    va_start(args, Size);
    for (i = 0; i < GroupCount; i++)
    {
        pSrc = va_arg(args, PVOID);
        u = va_arg(args, UINT);
        if (pSrc)
        {
            uStructCount += u;
        }
    }
    pBuffer = Mem_Alloc(uStructCount * Size);
    if (pBuffer)
    {
        pDst = pBuffer;
        va_start(args, Size);
        for (i = 0; i < GroupCount; i++)
        {
            pSrc = va_arg(args, PVOID);
            uStructCount = va_arg(args, UINT);
            if (pSrc)
            {
                RtlCopyMemory(pDst, pSrc, uStructCount * Size);
                pDst = Add2Ptr(pDst, uStructCount * Size);
            }
        }
    }
    return pBuffer;
}
