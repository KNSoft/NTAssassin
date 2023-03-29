#pragma once

#include "WIE_ntdef.h"

#define Add2Ptr(P,I) ((PVOID)((PUCHAR)(P) + (I)))
#define PtrOffset(B,O) ((ULONG)((ULONG_PTR)(O) - (ULONG_PTR)(B)))

#define ROUND_TO_SIZE(_length, _alignment)                      \
            ((((ULONG_PTR)(_length)) + ((_alignment)-1)) & ~(ULONG_PTR) ((_alignment) - 1))

#define IS_ALIGNED(_pointer, _alignment)                        \
        ((((ULONG_PTR) (_pointer)) & ((_alignment) - 1)) == 0)
