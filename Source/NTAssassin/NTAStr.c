#include "include\NTAssassin\NTAssassin.h"

// String Length

SIZE_T NTAPI Str_CchLenW(PCWSTR String) {
    PCWSTR psz = String;
    while (*psz++ != '\0');
    return psz - String - 1;
}

SIZE_T NTAPI Str_CchLenA(PCSTR String) {
    PCSTR psz = String;
    while (*psz++ != '\0');
    return psz - String - 1;
}

SIZE_T NTAPI Str_CcbLenW(PCWSTR String) {
    PCWSTR psz = String;
    while (*psz++ != '\0');
    return (psz - String - 1) << 1;
}

// String Copy

SIZE_T NTAPI Str_CchCopyExW(PWSTR Dest, SIZE_T DestCchSize, PCWSTR Src) {
    SIZE_T i = 0;
    while (i < DestCchSize) {
        if ((Dest[i] = Src[i]) == '\0')
            return i;
        i++;
    }
    return 0;
}

SIZE_T NTAPI Str_CchCopyExA(PSTR Dest, SIZE_T DestCchSize, PCSTR Src) {
    SIZE_T i = 0;
    while (i < DestCchSize) {
        if ((Dest[i] = Src[i]) == '\0')
            return i;
        i++;
    }
    return 0;
}

SIZE_T NTAPI Str_CcbCopyExW(PWSTR Dest, SIZE_T DestCcbSize, PCWSTR Src) {
    SIZE_T i = 0;
    while (i < DestCcbSize / sizeof(WCHAR)) {
        if ((Dest[i] = Src[i]) == '\0')
            return i * sizeof(WCHAR);
        i++;
    }
    return 0;
}

// String Equal

BOOL NTAPI Str_EqualW(PCWSTR String1, PCWSTR String2) {
    UINT i = 0;
    while (TRUE) {
        if (String1[i] != String2[i])
            return FALSE;
        else if (String1[i] == '\0')
            return TRUE;
        i++;
    }
    return FALSE;
}

BOOL NTAPI Str_EqualA(PCSTR String1, PCSTR String2) {
    UINT i = 0;
    while (TRUE) {
        if (String1[i] != String2[i])
            return FALSE;
        else if (String1[i] == '\0')
            return TRUE;
        i++;
    }
    return FALSE;
}

BOOL NTAPI Str_EqualICW(PCWSTR String1, PCWSTR String2) {
    UINT i = 0;
    while (TRUE) {
        if (String1[i] != String2[i]) {
            if ((String1[i] >= 'a' && String1[i] <= 'z' && String1[i] - ('a' - 'A') != String2[i]) ||
                (String1[i] >= 'A' && String1[i] <= 'Z' && String1[i] + ('a' - 'A') != String2[i])
                )
                return FALSE;
        } else if (String1[i] == '\0')
            return TRUE;
        i++;
    }
    return FALSE;
}

BOOL NTAPI Str_EqualICA(PCSTR String1, PCSTR String2) {
    UINT i = 0;
    while (TRUE) {
        if (String1[i] != String2[i]) {
            if ((String1[i] >= 'a' && String1[i] <= 'z' && String1[i] - ('a' - 'A') != String2[i]) ||
                (String1[i] >= 'A' && String1[i] <= 'Z' && String1[i] + ('a' - 'A') != String2[i])
                )
                return FALSE;
        } else if (String1[i] == '\0')
            return TRUE;
        i++;
    }
    return FALSE;
}

// String Index

// Algorithm: BF
SIZE_T NTAPI Str_Index_BFW(PCWSTR String, PCWSTR Pattern) {
    SIZE_T  i;
    PCWSTR  pszSrc = String;
    BOOL    bMatched;
    while (TRUE) {
        bMatched = TRUE;
        i = 0;
        while (TRUE) {
            if (pszSrc[i] != Pattern[i]) {
                if (pszSrc[i] == '\0')
                    return -1;
                else if (Pattern[i] == '\0')
                    return i == 0 ? -1 : (SIZE_T)(pszSrc - String);
                else {
                    bMatched = FALSE;
                    break;
                }
            } else if (pszSrc[i] == '\0')
                return (SIZE_T)(pszSrc - String);
            i++;
        }
        if (bMatched)
            return -1;
        pszSrc++;
    }
    return -1;
}

