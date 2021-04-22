#include "NTAssassin.h"

VOID NTAPI DPI_FromWindow(HWND Window, PUINT DPIX, PUINT DPIY) {
    HDC     hDC = GetDC(Window);
    if (DPIX)
        *DPIX = GetDeviceCaps(hDC, LOGPIXELSX);
    if (DPIY)
        *DPIY = GetDeviceCaps(hDC, LOGPIXELSY);
    ReleaseDC(Window, hDC);
}

PFNIsProcessDPIAware pIsProcessDPIAware = NULL;

BOOL NTAPI DPI_IsAware() {
    if (NT_GetKUSD()->NtMajorVersion >= 6) {
        if (!pIsProcessDPIAware)
            pIsProcessDPIAware = (PFNIsProcessDPIAware)Proc_LoadProcAddr(L"User32.dll", "IsProcessDPIAware");
        return pIsProcessDPIAware ? pIsProcessDPIAware() : FALSE;
    } else
        return FALSE;
}

VOID NTAPI DPI_Scale(PINT ValuePointer, UINT OldDPI, UINT NewDPI) {
    *ValuePointer = Math_RoundInt(*ValuePointer * ((FLOAT)NewDPI / OldDPI));
}

VOID NTAPI DPI_ScaleRect(PRECT Rect, UINT OldDPIX, UINT NewDPIX, UINT OldDPIY, UINT NewDPIY) {
    DPI_Scale(&Rect->left, OldDPIX, NewDPIX);
    DPI_Scale(&Rect->right, OldDPIX, NewDPIX);
    DPI_Scale(&Rect->top, OldDPIY, NewDPIY);
    DPI_Scale(&Rect->bottom, OldDPIY, NewDPIY);
}

typedef struct _DPI_SETAUTOADJUSTSUBCLASS_REF {
    DWORD               dwNewDPIX;
    DWORD               dwNewDPIY;
    DWORD               dwOldDPIX;
    DWORD               dwOldDPIY;
    ENUMLOGFONTEXDVW    stFont;
    HFONT               hFont;
} DPI_SETAUTOADJUSTSUBCLASS_REF, * PDPI_SETAUTOADJUSTSUBCLASS_REF;

BOOL CALLBACK DPI_Subclass_DlgProc_ApplyToChild(HWND hWnd, LPARAM lParam) {
    PDPI_SETAUTOADJUSTSUBCLASS_REF  pstRef = (PDPI_SETAUTOADJUSTSUBCLASS_REF)lParam;
    RECT                            rcWnd;
    HFONT                           hFont;
    if (UI_GetRelativeRect(hWnd, NULL, &rcWnd)) {
        DPI_ScaleRect(&rcWnd, pstRef->dwOldDPIX, pstRef->dwNewDPIX, pstRef->dwOldDPIY, pstRef->dwNewDPIY);
        UI_SetWindowRect(hWnd, &rcWnd);
    }
    hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
    if (hFont)
        DeleteObject(hFont);
    SendMessage(hWnd, WM_SETFONT, (WPARAM)pstRef->hFont, FALSE);
    UI_Redraw(hWnd);
    return TRUE;
}

LRESULT CALLBACK DPI_SetAutoAdjustSubclass_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_DPICHANGED) {
        PDPI_SETAUTOADJUSTSUBCLASS_REF  pstRef = (PDPI_SETAUTOADJUSTSUBCLASS_REF)dwRefData;
        // Adjust rectangle
        pstRef->dwOldDPIX = pstRef->dwNewDPIX;
        pstRef->dwOldDPIY = pstRef->dwNewDPIY;
        pstRef->dwNewDPIX = LOWORD(wParam);
        pstRef->dwNewDPIY = HIWORD(wParam);
        UI_SetWindowRect(hDlg, (PRECT)lParam);
        // Adjust font
        HFONT   hFont;
        DPI_Scale(&pstRef->stFont.elfEnumLogfontEx.elfLogFont.lfHeight, pstRef->dwOldDPIY, pstRef->dwNewDPIY);
        hFont = CreateFontIndirectExW(&pstRef->stFont);
        if (hFont) {
            if (pstRef->hFont)
                DeleteObject(pstRef->hFont);
            pstRef->hFont = hFont;
        }
        // Apply to child windows
        UI_EnumChildWindows(hDlg, DPI_Subclass_DlgProc_ApplyToChild, dwRefData);
        UI_Redraw(hDlg);
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    } else if (uMsg == WM_DESTROY) {
        PDPI_SETAUTOADJUSTSUBCLASS_REF  pstRef = (PDPI_SETAUTOADJUSTSUBCLASS_REF)dwRefData;
        if (pstRef->hFont)
            DeleteObject(pstRef->hFont);
        Mem_Free(pstRef);
    }
    return DefSubclassProc(hDlg, uMsg, wParam, lParam);
}

BOOL NTAPI DPI_SetAutoAdjustSubclass(HWND Dialog, HFONT Font) {
    PDPI_SETAUTOADJUSTSUBCLASS_REF  pstRef;
    HFONT                           hFont;
    BOOL                            bRet;
    pstRef = Mem_Alloc(sizeof(DPI_SETAUTOADJUSTSUBCLASS_REF));
    if (!pstRef)
        return FALSE;
    GDI_InitInternalFontInfo(&pstRef->stFont);
    hFont = Font;
    if (!hFont) {
        hFont = I18N_CreateFont(KNS_DIALOG_DEFAULT_FONTSIZE, 0);
        pstRef->hFont = hFont;
    } else
        pstRef->hFont = NULL;
    if (!GDI_GetFont(hFont, &pstRef->stFont.elfEnumLogfontEx.elfLogFont))
        goto Lable_0;
    pstRef->dwNewDPIX = pstRef->dwNewDPIY = pstRef->dwOldDPIX = pstRef->dwOldDPIY = USER_DEFAULT_SCREEN_DPI;
    bRet = SetWindowSubclass(Dialog, DPI_SetAutoAdjustSubclass_DlgProc, 0, (DWORD_PTR)pstRef);
    if (bRet) {
        RECT    rcDlg;
        UINT    DPIX, DPIY;
        LONG    lDelta;
        GetWindowRect(Dialog, &rcDlg);
        DPI_FromWindow(Dialog, &DPIX, &DPIY);
        if (DPIX != USER_DEFAULT_SCREEN_DPI) {
            lDelta = Math_RoundInt((rcDlg.right - rcDlg.left) * (((FLOAT)DPIX / USER_DEFAULT_SCREEN_DPI) - 1) / 2);
            if (lDelta <= rcDlg.left) {
                rcDlg.left -= lDelta;
                rcDlg.right += lDelta;
            } else {
                rcDlg.right += 2 * lDelta - rcDlg.left;
                rcDlg.left = 0;
            }
        }
        if (DPIY != USER_DEFAULT_SCREEN_DPI) {
            lDelta = Math_RoundInt((rcDlg.bottom - rcDlg.top) * (((FLOAT)DPIY / USER_DEFAULT_SCREEN_DPI) - 1) / 2);
            if (lDelta <= rcDlg.top) {
                rcDlg.top -= lDelta;
                rcDlg.bottom += lDelta;
            } else {
                rcDlg.bottom += 2 * lDelta - rcDlg.top;
                rcDlg.top = 0;
            }
        }
        SendMessage(Dialog, WM_DPICHANGED, MAKEDWORD(DPIX, DPIY), (LPARAM)&rcDlg);
        return TRUE;
    }
Lable_0:
    Mem_Free(pstRef);
    return FALSE;
}