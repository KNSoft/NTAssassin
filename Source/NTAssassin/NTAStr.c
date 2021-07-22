#include "NTAssassin\NTAssassin.h"

UINT NTAPI Str_CchLenExW(LPCWSTR psz, UINT cchMax) {
    UINT i = 0;
    while (i < cchMax) {
        if (psz[i] == '\0')
            return i;
        i++;
    }
    return 0;
}

UINT NTAPI Str_CchLenExA(LPCSTR psz, UINT cchMax) {
    UINT i = 0;
    while (i < cchMax) {
        if (psz[i] == '\0')
            return i;
        i++;
    }
    return 0;
}

UINT NTAPI Str_CcbLenExW(LPCWSTR psz, UINT ccbMax) {
    UINT i = 0;
    while (i < ccbMax / sizeof(WCHAR)) {
        if (psz[i] == '\0')
            return i * sizeof(WCHAR);
        i++;
    }
    return 0;
}

UINT NTAPI Str_CchCopyExW(LPWSTR pszDest, UINT cchDest, LPCWSTR pszSrc) {
    UINT i = 0;
    while (i < cchDest) {
        if ((pszDest[i] = pszSrc[i]) == '\0')
            return i;
        i++;
    }
    return 0;
}

UINT NTAPI Str_CchCopyExA(LPSTR pszDest, UINT cchDest, LPCSTR pszSrc) {
    UINT i = 0;
    while (i < cchDest) {
        if ((pszDest[i] = pszSrc[i]) == '\0')
            return i;
        i++;
    }
    return 0;
}

UINT NTAPI Str_CcbCopyExW(LPWSTR pszDest, UINT ccbDest, LPCWSTR pszSrc) {
    UINT i = 0;
    while (i < ccbDest / sizeof(WCHAR)) {
        if ((pszDest[i] = pszSrc[i]) == '\0')
            return i * sizeof(WCHAR);
        i++;
    }
    return 0;
}

BOOL NTAPI Str_CchEqualExW(LPCWSTR psz1, UINT cch1, LPCWSTR psz2, UINT cch2) {
    UINT i = 0;
    while (i < cch1 && i < cch2) {
        if (psz1[i] != psz2[i])
            return FALSE;
        else if (psz1[i] == '\0')
            return TRUE;
        i++;
    }
    return FALSE;
}

BOOL NTAPI Str_CcbEqualICExW(LPCWSTR psz1, UINT ccb1, LPCWSTR psz2, UINT ccb2) {
    UINT i = 0;
    while (i < ccb1 / sizeof(WCHAR) && i < ccb2 / sizeof(WCHAR)) {
        if (psz1[i] != psz2[i]) {
            if ((psz1[i] >= 'a' && psz1[i] <= 'z' && psz1[i] - ('a' - 'A') != psz2[i]) ||
                (psz1[i] >= 'A' && psz1[i] <= 'Z' && psz1[i] + ('a' - 'A') != psz2[i])
                )
                return FALSE;
        } else if (psz1[i] == '\0')
            return TRUE;
        i++;
    }
    return FALSE;
}

// Algorithm: BF
UINT NTAPI Str_CchIndex_BFExW(LPCWSTR lpszSrc, UINT cchMaxSrc, LPCWSTR lpszPattern, UINT cchMaxPattern) {
    UINT    i;
    LPCWSTR pszSrc;
    BOOL    bMatched;
    for (pszSrc = lpszSrc; pszSrc < lpszSrc + cchMaxSrc; pszSrc++) {
        bMatched = TRUE;
        for (i = 0; i < cchMaxPattern && pszSrc + i < lpszSrc + cchMaxSrc; i++) {
            if (pszSrc[i] != lpszPattern[i]) {
                if (pszSrc[i] == '\0')
                    return -1;
                else if (lpszPattern[i] == '\0')
                    return i == 0 ? -1 : (UINT)(pszSrc - lpszSrc);
                else {
                    bMatched = FALSE;
                    break;
                }
            } else if (pszSrc[i] == '\0')
                return (UINT)(pszSrc - lpszSrc);
        }
        if (bMatched)
            return -1;
    }
    return -1;
}

BOOL NTAPI Str_CchEqualExA(LPCSTR psz1, UINT cch1, LPCSTR psz2, UINT cch2) {
    UINT i = 0;
    while (i < cch1 && i < cch2) {
        if (psz1[i] != psz2[i])
            return FALSE;
        else if (psz1[i] == '\0')
            return TRUE;
        i++;
    }
    return FALSE;
}

