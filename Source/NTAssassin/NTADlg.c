#include "NTAssassin.h"

LPDLGTEMPLATEW NTAPI Dlg_InitTemplate(PDLG_TEMPLATE Template, DWORD Style, DWORD ExtendedStyle, INT X, INT Y, INT Width, INT Height) {
    Template->wMenu = Template->wClass = Template->wTitle = Template->Template.cdit = 0;
    Template->Template.style = Style;
    Template->Template.dwExtendedStyle = ExtendedStyle;
    Template->Template.x = X;
    Template->Template.y = Y;
    Template->Template.cx = Width;
    Template->Template.cy = Height;
    return &Template->Template;
}

BOOL NTAPI Dlg_GetOpenFileNameEx(HWND Owner, PCWSTR Filter, PWSTR File, DWORD MaxFile, PCWSTR DefExt) {
    OPENFILENAMEW   stOpenOFNW = { sizeof(OPENFILENAMEW) };
    stOpenOFNW.hwndOwner = Owner;
    stOpenOFNW.lpstrFilter = Filter;
    stOpenOFNW.lpstrFile = File;
    stOpenOFNW.nMaxFile = MaxFile;
    stOpenOFNW.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
    stOpenOFNW.lpstrDefExt = DefExt;
    return GetOpenFileNameW(&stOpenOFNW);
}

BOOL NTAPI Dlg_GetSaveFileNameEx(HWND Owner, PCWSTR Filter, PWSTR File, DWORD MaxFile, PCWSTR DefExt) {
    OPENFILENAMEW   stSaveOFNW = { sizeof(OPENFILENAMEW) };
    stSaveOFNW.hwndOwner = Owner;
    stSaveOFNW.lpstrFilter = Filter;
    stSaveOFNW.lpstrFile = File;
    stSaveOFNW.nMaxFile = MaxFile;
    stSaveOFNW.Flags = OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
    stSaveOFNW.lpstrDefExt = DefExt;
    return GetSaveFileNameW(&stSaveOFNW);
}

BOOL NTAPI Dlg_ChooseColor(HWND Owner, LPCOLORREF ColorPointer) {
    COLORREF    acrCustClr[16] = {
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255),
        RGB(255, 255, 255)
    };
    CHOOSECOLOR stChooseColor = { sizeof(CHOOSECOLOR), NULL, NULL, RGB(0, 0, 0), acrCustClr, CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT };
    BOOL        bRet;
    stChooseColor.hwndOwner = Owner;
    stChooseColor.rgbResult = *ColorPointer;
    bRet = ChooseColor(&stChooseColor);
    if (bRet)
        *ColorPointer = stChooseColor.rgbResult;
    return bRet;
}

BOOL NTAPI Dlg_ChooseFont(HWND Owner, PLOGFONTW FontPointer, LPCOLORREF ColorPointer) {
    CHOOSEFONTW stChooseFontW = { sizeof(CHOOSEFONTW) };
    BOOL        bRet;
    stChooseFontW.hwndOwner = Owner;
    if (ColorPointer) {
        stChooseFontW.Flags = CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST | CF_EFFECTS;
        stChooseFontW.rgbColors = *ColorPointer;
    } else
        stChooseFontW.Flags = CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST;
    stChooseFontW.lpLogFont = FontPointer;
    bRet = ChooseFontW(&stChooseFontW);
    if (bRet && ColorPointer)
        *ColorPointer = stChooseFontW.rgbColors;
    return bRet;
}

BOOL NTAPI Dlg_ScreenSnapshot(PDLG_SCREENSNAPSHOT ScreenSnapshot) {
    WNDCLASSEXW stWndClsExCaptureW = { sizeof(WNDCLASSEXW), 0, NULL, 0, 0, NULL, NULL, NULL, NULL, NULL, L"NTAssassin.UI.Dlg.ScreenSnapshotClass", NULL };
    ATOM        atomClass;
    HWND        hWnd;
    HDC         hdcScreen;
    BOOL        bRet = FALSE;
    stWndClsExCaptureW.style = ScreenSnapshot->dwClassStyle;
    stWndClsExCaptureW.lpfnWndProc = ScreenSnapshot->lpfnWndProc;
    stWndClsExCaptureW.hCursor = ScreenSnapshot->hCursor;
    stWndClsExCaptureW.hInstance = ScreenSnapshot->hInstance;
    atomClass = RegisterClassExW(&stWndClsExCaptureW);
    if (atomClass) {
        // Get virtual screen position
        ScreenSnapshot->iScreenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
        ScreenSnapshot->iScreenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
        ScreenSnapshot->iScreenCX = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        ScreenSnapshot->iScreenCY = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        // Mirror current screen
        hdcScreen = GetDC(NULL);
        ScreenSnapshot->hdcMirror = CreateCompatibleDC(hdcScreen);
        ScreenSnapshot->hbmMirror = CreateCompatibleBitmap(hdcScreen, ScreenSnapshot->iScreenCX, ScreenSnapshot->iScreenCY);
        SelectObject(ScreenSnapshot->hdcMirror, ScreenSnapshot->hbmMirror);
        BitBlt(ScreenSnapshot->hdcMirror, 0, 0, ScreenSnapshot->iScreenCX, ScreenSnapshot->iScreenCY, hdcScreen, ScreenSnapshot->iScreenX, ScreenSnapshot->iScreenY, SRCCOPY);
        // Create window and enter message loop
        hWnd = CreateWindowExW(ScreenSnapshot->dwExStyle, MAKEINTRESOURCEW(atomClass), L"NTAssassin.UI.Dlg.ScreenSnapshotClass", ScreenSnapshot->dwStyle, ScreenSnapshot->iScreenX, ScreenSnapshot->iScreenY, ScreenSnapshot->iScreenCX, ScreenSnapshot->iScreenCY, HWND_DESKTOP, NULL, ScreenSnapshot->hInstance, ScreenSnapshot->lParam);
        if (hWnd) {
            MSG     stMsg;
            while (TRUE) {
                bRet = GetMessageW(&stMsg, hWnd, 0, 0);
                if (bRet == 0 || bRet == -1) {
                    break;
                } else {
                    TranslateMessage(&stMsg);
                    DispatchMessageW(&stMsg);
                }
            }
        }
        // Cleanup
        DeleteObject(ScreenSnapshot->hbmMirror);
        DeleteDC(ScreenSnapshot->hdcMirror);
        ReleaseDC(NULL, hdcScreen);
        UnregisterClassW(MAKEINTRESOURCEW(atomClass), stWndClsExCaptureW.hInstance);
        bRet = TRUE;
    }
    return bRet;
}

