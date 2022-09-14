#include "include\NTAssassin\NTAMath.h"

static ULONG ulRandSeed = 0;

ULONG NTAPI Math_Random() {
    return ulRandSeed = RtlRandomEx(&ulRandSeed);
}

ULONG NTAPI Math_RangedRandom(ULONG Min, ULONG Max) {
    ULONG ulRand = Math_Random();
    return Min + ulRand % (Max - Min + 1);
}
