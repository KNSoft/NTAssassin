#include "NTAssassin\NTAssassin.h"

LPWSTR      lpI18NFontName = NULL;
BOOL        bI18NRTLReading = FALSE;
PI18N_ITEM  lpI18NStringTable = NULL;
PI18N_ITEM  lpI18NDefaultStringTable = NULL;
USHORT      usI18NStringCount = 0;

PI18N_LANGUAGE NTAPI I18N_InitEx(PI18N_LANGUAGE *Langs, UINT LangCount, USHORT ItemCount, LPCWSTR LangName) {
    usI18NStringCount = ItemCount;
    lpI18NDefaultStringTable = Langs[0]->StringTable;
    PI18N_LANGUAGE lpLang = I18N_FindLangEx(Langs, LangCount, LangName);
    I18N_SetLocale(lpLang);
    return lpLang;
}

VOID NTAPI I18N_SetLocale(PI18N_LANGUAGE Lang) {
    lpI18NFontName = Lang->FontName;
    bI18NRTLReading = Lang->RTLReading;
    lpI18NStringTable = Lang->StringTable;
}

PI18N_LANGUAGE I18N_FindLangEx(PI18N_LANGUAGE *Langs, UINT LangCount, LPCWSTR LangName) {
    WCHAR   wcName[MAX_LOCALENAME_CCH];
    LPCWSTR lpszLangName;
    int     iCchName;

    // Use user default language instead when lpszName is NULL
    if (LangName) {
        lpszLangName = LangName;
        iCchName = MAX_LOCALENAME_CCH;
    } else {
        iCchName = GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_SNAME, wcName, ARRAYSIZE(wcName));
        if (iCchName == 0 || wcName[0] == '\0')
            return Langs[0];
        lpszLangName = wcName;
    }

    // Match locale
    do {
        // Split name
        QWORD   qwNames[3] = { 0 };
        INT     i = 0, iNameIndex = 0;
        QWORD   qwName = 0;
        do {
            while (lpszLangName[i] != '\0' && lpszLangName[i] != '-') {
                qwName <<= 8;
                qwName |= lpszLangName[i];
                if (++i >= iCchName)
                    break;
            }
            qwNames[iNameIndex] = qwName;
            qwName = 0;
        } while (iNameIndex++ < ARRAYSIZE(qwNames) && lpszLangName[i] != '\0' && i++ < iCchName);
        // Find in list
        for (i = 0; i < (INT)LangCount; i++) {
            if (Langs[i]->Name[0] == qwNames[0] &&
                Langs[i]->Name[1] == qwNames[1] &&
                Langs[i]->Name[2] == qwNames[2])
                return Langs[i];
        }
        // Use parent instead
        iCchName = GetLocaleInfoEx(lpszLangName, LOCALE_SPARENT, wcName, ARRAYSIZE(wcName));
    } while (iCchName != 0 && wcName[0] != '\0');

    return Langs[0];
}

LPCWSTR NTAPI I18N_GetString(UINT_PTR StrIndex) {
    if (IS_INTRESOURCE(StrIndex)) {
        LPCWSTR lpsz = NULL;
        USHORT  usIndex = LOWORD(StrIndex);
        if (usIndex < usI18NStringCount) {
            lpsz = lpI18NStringTable[usIndex].Value;
            return lpsz ? lpsz : lpI18NDefaultStringTable[usIndex].Value;
        }
    } else {
        DWORD       dwHash = Str_HashExW((LPCWSTR)StrIndex, STRSAFE_MAX_CCH, StrHashAlgorithmX65599);
        PI18N_ITEM  lpItem = lpI18NStringTable, lpEndItem = lpI18NStringTable + usI18NStringCount;
        BOOL        bUseDefault = FALSE;
        while (TRUE) {
            while (lpItem < lpEndItem)
                if (lpItem->KeyHash == dwHash)
                    return lpItem->Value;
                else
                    lpItem++;
            if (bUseDefault || lpI18NStringTable == lpI18NDefaultStringTable)
                return NULL;
            lpItem = lpI18NDefaultStringTable;
            lpEndItem = lpI18NDefaultStringTable + usI18NStringCount;
            bUseDefault = TRUE;
        }
    }
    return NULL;
}

HFONT NTAPI I18N_CreateFont(INT FontSize, INT FontWeight) {
    return GDI_CreateFont(FontSize, FontWeight, lpI18NFontName ? lpI18NFontName : L"MS Shell Dlg");
}

HWND NTAPI I18N_InitCtlText(HWND Dialog, INT CtlID, UINT_PTR StrIndex) {
    HWND hCtl = GetDlgItem(Dialog, CtlID);
    UI_EnableWindowStyle(hCtl, GWL_EXSTYLE, WS_EX_RTLREADING, bI18NRTLReading);
    I18N_SetWndText(hCtl, StrIndex);
    return hCtl;
}

VOID NTAPI I18N_InitCtlTextsEx(HWND Dialog, PI18N_CTLTEXT CtlTexts, UINT CtlTextCount) {
    HWND hCtl;
    UINT i;
    for (i = 0; i < CtlTextCount; i++) {
        hCtl = GetDlgItem(Dialog, CtlTexts->CtlID);
        UI_EnableWindowStyle(hCtl, GWL_EXSTYLE, WS_EX_RTLREADING, bI18NRTLReading);
        SendMessageW(hCtl, WM_SETTEXT, 0, (LPARAM)I18N_GetString(CtlTexts->StrIndex));
        CtlTexts++;
    }
}