SIZE_T NTAPI Str_Index_BFA(PCSTR String, PCSTR Pattern) {
    SIZE_T  i;
    PCSTR   pszSrc = String;
    BOOL    bMatched;
    while (TRUE) {
        bMatched = TRUE;
        i = 0;
        while (TRUE) {
            if (pszSrc[i] != Pattern[i]) {
                if (pszSrc[i] == '\0')
                    return -1;
                else if (Pattern[i] == '\0')
                    return i == 0 ? -1 : (SIZE_T)(pszSrc - String);
                else {
                    bMatched = FALSE;
                    break;
                }
            } else if (pszSrc[i] == '\0')
                return (SIZE_T)(pszSrc - String);
            i++;
        }
        if (bMatched)
            return -1;
        pszSrc++;
    }
    return -1;
}

ULONG NTAPI Str_CchU2AEx(PSTR Dest, ULONG DestCchSize, PCWSTR Src) {
    ULONG ulANSIBytes = 0;
    RtlUnicodeToMultiByteN(Dest, DestCchSize * sizeof(CHAR), &ulANSIBytes, Src, (ULONG)Str_CcbLenW(Src) + 1);
    return ulANSIBytes / sizeof(CHAR);
}

ULONG NTAPI Str_CchA2UEx(_Out_writes_(DestCchSize) PWSTR Dest, _In_ ULONG DestCchSize, PCSTR Src) {
    ULONG ulUnicodeBytes = 0;
    RtlMultiByteToUnicodeN(Dest, DestCchSize * sizeof(WCHAR), &ulUnicodeBytes, Src, (ULONG)Str_CcbLenA(Src) + 1);
    return ulUnicodeBytes / sizeof(WCHAR);
}

ULONG NTAPI Str_CcbA2UEx(_Out_writes_bytes_(DestCcbSize) PWSTR Dest, _In_ ULONG DestCcbSize, PCSTR Src) {
    ULONG ulUnicodeBytes = 0;
    RtlMultiByteToUnicodeN(Dest, DestCcbSize, &ulUnicodeBytes, Src, (ULONG)Str_CcbLenA(Src) + 1);
    return ulUnicodeBytes;
}

