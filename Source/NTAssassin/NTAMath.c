#include "include\NTAssassin\NTAssassin.h"

ULONG ulRandSeed = 0;

ULONG NTAPI Math_Random() {
    return ulRandSeed = RtlRandomEx(&ulRandSeed);
}

ULONG NTAPI Math_RangedRandom(ULONG Min, ULONG Max) {
    ULONG ulRand = Math_Random();
    return Min + ulRand % (Max - Min + 1);
}

static CHAR arrSizeUnitPrefix[] = {
    '\0', 'K', 'M', 'G', 'T', 'P', 'E' // , 'Z', 'Y'
};

DOUBLE NTAPI Math_SimplifySize(UINT64 Size, PCHAR Unit) {
    UINT64 uTotal = Size, uDivisor, uDivisorTemp = 1;
    UINT uTimes = 0;
    do {
        uDivisor = uDivisorTemp;
        uDivisorTemp = uDivisorTemp << 10;
        uTimes++;
    } while (uTotal >= uDivisorTemp && uDivisorTemp > uDivisor);
    if (Unit) {
        *Unit = arrSizeUnitPrefix[uTimes - 1];
    }
    return uTotal / (DOUBLE)uDivisor;
}