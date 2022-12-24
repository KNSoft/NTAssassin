#define _NO_CRT_STDIO_INLINE

#include "include\NTAssassin\NTAStr.h"

#if !defined(SLIMCRT_USED)
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

// String Copy

SIZE_T NTAPI Str_CopyExW(_Out_writes_z_(DestCchSize) PWSTR Dest, SIZE_T DestCchSize, _In_ PCWSTR Src)
{
    SIZE_T i = 0;
    while (i < DestCchSize) {
        if ((Dest[i] = Src[i]) == UNICODE_NULL) {
            return i;
        }
        i++;
    }
    return 0;
}

SIZE_T NTAPI Str_CopyExA(_Out_writes_z_(DestCchSize) PSTR Dest, SIZE_T DestCchSize, _In_ PCSTR Src)
{
    SIZE_T i = 0;
    while (i < DestCchSize) {
        if ((Dest[i] = Src[i]) == ANSI_NULL) {
            return i;
        }
        i++;
    }
    return 0;
}

// String Compare

INT NTAPI Str_ICmpW(_In_ PCWSTR String1, _In_ PCWSTR String2)
{
    SIZE_T	i = 0;
    INT		iRes;
    do {
        iRes = Str_LowerChar(String1[i]) - Str_LowerChar(String2[i]);
        i++;
    } while (iRes == 0 && String1[i] != UNICODE_NULL);
    return iRes;
}

INT NTAPI Str_ICmpA(_In_ PCSTR String1, _In_ PCSTR String2)
{
    SIZE_T	i = 0;
    INT		iRes;
    do {
        iRes = Str_LowerChar(String1[i]) - Str_LowerChar(String2[i]);
        i++;
    } while (iRes == 0 && String1[i] != ANSI_NULL);
    return iRes;
}

// String Format

_Success_(return >= 0)
INT WINAPIV Str_PrintfExW(_Out_writes_z_(DestCchSize) PWSTR Dest, _In_ INT DestCchSize, _In_ _Printf_format_string_ PCWSTR Format, ...)
{
    va_list args;
    va_start(args, Format);
    return vswprintf_s(Dest, DestCchSize, Format, args);
}

_Success_(return >= 0)
INT WINAPIV Str_PrintfExA(_Out_writes_z_(DestCchSize) PSTR Dest, _In_ INT DestCchSize, _In_ _Printf_format_string_ PCSTR Format, ...)
{
    va_list args;
    va_start(args, Format);
    return vsprintf_s(Dest, DestCchSize, Format, args);
}

// String Index

INT NTAPI Str_IndexW(_In_ PCWSTR String, _In_ PCWSTR SubString)
{
    PCWSTR pSubStr = wcsstr(String, SubString);
    return pSubStr ? (INT)(pSubStr - String) : -1;
}

INT NTAPI Str_IndexA(_In_ PCSTR String, _In_ PCSTR SubString)
{
    PCSTR pSubStr = strstr(String, SubString);
    return pSubStr ? (INT)(pSubStr - String) : -1;
}

// String Converts

ULONG NTAPI Str_U2AEx(_Out_writes_z_(DestCchSize) PSTR Dest, _In_ ULONG DestCchSize, _In_ PCWSTR Src)
{
    ULONG ulANSIBytes = 0, iCch;
    RtlUnicodeToMultiByteN(Dest, DestCchSize * sizeof(CHAR), &ulANSIBytes, Src, (ULONG)Str_SizeW(Src) + sizeof(WCHAR));
    iCch = ulANSIBytes / sizeof(CHAR) - 1;
    Dest[iCch] = ANSI_NULL;
    return iCch;
}

ULONG NTAPI Str_A2UEx(_Out_writes_z_(DestCchSize) PWSTR Dest, _In_ ULONG DestCchSize, _In_ PCSTR Src)
{
    ULONG ulUnicodeBytes = 0, iCch;
    RtlMultiByteToUnicodeN(Dest, DestCchSize * sizeof(WCHAR), &ulUnicodeBytes, Src, (ULONG)Str_SizeA(Src) + sizeof(CHAR));
    iCch = ulUnicodeBytes / sizeof(WCHAR) - 1;
    Dest[iCch] = UNICODE_NULL;
    return iCch;
}