UINT NTAPI Str_CchU2AEx(LPSTR lpszANSI, UINT cchANSIMax, LPCWSTR lpszUnicode) {
    ULONG ulANSIBytes = 0;
    RtlUnicodeToMultiByteN(lpszANSI, cchANSIMax * sizeof(CHAR), &ulANSIBytes, lpszUnicode, Str_CcbLenExW(lpszUnicode, STRSAFE_MAX_CCH * sizeof(WCHAR)) + 1);
    return ulANSIBytes / sizeof(CHAR);
}

UINT NTAPI Str_CchA2UEx(_Out_writes_(cchUnicodeMax) LPWSTR lpszUnicode, _In_ UINT cchUnicodeMax, LPCSTR lpszANSI) {
    ULONG ulUnicodeBytes = 0;
    RtlMultiByteToUnicodeN(lpszUnicode, cchUnicodeMax * sizeof(WCHAR), &ulUnicodeBytes, lpszANSI, Str_CcbLenExA(lpszANSI, STRSAFE_MAX_CCH * sizeof(CHAR)) + 1);
    return ulUnicodeBytes / sizeof(WCHAR);
}

UINT NTAPI Str_CcbA2UEx(_Out_writes_bytes_(ccbUnicodeMax) LPWSTR lpszUnicode, _In_ UINT ccbUnicodeMax, LPCSTR lpszANSI) {
    ULONG ulUnicodeBytes = 0;
    RtlMultiByteToUnicodeN(lpszUnicode, ccbUnicodeMax, &ulUnicodeBytes, lpszANSI, Str_CcbLenExA(lpszANSI, STRSAFE_MAX_CCH * sizeof(CHAR)) + 1);
    return ulUnicodeBytes;
}

VOID NTAPI Str_CchInitExW(PUNICODE_STRING lpstStr, LPWSTR lpStr, UINT cchMax) {
    UINT uLen = Str_CcbLenExW(lpStr, cchMax * sizeof(WCHAR));
    lpstStr->Length = uLen;
    lpstStr->MaximumLength = uLen + (UINT)sizeof(WCHAR);
    lpstStr->Buffer = lpStr;
}

VOID NTAPI Str_CchInitExA(PSTRING lpstStr, LPSTR lpStr, UINT cchMax) {
    UINT uLen = Str_CcbLenExA(lpStr, cchMax);
    lpstStr->Length = uLen;
    lpstStr->MaximumLength = uLen + 1;
    lpstStr->Buffer = lpStr;
}