typedef struct _DLG_SETRESIZINGSUBCLASS_REF {
    LONG            lMinWidth;
    LONG            lMinHeight;
    DWORD           dwNewDPIX;
    DWORD           dwNewDPIY;
    DWORD           dwOldDPIX;
    DWORD           dwOldDPIY;
    LONG            lStyle;
    DLG_RESIZEDPROC pfnResizedProc;
} DLG_SETRESIZINGSUBCLASS_REF, * PDLG_SETRESIZINGSUBCLASS_REF;

LRESULT CALLBACK Dlg_SetResizingSubclass_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_DPICHANGED) {
        PDLG_SETRESIZINGSUBCLASS_REF    pstRef = (PDLG_SETRESIZINGSUBCLASS_REF)dwRefData;
        pstRef->dwOldDPIX = pstRef->dwNewDPIX;
        pstRef->dwOldDPIY = pstRef->dwNewDPIY;
        pstRef->dwNewDPIX = LOWORD(wParam);
        pstRef->dwNewDPIY = HIWORD(wParam);
        DPI_Scale(&pstRef->lMinWidth, pstRef->dwOldDPIX, pstRef->dwNewDPIX);
        DPI_Scale(&pstRef->lMinHeight, pstRef->dwOldDPIY, pstRef->dwNewDPIY);
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    } else if (uMsg == WM_SIZING) {
        PDLG_SETRESIZINGSUBCLASS_REF    pstRef = (PDLG_SETRESIZINGSUBCLASS_REF)dwRefData;
        if (pstRef->lStyle & WS_THICKFRAME) {
            PRECT lpRect = (RECT*)lParam;
            if (pstRef->lMinWidth && lpRect->right < lpRect->left + pstRef->lMinWidth)
                lpRect->right = lpRect->left + pstRef->lMinWidth;
            if (pstRef->lMinHeight && lpRect->bottom < lpRect->top + pstRef->lMinHeight)
                lpRect->bottom = lpRect->top + pstRef->lMinHeight;
            SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
        }
    } else if (uMsg == WM_SIZE) {
        if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED) {
            PDLG_SETRESIZINGSUBCLASS_REF    pstRef = (PDLG_SETRESIZINGSUBCLASS_REF)dwRefData;
            pstRef->pfnResizedProc(hDlg, LOWORD(lParam), HIWORD(lParam), FALSE);
            UI_Redraw(hDlg);
            SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
        }
    } else if (uMsg == WM_WINDOWPOSCHANGED) {
        PDLG_SETRESIZINGSUBCLASS_REF    pstRef = (PDLG_SETRESIZINGSUBCLASS_REF)dwRefData;
        RECT                            rcClient;
        GetClientRect(hDlg, &rcClient);
        pstRef->pfnResizedProc(hDlg, rcClient.right, rcClient.bottom, TRUE);
        UI_Redraw(hDlg);
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    } else if (uMsg == WM_STYLECHANGED) {
        PDLG_SETRESIZINGSUBCLASS_REF    pstRef = (PDLG_SETRESIZINGSUBCLASS_REF)dwRefData;
        if (wParam == GWL_STYLE)
            pstRef->lStyle = ((LPSTYLESTRUCT)lParam)->styleNew;
    } else if (uMsg == WM_DESTROY) {
        Mem_Free((PVOID)dwRefData);
    }
    return DefSubclassProc(hDlg, uMsg, wParam, lParam);
}

BOOL NTAPI Dlg_SetResizingSubclass(HWND Dialog, LONG MinWidth, LONG MinHeight, DLG_RESIZEDPROC ResizedProc) {
    PDLG_SETRESIZINGSUBCLASS_REF    pstRef;
    pstRef = Mem_Alloc(sizeof(DLG_SETRESIZINGSUBCLASS_REF));
    if (!pstRef)
        return FALSE;
    pstRef->lMinWidth = MinWidth;
    pstRef->lMinHeight = MinHeight;
    DPI_FromWindow(Dialog, &pstRef->dwOldDPIX, &pstRef->dwOldDPIY);
    pstRef->dwNewDPIX = pstRef->dwOldDPIX;
    pstRef->dwNewDPIY = pstRef->dwOldDPIY;
    pstRef->lStyle = (LONG)GetWindowLongPtr(Dialog, GWL_STYLE);
    pstRef->pfnResizedProc = ResizedProc;
    return SetWindowSubclass(Dialog, Dlg_SetResizingSubclass_DlgProc, 0, (DWORD_PTR)pstRef);
}