/*
    Unicode             | UTF-8
    ---------------------------------------------------------
    0000 0000-0000 007F | 0xxxxxxx
    0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
NTSTATUS NTAPI Str_UnicodeToUTF8Ex(PSTR Dest, SIZE_T DestCchSize, PCWSTR Src, PSIZE_T CharsWritten) {
    NTSTATUS    lStatus = STATUS_SUCCESS;
    UINT        cChDest = 0, cChSrc = 0;
    PSTR        psz = Dest;
    ULONG       ch;
    SIZE_T      uChLength;
    while (TRUE) {
        ch = Src[cChSrc];
        if (ch == '\0') {
        Label_Terminate:
            psz[0] = '\0';
            break;
        }
        // Surrogate Pair
        if (ch >= 0xD800 && ch <= 0xDBFF && Src[cChSrc + 1] != '\0' && Src[cChSrc + 1] >= 0xDC00 && Src[cChSrc + 1] <= 0xDFFF) {
            ch = ((ch - 0xD800) << 10 | (Src[cChSrc + 1] - 0xDC00)) + 0x010000;
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
            uChLength = 1;
            if (psz - Dest + uChLength >= DestCchSize)
                goto Label_Terminate;
            psz[0] = ch & 0b01111111;
        } else if (ch <= 0x7FF) {
            uChLength = 2;
            if (psz - Dest + uChLength >= DestCchSize)
                goto Label_Terminate;
            psz[0] = 0b11000000 | (ch >> 6 & 0b00011111);
            psz[1] = 0b10000000 | (ch >> 0 & 0b00111111);
        } else if (ch <= 0xFFFF) {
            uChLength = 3;
            if (psz - Dest + uChLength >= DestCchSize)
                goto Label_Terminate;
            psz[0] = 0b11100000 | (ch >> 12 & 0b00001111);
            psz[1] = 0b10000000 | (ch >> 6 & 0b00111111);
            psz[2] = 0b10000000 | (ch >> 0 & 0b00111111);
        } else {
            uChLength = 4;
            if (psz - Dest + uChLength >= DestCchSize)
                goto Label_Terminate;
            psz[0] = 0b11110000 | (ch >> 18 & 0b00000111);
            psz[1] = 0b10000000 | (ch >> 12 & 0b00111111);
            psz[2] = 0b10000000 | (ch >> 6 & 0b00111111);
            psz[3] = 0b10000000 | (ch >> 0 & 0b00111111);
        }
        psz += uChLength;
        Src++;
    }
    // Output number of characters written and exit
    if (CharsWritten)
        *CharsWritten = (SIZE_T)(psz - Dest);
    return lStatus;
}

VOID NTAPI Str_UpperW(PWSTR String) {
    while (*String != '\0') {
        if (*String >= 'a' && *String <= 'z')
            *String -= 'a' - 'A';
        String++;
    }
}

VOID NTAPI Str_UpperA(PSTR String) {
    while (*String != '\0') {
        if (*String >= 'a' && *String <= 'z')
            *String -= 'a' - 'A';
        String++;
    }
}

// String Initialize

VOID NTAPI Str_CchInitW(PUNICODE_STRING NTString, PWSTR String) {
    SIZE_T uLen = Str_CcbLenW(String);
    NTString->Length = (USHORT)uLen;
    NTString->MaximumLength = (USHORT)(uLen + sizeof(WCHAR));
    NTString->Buffer = String;
}

VOID NTAPI Str_CchInitA(PSTRING NTString, PSTR String) {
    SIZE_T uLen = Str_CcbLenA(String);
    NTString->Length = (USHORT)uLen;
    NTString->MaximumLength = (USHORT)(uLen + sizeof(CHAR));
    NTString->Buffer = String;
}

// String Convert

BOOL NTAPI Str_ToIntExW(PCWSTR StrValue, BOOL Unsigned, UINT Base, PVOID Value, SIZE_T ValueSize) {
    PCWSTR  psz = StrValue;
    UINT64  uTotal;

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
            if (*psz == '\0') {
                uTotal = 0;
                goto Label_Output;
            } else if (*psz == 'b') {
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
    UINT64  uTemp;
    uTotal = 0;
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
Label_Output:
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

BOOL NTAPI Str_RGBToHexExW(COLORREF Color, PWSTR Dest, SIZE_T DestCchSize) {
    PWSTR   psz;
    WCHAR   ch;
    UINT    uCh = 6;
    DWORD   dwColor = Color;
    if (DestCchSize < HEXRGB_CCH)
        return FALSE;
    psz = Dest;
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

BOOL NTAPI Str_RGBToHexExA(COLORREF Color, PSTR Dest, SIZE_T DestCchSize) {
    PSTR   psz;
    CHAR   ch;
    UINT    uCh = 6;
    DWORD   dwColor = Color;
    if (DestCchSize < HEXRGB_CCH)
        return FALSE;
    psz = Dest;
    *psz++ = '#';
    psz[uCh] = '\0';
    do {
        ch = (CHAR)(dwColor & 0xF);
        ch += ch <= 9 ? '0' : ('A' - 0xA);
        psz[--uCh] = ch;
        dwColor >>= 4;
    } while (uCh);
    return TRUE;
}

// String Hash

DWORD NTAPI Str_HashW(PCWSTR String, STR_HASH_ALGORITHM HashAlgorithm) {
    PCWSTR  psz = String;
    DWORD   dwHash = 0;
    if (HashAlgorithm == StrHashAlgorithmSDBM) {
        while (*psz != '\0')
            dwHash = 65599 * dwHash + *psz++;
    } else if (HashAlgorithm == StrHashAlgorithmBKDR) {
        while (*psz != '\0')
            dwHash = 31 * dwHash + *psz++;
    } else if (HashAlgorithm == StrHashAlgorithmAP) {
        BOOL    bOdd = FALSE;
        while (*psz != '\0') {
            dwHash ^= bOdd ? (~(dwHash << 11)) ^ (*psz++) ^ (dwHash >> 5) : (dwHash << 7) ^ (*psz++) ^ (dwHash >> 3);
            bOdd ^= TRUE;
        }
    } else if (HashAlgorithm == StrHashAlgorithmDJB) {
        dwHash = 5381;
        while (*psz != '\0')
            dwHash += (dwHash << 5) + *psz++;
    } else if (HashAlgorithm == StrHashAlgorithmJS) {
        dwHash = 1315423911;
        while (*psz != '\0')
            dwHash ^= (dwHash << 5) + (*psz++) + (dwHash >> 2);
    } else if (HashAlgorithm == StrHashAlgorithmRS) {
        DWORD   x = 63689;
        while (*psz != '\0') {
            dwHash = dwHash * x + *psz++;
            x *= 378551;
        }
    } else if (HashAlgorithm == StrHashAlgorithmELF) {
        DWORD   x = 0;
        while (*psz != '\0') {
            dwHash = (dwHash << 4) + *psz++;
            if ((x = dwHash & 0xF0000000) != 0)
                dwHash = (dwHash ^ (x >> 24)) & ~x;
        }
    } else if (HashAlgorithm == StrHashAlgorithmPJW) {
        DWORD   x = 0;
        while (*psz != '\0') {
            dwHash = (dwHash << 4) + *psz++;
            if ((x = dwHash & 0xF0000000) != 0)
                dwHash = (dwHash ^ (x >> 24)) & 0x0FFFFFFF;
        }
    }
    return dwHash;
}

DWORD NTAPI Str_HashA(PCSTR String, STR_HASH_ALGORITHM HashAlgorithm) {
    PCSTR   psz = String;
    DWORD   dwHash = 0;
    if (HashAlgorithm == StrHashAlgorithmSDBM) {
        while (*psz != '\0')
            dwHash = 65599 * dwHash + *psz++;
    } else if (HashAlgorithm == StrHashAlgorithmBKDR) {
        while (*psz != '\0')
            dwHash = 31 * dwHash + *psz++;
    } else if (HashAlgorithm == StrHashAlgorithmAP) {
        BOOL    bOdd = FALSE;
        while (*psz != '\0') {
            dwHash ^= bOdd ? (~(dwHash << 11)) ^ (*psz++) ^ (dwHash >> 5) : (dwHash << 7) ^ (*psz++) ^ (dwHash >> 3);
            bOdd ^= TRUE;
        }
    } else if (HashAlgorithm == StrHashAlgorithmDJB) {
        dwHash = 5381;
        while (*psz != '\0')
            dwHash += (dwHash << 5) + *psz++;
    } else if (HashAlgorithm == StrHashAlgorithmJS) {
        dwHash = 1315423911;
        while (*psz != '\0')
            dwHash ^= (dwHash << 5) + (*psz++) + (dwHash >> 2);
    } else if (HashAlgorithm == StrHashAlgorithmRS) {
        DWORD   x = 63689;
        while (*psz != '\0') {
            dwHash = dwHash * x + *psz++;
            x *= 378551;
        }
    } else if (HashAlgorithm == StrHashAlgorithmELF) {
        DWORD   x = 0;
        while (*psz != '\0') {
            dwHash = (dwHash << 4) + *psz++;
            if ((x = dwHash & 0xF0000000) != 0)
                dwHash = (dwHash ^ (x >> 24)) & ~x;
        }
    } else if (HashAlgorithm == StrHashAlgorithmPJW) {
        DWORD   x = 0;
        while (*psz != '\0') {
            dwHash = (dwHash << 4) + *psz++;
            if ((x = dwHash & 0xF0000000) != 0)
                dwHash = (dwHash ^ (x >> 24)) & 0x0FFFFFFF;
        }
    }
    return dwHash;
}