BOOL NTAPI Str_ToIntExW(PCWSTR StrValue, BOOL Unsigned, UINT Base, PVOID Value, SIZE_T ValueSize) {
    PCWSTR  psz = StrValue;

    // Minus
    BOOL    bMinus;
    if (*psz == '\0')
        return FALSE;
    if (*psz == '-') {
        if (!Unsigned) {
            psz++;
            bMinus = TRUE;
        } else
            return FALSE;
    } else if (*psz == '+') {
        psz++;
        bMinus = FALSE;
    } else
        bMinus = FALSE;

    // Base
    UINT    uBase;
    if (Base == 0) {
        if (*psz == '\0')
            return FALSE;
        if (*psz == '0') {
            psz++;
            if (*psz == '\0')
                return FALSE;
            if (*psz == 'b') {
                psz++;
                uBase = 2;
            } else if (*psz == 'o') {
                psz++;
                uBase = 8;
            } else if (*psz == 'x') {
                psz++;
                uBase = 16;
            } else
                uBase = 10;
        } else
            uBase = 10;
    } else if (Base == 2 || Base == 8 || Base == 16) {
        uBase = Base;
    } else
        return FALSE;

    // Overflow limitation
    UINT64  uMax;
    if (ValueSize == sizeof(UINT8))
        uMax = Unsigned ? MAXUINT8 : MAXINT8;
    else if (ValueSize == sizeof(UINT16))
        uMax = Unsigned ? MAXUINT16 : MAXINT16;
    else if (ValueSize == sizeof(UINT32))
        uMax = Unsigned ? MAXUINT32 : MAXINT32;
    else if (ValueSize == sizeof(UINT64))
        uMax = Unsigned ? 0 : MAXINT64;
    else
        return FALSE;

    // Convert
    WCHAR   wc;
    USHORT  uc;
    UINT64  uTotal = 0, uTemp;
    if (uBase == 2) {
        while ((wc = *psz++) != '\0') {
            if (wc == '0') {
                uTemp = uTotal << 1;
            } else if (wc == '1') {
                uTemp = (uTotal << 1) | 1;
            } else
                return FALSE;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    } else if (uBase == 8) {
        while ((wc = *psz++) != '\0') {
            if (wc >= '0' && wc <= '7') {
                uc = wc - '0';
            } else
                return FALSE;
            uTemp = (uTotal << 3) + uc;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    } else if (uBase == 10) {
        while ((wc = *psz++) != '\0') {
            if (wc >= '0' && wc <= '9') {
                uc = wc - '0';
            } else
                return FALSE;
            uTemp = uTotal * 10 + uc;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    } else if (uBase == 16) {
        while ((wc = *psz++) != '\0') {
            if (wc >= '0' && wc <= '9') {
                uc = wc - '0';
            } else if (wc >= 'A' && wc <= 'F') {
                uc = wc - 'A' + 10;
            } else if (wc >= 'a' && wc <= 'f') {
                uc = wc - 'a' + 10;
            } else
                return FALSE;
            uTemp = (uTotal << 4) + uc;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    }

    // Output
    if (bMinus)
        (INT64)uTotal = -(INT64)uTotal;
    if (ValueSize == sizeof(UINT8))
        *((PUINT8)Value) = (UINT8)uTotal;
    else if (ValueSize == sizeof(UINT16))
        *((PUINT16)Value) = (UINT16)uTotal;
    else if (ValueSize == sizeof(UINT32))
        *((PUINT32)Value) = (UINT32)uTotal;
    else if (ValueSize == sizeof(UINT64))
        *((PUINT64)Value) = (UINT64)uTotal;
    else
        return FALSE;

    return TRUE;
}

BOOL NTAPI Str_ToIntExA(PCSTR StrValue, BOOL Unsigned, UINT Base, PVOID Value, SIZE_T ValueSize) {
    PCSTR  psz = StrValue;

    // Minus
    BOOL    bMinus;
    if (*psz == '\0')
        return FALSE;
    if (*psz == '-') {
        if (!Unsigned) {
            psz++;
            bMinus = TRUE;
        } else
            return FALSE;
    } else if (*psz == '+') {
        psz++;
        bMinus = FALSE;
    } else
        bMinus = FALSE;

    // Base
    UINT    uBase;
    if (Base == 0) {
        if (*psz == '\0')
            return FALSE;
        if (*psz == '0') {
            psz++;
            if (*psz == '\0')
                return FALSE;
            if (*psz == 'b') {
                psz++;
                uBase = 2;
            } else if (*psz == 'o') {
                psz++;
                uBase = 8;
            } else if (*psz == 'x') {
                psz++;
                uBase = 16;
            } else
                uBase = 10;
        } else
            uBase = 10;
    } else if (Base == 2 || Base == 8 || Base == 16) {
        uBase = Base;
    } else
        return FALSE;

    // Overflow limitation
    UINT64  uMax;
    if (ValueSize == sizeof(UINT8))
        uMax = Unsigned ? MAXUINT8 : MAXINT8;
    else if (ValueSize == sizeof(UINT16))
        uMax = Unsigned ? MAXUINT16 : MAXINT16;
    else if (ValueSize == sizeof(UINT32))
        uMax = Unsigned ? MAXUINT32 : MAXINT32;
    else if (ValueSize == sizeof(UINT64))
        uMax = Unsigned ? 0 : MAXINT64;
    else
        return FALSE;

    // Convert
    CHAR    ch;
    USHORT  uc;
    UINT64  uTotal = 0, uTemp;
    if (uBase == 2) {
        while ((ch = *psz++) != '\0') {
            if (ch == '0') {
                uTemp = uTotal << 1;
            } else if (ch == '1') {
                uTemp = (uTotal << 1) | 1;
            } else
                return FALSE;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    } else if (uBase == 8) {
        while ((ch = *psz++) != '\0') {
            if (ch >= '0' && ch <= '7') {
                uc = ch - '0';
            } else
                return FALSE;
            uTemp = (uTotal << 3) + uc;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    } else if (uBase == 10) {
        while ((ch = *psz++) != '\0') {
            if (ch >= '0' && ch <= '9') {
                uc = ch - '0';
            } else
                return FALSE;
            uTemp = uTotal * 10 + uc;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    } else if (uBase == 16) {
        while ((ch = *psz++) != '\0') {
            if (ch >= '0' && ch <= '9') {
                uc = ch - '0';
            } else if (ch >= 'A' && ch <= 'F') {
                uc = ch - 'A' + 10;
            } else if (ch >= 'a' && ch <= 'f') {
                uc = ch - 'a' + 10;
            } else
                return FALSE;
            uTemp = (uTotal << 4) + uc;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    }

    // Output
    if (bMinus)
        (INT64)uTotal = -(INT64)uTotal;
    if (ValueSize == sizeof(UINT8))
        *((PUINT8)Value) = (UINT8)uTotal;
    else if (ValueSize == sizeof(UINT16))
        *((PUINT16)Value) = (UINT16)uTotal;
    else if (ValueSize == sizeof(UINT32))
        *((PUINT32)Value) = (UINT32)uTotal;
    else if (ValueSize == sizeof(UINT64))
        *((PUINT64)Value) = (UINT64)uTotal;
    else
        return FALSE;

    return TRUE;
}

BOOL NTAPI Str_HexTo32ExW(LPCWSTR lpsz, UINT cchMax, PDWORD lpdwOut) {
    UINT    i = 0;
    DWORD   dwOut = 0;
    WCHAR   ch;
    UCHAR   uc;
    while ((ch = lpsz[i]) != '\0') {
        if (i >= cchMax || i >= sizeof(DWORD) * 2)
            return FALSE;
        if (ch >= 'a' && ch <= 'f')
            uc = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')
            uc = ch - 'A' + 10;
        else if (ch >= '0' && ch <= '9')
            uc = ch - '0';
        else
            return FALSE;
        dwOut = (dwOut << 4) | uc;
        i++;
    }
    *lpdwOut = dwOut;
    return TRUE;
}

BOOL NTAPI Str_HexTo32ExA(LPCSTR lpsz, UINT cchMax, PDWORD lpdwOut) {
    UINT    i = 0;
    DWORD   dwOut = 0;
    CHAR    ch;
    UCHAR   uc;
    while ((ch = lpsz[i]) != '\0') {
        if (i >= cchMax || i >= sizeof(DWORD) * 2)
            return FALSE;
        if (ch >= 'a' && ch <= 'f')
            uc = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')
            uc = ch - 'A' + 10;
        else if (ch >= '0' && ch <= '9')
            uc = ch - '0';
        else
            return FALSE;
        dwOut = (dwOut << 4) | uc;
        i++;
    }
    *lpdwOut = dwOut;
    return TRUE;
}

BOOL NTAPI Str_16ToUDecExW(USHORT uNum, LPWSTR lpszOutput, UINT cchMax) {
    LPWSTR  lpsz = lpszOutput;
    USHORT  uRemain = uNum;
    USHORT  uQuotient;
    USHORT  uDivisor;
    ULONG   uTemp = 1;
    BOOL    bWritten = FALSE;
    if (cchMax == 0)
        return FALSE;
    do {
        uDivisor = LOWORD(uTemp);
        uTemp *= 10;
    } while (uTemp < uRemain);
    do {
        if (lpsz >= lpszOutput + cchMax) {
            *(lpsz - 1) = '\0';
            return FALSE;
        }
        uQuotient = uRemain / uDivisor;
        uRemain %= uDivisor;
        if (bWritten || uRemain != 0) {
            *lpsz++ = uQuotient + '0';
            bWritten = TRUE;
        }
        uDivisor /= 10;
    } while (uDivisor > 0);
    *lpsz = '\0';
    return TRUE;
}

BOOL NTAPI Str_RGBToHexExW(COLORREF Color, PWSTR HexRGB, UINT MaxCh) {
    PWSTR   psz;
    WCHAR   ch;
    UINT    uCh = 6;
    DWORD   dwColor = Color;
    if (MaxCh < HEXRGB_CCH)
        return FALSE;
    psz = HexRGB;
    *psz++ = '#';
    psz[uCh] = '\0';
    do {
        ch = (WCHAR)(dwColor & 0xF);
        ch += ch <= 9 ? '0' : ('A' - 0xA);
        psz[--uCh] = ch;
        dwColor >>= 4;
    } while (uCh);
    return TRUE;
}

/*
    Unicode             | UTF-8
    ---------------------------------------------------------
    0000 0000-0000 007F | 0xxxxxxx
    0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
NTSTATUS NTAPI Str_UnicodeToUTF8Ex(LPSTR pszDest, UINT cchDest, LPCWSTR pszSrc, PULONG pulChWritten) {
    NTSTATUS    lStatus = STATUS_SUCCESS;
    UINT        cChDest = 0, cChSrc = 0;
    LPSTR       lpsz = pszDest;
    ULONG       ch, ulChLength;
    while (TRUE) {
        ch = pszSrc[cChSrc];
        if (ch == '\0') {
        Label_Terminate:
            lpsz[0] = '\0';
            break;
        }
        // Surrogate Pair
        if (ch >= 0xD800 && ch <= 0xDBFF && pszSrc[cChSrc + 1] != '\0' && pszSrc[cChSrc + 1] >= 0xDC00 && pszSrc[cChSrc + 1] <= 0xDFFF) {
            ch = ((ch - 0xD800) << 10 | (pszSrc[cChSrc + 1] - 0xDC00)) + 0x010000;
            cChSrc++;
            goto Label_Encode;
        } else if (ch < 0xDC00 || ch > 0xDFFF)
            goto Label_Encode;
        // Invalid character
        ch = 0xFFFD;
        lStatus = STATUS_SOME_NOT_MAPPED;
    // Encode, ch always <= 0x10FFFF (MAX_UCSCHAR)
    Label_Encode:
        if (ch <= 0x7F) {
            ulChLength = 1;
            if (lpsz - pszDest + ulChLength >= cchDest)
                goto Label_Terminate;
            lpsz[0] = ch & 0b01111111;
        } else if (ch <= 0x7FF) {
            ulChLength = 2;
            if (lpsz - pszDest + ulChLength >= cchDest)
                goto Label_Terminate;
            lpsz[0] = 0b11000000 | (ch >> 6 & 0b00011111);
            lpsz[1] = 0b10000000 | (ch >> 0 & 0b00111111);
        } else if (ch <= 0xFFFF) {
            ulChLength = 3;
            if (lpsz - pszDest + ulChLength >= cchDest)
                goto Label_Terminate;
            lpsz[0] = 0b11100000 | (ch >> 12 & 0b00001111);
            lpsz[1] = 0b10000000 | (ch >> 6 & 0b00111111);
            lpsz[2] = 0b10000000 | (ch >> 0 & 0b00111111);
        } else {
            ulChLength = 4;
            if (lpsz - pszDest + ulChLength >= cchDest)
                goto Label_Terminate;
            lpsz[0] = 0b11110000 | (ch >> 18 & 0b00000111);
            lpsz[1] = 0b10000000 | (ch >> 12 & 0b00111111);
            lpsz[2] = 0b10000000 | (ch >> 6 & 0b00111111);
            lpsz[3] = 0b10000000 | (ch >> 0 & 0b00111111);
        }
        lpsz += ulChLength;
        pszSrc++;
    }
    // Output number of characters written and exit
    if (pulChWritten)
        *pulChWritten = (ULONG)(lpsz - pszDest);
    return lStatus;
}

DWORD NTAPI Str_HashExW(LPCWSTR psz, UINT cchMax, STR_HASH_ALGORITHM HashAlgorithm) {
    LPCWSTR lpsz = psz, lpszMax = psz + cchMax;
    DWORD   dwHash = 0;
    if (HashAlgorithm == StrHashAlgorithmSDBM) {
        while (lpsz < lpszMax && *lpsz != '\0')
            dwHash = 65599 * dwHash + *lpsz++;
    } else if (HashAlgorithm == StrHashAlgorithmBKDR) {
        while (lpsz < lpszMax && *lpsz != '\0')
            dwHash = 31 * dwHash + *lpsz++;
    } else if (HashAlgorithm == StrHashAlgorithmAP) {
        BOOL    bOdd = FALSE;
        while (lpsz < lpszMax && *lpsz != '\0') {
            dwHash ^= bOdd ? (~(dwHash << 11)) ^ (*lpsz++) ^ (dwHash >> 5) : (dwHash << 7) ^ (*lpsz++) ^ (dwHash >> 3);
            bOdd ^= TRUE;
        }
    } else if (HashAlgorithm == StrHashAlgorithmDJB) {
        dwHash = 5381;
        while (lpsz < lpszMax && *lpsz != '\0')
            dwHash += (dwHash << 5) + *lpsz++;
    } else if (HashAlgorithm == StrHashAlgorithmJS) {
        dwHash = 1315423911;
        while (lpsz < lpszMax && *lpsz != '\0')
            dwHash ^= (dwHash << 5) + (*lpsz++) + (dwHash >> 2);
    } else if (HashAlgorithm == StrHashAlgorithmRS) {
        DWORD   x = 63689;
        while (lpsz < lpszMax && *lpsz != '\0') {
            dwHash = dwHash * x + *lpsz++;
            x *= 378551;
        }
    } else if (HashAlgorithm == StrHashAlgorithmELF) {
        DWORD   x = 0;
        while (lpsz < lpszMax && *lpsz != '\0') {
            dwHash = (dwHash << 4) + *lpsz++;
            if ((x = dwHash & 0xF0000000) != 0)
                dwHash = (dwHash ^ (x >> 24)) & ~x;
        }
    } else if (HashAlgorithm == StrHashAlgorithmPJW) {
        DWORD   x = 0;
        while (lpsz < lpszMax && *lpsz != '\0') {
            dwHash = (dwHash << 4) + *lpsz++;
            if ((x = dwHash & 0xF0000000) != 0)
                dwHash = (dwHash ^ (x >> 24)) & 0x0FFFFFFF;
        }
    }
    return dwHash;
}

DWORD NTAPI Str_HashExA(LPCSTR psz, UINT cchMax, STR_HASH_ALGORITHM HashAlgorithm) {
    LPCSTR  lpsz = psz, lpszMax = psz + cchMax;
    DWORD   dwHash = 0;
    if (HashAlgorithm == StrHashAlgorithmSDBM) {
        while (lpsz < lpszMax && *lpsz != '\0')
            dwHash = 65599 * dwHash + *lpsz++;
    } else if (HashAlgorithm == StrHashAlgorithmBKDR) {
        while (lpsz < lpszMax && *lpsz != '\0')
            dwHash = 31 * dwHash + *lpsz++;
    } else if (HashAlgorithm == StrHashAlgorithmAP) {
        BOOL    bOdd = FALSE;
        while (lpsz < lpszMax && *lpsz != '\0') {
            dwHash ^= bOdd ? (~(dwHash << 11)) ^ (*lpsz++) ^ (dwHash >> 5) : (dwHash << 7) ^ (*lpsz++) ^ (dwHash >> 3);
            bOdd ^= TRUE;
        }
    } else if (HashAlgorithm == StrHashAlgorithmDJB) {
        dwHash = 5381;
        while (lpsz < lpszMax && *lpsz != '\0')
            dwHash += (dwHash << 5) + *lpsz++;
    } else if (HashAlgorithm == StrHashAlgorithmJS) {
        dwHash = 1315423911;
        while (lpsz < lpszMax && *lpsz != '\0')
            dwHash ^= (dwHash << 5) + (*lpsz++) + (dwHash >> 2);
    } else if (HashAlgorithm == StrHashAlgorithmRS) {
        DWORD   x = 63689;
        while (lpsz < lpszMax && *lpsz != '\0') {
            dwHash = dwHash * x + *lpsz++;
            x *= 378551;
        }
    } else if (HashAlgorithm == StrHashAlgorithmELF) {
        DWORD   x = 0;
        while (lpsz < lpszMax && *lpsz != '\0') {
            dwHash = (dwHash << 4) + *lpsz++;
            if ((x = dwHash & 0xF0000000) != 0)
                dwHash = (dwHash ^ (x >> 24)) & ~x;
        }
    } else if (HashAlgorithm == StrHashAlgorithmPJW) {
        DWORD   x = 0;
        while (lpsz < lpszMax && *lpsz != '\0') {
            dwHash = (dwHash << 4) + *lpsz++;
            if ((x = dwHash & 0xF0000000) != 0)
                dwHash = (dwHash ^ (x >> 24)) & 0x0FFFFFFF;
        }
    }
    return dwHash;
}

VOID NTAPI Str_UpperW(PWSTR psz) {
    while (*psz != '\0') {
        if (*psz >= 'a' && *psz <= 'z')
            *psz -= 'a' - 'A';
        psz++;
    }
}