/*
    Unicode             | UTF-8
    ---------------------------------------------------------
    0000 0000-0000 007F | 0xxxxxxx
    0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
SIZE_T NTAPI Str_UnicodeToUTF8Ex(_Out_writes_z_(DestCchSize) PSTR Dest, _In_ SIZE_T DestCchSize, _In_ PCWSTR Src)
{
    UINT        cChDest = 0, cChSrc = 0;
    PSTR        psz = Dest;
    ULONG       ch;
    SIZE_T      uChLength;
    while (TRUE) {
        ch = Src[cChSrc];
        if (ch == UNICODE_NULL) {
        Label_Terminate:
            psz[0] = ANSI_NULL;
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
    return (SIZE_T)(psz - Dest);
}

VOID NTAPI Str_UpperW(_Inout_ PWSTR String)
{
    while (*String != UNICODE_NULL) {
        if (Str_IsLowerChar(*String)) {
            *String = Str_InverseCaseChar(*String);
        }
        String++;
    }
}

VOID NTAPI Str_UpperA(_Inout_ PSTR String)
{
    while (*String != ANSI_NULL) {
        if (Str_IsLowerChar(*String)) {
            *String = Str_InverseCaseChar(*String);
        }
        String++;
    }
}

VOID NTAPI Str_LowerW(_Inout_ PWSTR String)
{
    while (*String != UNICODE_NULL) {
        if (Str_IsUpperChar(*String)) {
            *String = Str_InverseCaseChar(*String);
        }
        String++;
    }
}

VOID NTAPI Str_LowerA(_Inout_ PSTR String)
{
    while (*String != ANSI_NULL) {
        if (Str_IsUpperChar(*String)) {
            *String = Str_InverseCaseChar(*String);
        }
        String++;
    }
}

// String Initialize

VOID NTAPI Str_InitW(_Out_ PUNICODE_STRING NTString, _In_ PWSTR String)
{
    SIZE_T uLen = Str_SizeW(String);
    NTString->Length = (USHORT)uLen;
    NTString->MaximumLength = (USHORT)(uLen + sizeof(WCHAR));
    NTString->Buffer = String;
}

VOID NTAPI Str_InitA(_Out_ PANSI_STRING NTString, _In_ PSTR String)
{
    SIZE_T uLen = Str_SizeA(String);
    NTString->Length = (USHORT)uLen;
    NTString->MaximumLength = (USHORT)(uLen + sizeof(CHAR));
    NTString->Buffer = String;
}

// String Convert
_Success_(return != FALSE)
BOOL NTAPI Str_ToIntExW(_In_ PCWSTR StrValue, BOOL Unsigned, UINT Base, _Out_writes_bytes_(ValueSize) PVOID Value, _In_ SIZE_T ValueSize)
{
    PCWSTR  psz = StrValue;
    UINT64  uTotal;

    // Minus
    BOOL    bMinus;
    if (*psz == UNICODE_NULL)
        return FALSE;
    if (*psz == L'-') {
        if (!Unsigned) {
            psz++;
            bMinus = TRUE;
        } else
            return FALSE;
    } else if (*psz == L'+') {
        psz++;
        bMinus = FALSE;
    } else
        bMinus = FALSE;

    // Base
    UINT    uBase;
    if (Base == 0) {
        if (*psz == UNICODE_NULL)
            return FALSE;
        if (*psz == L'0') {
            psz++;
            if (*psz == UNICODE_NULL) {
                uTotal = 0;
                goto Label_Output;
            } else if (*psz == L'b') {
                psz++;
                uBase = 2;
            } else if (*psz == L'o') {
                psz++;
                uBase = 8;
            } else if (*psz == L'x') {
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
        while ((wc = *psz++) != UNICODE_NULL) {
            if (wc == L'0') {
                uTemp = uTotal << 1;
            } else if (wc == L'1') {
                uTemp = (uTotal << 1) | 1;
            } else
                return FALSE;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    } else if (uBase == 8) {
        while ((wc = *psz++) != UNICODE_NULL) {
            if (wc >= L'0' && wc <= L'7') {
                uc = wc - L'0';
            } else
                return FALSE;
            uTemp = (uTotal << 3) + uc;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    } else if (uBase == 10) {
        while ((wc = *psz++) != UNICODE_NULL) {
            if (wc >= L'0' && wc <= L'9') {
                uc = wc - L'0';
            } else
                return FALSE;
            uTemp = uTotal * 10 + uc;
            if (uMax ? uTemp > uMax : uTemp <= uTotal)
                return FALSE;
            uTotal = uTemp;
        }
    } else if (uBase == 16) {
        while ((wc = *psz++) != UNICODE_NULL) {
            if (wc >= L'0' && wc <= L'9') {
                uc = wc - L'0';
            } else if (wc >= L'A' && wc <= L'F') {
                uc = wc - L'A' + 10;
            } else if (wc >= L'a' && wc <= L'f') {
                uc = wc - L'a' + 10;
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

_Success_(return != FALSE)
BOOL NTAPI Str_ToIntExA(_In_ PCSTR StrValue, BOOL Unsigned, UINT Base, _Out_writes_bytes_(ValueSize) PVOID Value, _In_ SIZE_T ValueSize)
{
    PCSTR  psz = StrValue;

    // Minus
    BOOL    bMinus;
    if (*psz == ANSI_NULL)
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
        if (*psz == ANSI_NULL)
            return FALSE;
        if (*psz == '0') {
            psz++;
            if (*psz == ANSI_NULL)
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
        while ((ch = *psz++) != ANSI_NULL) {
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
        while ((ch = *psz++) != ANSI_NULL) {
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
        while ((ch = *psz++) != ANSI_NULL) {
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
        while ((ch = *psz++) != ANSI_NULL) {
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

_Success_(return != FALSE)
BOOL NTAPI Str_FromIntExW(INT64 Value, BOOL Unsigned, UINT Base, _Out_writes_z_(DestCchSize) PWSTR StrValue, _In_ ULONG DestCchSize)
{
    PWSTR psz = StrValue;
    UINT64  uTotal, uDivisor, uPowerFlag;

    // Minus
    BOOL bNegative = !Unsigned && Value < 0;
    uTotal = bNegative ? -Value : (UINT64)Value;

    // Base
    if (Base == 2)
        uPowerFlag = 1;
    else if (Base == 8)
        uPowerFlag = 3;
    else if (Base == 10 || Base == 0)
        uPowerFlag = 0;
    else if (Base == 16)
        uPowerFlag = 4;
    else
        return FALSE;

    if (!DestCchSize)
        return FALSE;

    // Find max divisor
    UINT64 uDivisorTemp = Base;
    do {
        uDivisor = uDivisorTemp;
        uDivisorTemp = uPowerFlag ? uDivisorTemp << uPowerFlag : uDivisorTemp * 10;
    } while (uTotal >= uDivisorTemp && uDivisorTemp > uDivisor);

    // Convert
    BOOL bRet = TRUE;
    if (bNegative)
        *psz++ = L'-';
    while (TRUE) {
        UINT64 i = uTotal / uDivisor;
        uTotal = uTotal % uDivisor;
        if ((ULONG_PTR)psz - (ULONG_PTR)StrValue < ((ULONG_PTR)DestCchSize - 1) * sizeof(WCHAR)) {
            if (i != 0 || (bNegative ? psz != StrValue + 1 : psz != StrValue))
                *psz++ = (WCHAR)(i <= 9 ? i + L'0' : i - 10 + L'A');
        } else {
            bRet = FALSE;
            break;
        }
        if (uDivisor == Base) {
            i = uTotal;
            if ((ULONG_PTR)psz - (ULONG_PTR)StrValue < ((ULONG_PTR)DestCchSize - 1) * sizeof(WCHAR)) {
                *psz++ = (WCHAR)(i <= 9 ? i + L'0' : i - 10 + L'A');
            } else {
                bRet = FALSE;
            }
            break;
        }
        uDivisor = uPowerFlag ? uDivisor >> uPowerFlag : uDivisor / 10;
    };
    *psz = UNICODE_NULL;

    return bRet;
}

_Success_(return != FALSE)
BOOL NTAPI Str_FromIntExA(INT64 Value, BOOL Unsigned, UINT Base, _Out_writes_z_(DestCchSize) PSTR StrValue, _In_ ULONG DestCchSize)
{
    PSTR psz = StrValue;
    UINT64  uTotal, uDivisor, uPowerFlag;

    // Minus
    BOOL bNegative = !Unsigned && Value < 0;
    uTotal = bNegative ? -Value : (UINT64)Value;

    // Base
    if (Base == 2)
        uPowerFlag = 1;
    else if (Base == 8)
        uPowerFlag = 3;
    else if (Base == 10 || Base == 0)
        uPowerFlag = 0;
    else if (Base == 16)
        uPowerFlag = 4;
    else
        return FALSE;

    if (!DestCchSize)
        return FALSE;

    // Find max divisor
    UINT64 uDivisorTemp = Base;
    do {
        uDivisor = uDivisorTemp;
        uDivisorTemp = uPowerFlag ? uDivisorTemp << uPowerFlag : uDivisorTemp * 10;
    } while (uTotal >= uDivisorTemp && uDivisorTemp > uDivisor);

    // Convert
    BOOL bRet = TRUE;
    if (bNegative)
        *psz++ = '-';
    while (TRUE) {
        UINT64 i = uTotal / uDivisor;
        uTotal = uTotal % uDivisor;
        if ((ULONG_PTR)psz - (ULONG_PTR)StrValue < (ULONG_PTR)DestCchSize - 1) {
            if (i != 0 || (bNegative ? psz != StrValue + 1 : psz != StrValue))
                *psz++ = (CHAR)(i <= 9 ? i + '0' : i - 10 + 'A');
        } else {
            bRet = FALSE;
            break;
        }
        if (uDivisor == Base) {
            i = uTotal;
            if ((ULONG_PTR)psz - (ULONG_PTR)StrValue < (ULONG_PTR)DestCchSize - 1) {
                *psz++ = (CHAR)(i <= 9 ? i + '0' : i - 10 + 'A');
            } else {
                bRet = FALSE;
            }
            break;
        }
        uDivisor = uPowerFlag ? uDivisor >> uPowerFlag : uDivisor / 10;
    };
    *psz = ANSI_NULL;

    return bRet;
}

_Success_(return != FALSE)
BOOL NTAPI Str_RGBToHexExW(COLORREF Color, _Out_writes_z_(DestCchSize) PWSTR Dest, _In_ SIZE_T DestCchSize)
{
    PWSTR   psz;
    WCHAR   ch;
    UINT    uCh;
    DWORD   dwColor = Color;
    if (DestCchSize < HEXRGB_CCH)
        return FALSE;
    psz = Dest;
    *psz++ = L'#';
    for (uCh = 0; uCh < 6; uCh++) {
        ch = (WCHAR)(dwColor & 0xF);
        ch += ch <= 9 ? L'0' : (L'A' - 0xA);
        psz[uCh] = ch;
        dwColor >>= 4;
    }
    psz[uCh] = UNICODE_NULL;
    return TRUE;
}

_Success_(return != FALSE)
BOOL NTAPI Str_RGBToHexExA(COLORREF Color, _Out_writes_z_(DestCchSize) PSTR Dest, _In_ SIZE_T DestCchSize)
{
    PSTR    psz;
    CHAR    ch;
    UINT    uCh;
    DWORD   dwColor = Color;
    if (DestCchSize < HEXRGB_CCH)
        return FALSE;
    psz = Dest;
    *psz++ = '#';
    for (uCh = 0; uCh < 6; uCh++) {
        ch = (CHAR)(dwColor & 0xF);
        ch += ch <= 9 ? '0' : ('A' - 0xA);
        psz[uCh] = ch;
        dwColor >>= 4;
    }
    psz[uCh] = ANSI_NULL;
    return TRUE;
}

static CHAR arrSizeUnitPrefix[] = {
    '\0', 'K', 'M', 'G', 'T', 'P', 'E' // , 'Z', 'Y'
};

DOUBLE NTAPI Str_SimplifySize(UINT64 Size, _Out_opt_ PCHAR Unit)
{
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


// String Hash

DWORD NTAPI Str_Hash_X65599W(_In_ PCWSTR String)
{
    DWORD dwHash = 0;
    PCWSTR psz;
    for (psz = String; *psz != UNICODE_NULL; psz++) {
        dwHash = 65599 * dwHash + *psz;
    }
    return dwHash;
}

DWORD NTAPI Str_Hash_X65599A(_In_ PCSTR String)
{
    DWORD dwHash = 0;
    PCSTR psz;
    for (psz = String; *psz != ANSI_NULL; psz++) {
        dwHash = 65599 * dwHash + *psz;
    }
    return dwHash;
}

DWORD NTAPI Str_Hash_FNV1aW(_In_ PCWSTR String)
{
    DWORD dwHash = 2166136261U;
    PCWSTR psz;
    for (psz = String; *psz != UNICODE_NULL; psz++) {
        dwHash ^= *psz;
        dwHash *= 16777619U;
    }
    return dwHash;
}

DWORD NTAPI Str_Hash_FNV1aA(_In_ PCSTR String)
{
    DWORD dwHash = 2166136261U;
    PCSTR psz;
    for (psz = String; *psz != ANSI_NULL; psz++) {
        dwHash ^= *psz;
        dwHash *= 16777619U;
    }
    return dwHash;
}

PCWSTR NTAPI Str_RCharW(_In_ PCWSTR Path, _In_ WCHAR Char, _In_opt_ ULONG LengthOfPath)
{
    ULONG ulLen = LengthOfPath ? LengthOfPath : (ULONG)Str_LenW(Path);
    while (ulLen && Path[--ulLen] != Char);
    return ulLen ? Path + ulLen + 1 : NULL;
}

PCSTR NTAPI Str_RCharA(_In_ PCSTR Path, _In_ CHAR Char, _In_opt_ ULONG LengthOfPath)
{
    ULONG ulLen = LengthOfPath ? LengthOfPath : (ULONG)Str_LenA(Path);
    while (ulLen && Path[--ulLen] != Char);
    return ulLen ? Path + ulLen + 1 : NULL;
}
