#include "include\NTAssassin\NTAssassin.h"

typedef struct _DPI_SETAUTOADJUSTSUBCLASS_REF {
    DWORD   dwNewDPIX;
    DWORD   dwNewDPIY;
    DWORD   dwOldDPIX;
    DWORD   dwOldDPIY;
    HFONT   hFont;
} DPI_SETAUTOADJUSTSUBCLASS_REF, *PDPI_SETAUTOADJUSTSUBCLASS_REF;

typedef struct _DPI_APPLYTOCHILD_REF {
    // Update DPI
    BOOL    bUpdateDPI;
    DWORD   dwNewDPIX;
    DWORD   dwNewDPIY;
    DWORD   dwOldDPIX;
    DWORD   dwOldDPIY;
    POINT   ptParent;

    // Update Font
    BOOL    bUpdateFont;
    HFONT   hFont;
    BOOL    bRedrawFontNow;
} DPI_APPLYTOCHILD_REF, *PDPI_APPLYTOCHILD_REF;

static PFNGetDpiForMonitor pfnGetDpiForMonitor = NULL;

BOOL NTAPI DPI_FromWindow(HWND Window, _Out_ PUINT DPIX, _Out_ PUINT DPIY) {
    PKUSER_SHARED_DATA pKUSD = NT_GetKUSD();
    if (pKUSD->NtMajorVersion > 6 || (pKUSD->NtMajorVersion == 6 && pKUSD->NtMinorVersion >= 3)) {
        if (!pfnGetDpiForMonitor) {
            pfnGetDpiForMonitor = (PFNGetDpiForMonitor)Proc_GetProcAddr(Sys_LoadDll(SysDllNameShcore), "GetDpiForMonitor");
        }
        if (pfnGetDpiForMonitor) {
            HMONITOR hMon = MonitorFromWindow(Window, MONITOR_DEFAULTTONULL);
            if (hMon && pfnGetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, DPIX, DPIY) == S_OK) {
                return TRUE;
            }
        }
    }
    HDC hDC;
    hDC = GetDC(Window);
    *DPIX = GetDeviceCaps(hDC, LOGPIXELSX);
    *DPIY = GetDeviceCaps(hDC, LOGPIXELSY);
    ReleaseDC(Window, hDC);
    return FALSE;
}

VOID NTAPI DPI_ScaleInt(_Inout_ PINT Value, _In_ UINT OldDPI, _In_ UINT NewDPI) {
    *Value = Math_RoundInt(*Value * ((FLOAT)NewDPI / OldDPI));
}

VOID NTAPI DPI_ScaleUInt(_Inout_ PUINT Value, _In_ UINT OldDPI, _In_ UINT NewDPI) {
    *Value = Math_RoundUInt(*Value * ((FLOAT)NewDPI / OldDPI));
}

VOID NTAPI DPI_ScaleRect(_Inout_ PRECT Rect, _In_ UINT OldDPIX, _In_ UINT NewDPIX, _In_ UINT OldDPIY, _In_ UINT NewDPIY) {
    DPI_ScaleInt(&Rect->left, OldDPIX, NewDPIX);
    DPI_ScaleInt(&Rect->right, OldDPIX, NewDPIX);
    DPI_ScaleInt(&Rect->top, OldDPIY, NewDPIY);
    DPI_ScaleInt(&Rect->bottom, OldDPIY, NewDPIY);
}

static BOOL CALLBACK DPI_Subclass_DlgProc_ApplyToChild(HWND hWnd, LPARAM lParam) {
    PDPI_APPLYTOCHILD_REF pstRef = (PDPI_APPLYTOCHILD_REF)lParam;
    BOOL bNeedsRedraw = FALSE;
    if (pstRef->bUpdateFont) {
        SendMessage(hWnd, WM_SETFONT, (WPARAM)pstRef->hFont, FALSE);
        if (pstRef->bRedrawFontNow) {
            bNeedsRedraw = TRUE;
        }
    }
    if (pstRef->bUpdateDPI) {
        RECT rcWnd;
        GetWindowRect(hWnd, &rcWnd);
        rcWnd.left -= pstRef->ptParent.x;
        rcWnd.right -= pstRef->ptParent.x;
        rcWnd.top -= pstRef->ptParent.y;
        rcWnd.bottom -= pstRef->ptParent.y;
        DPI_ScaleRect(&rcWnd, pstRef->dwOldDPIX, pstRef->dwNewDPIX, pstRef->dwOldDPIY, pstRef->dwNewDPIY);
        SetWindowPos(
            hWnd,
            NULL,
            rcWnd.left,
            rcWnd.top,
            rcWnd.right - rcWnd.left,
            rcWnd.bottom - rcWnd.top,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
        if (bNeedsRedraw) {
            bNeedsRedraw = TRUE;
        }
    }
    if (bNeedsRedraw) {
        UI_Redraw(hWnd);
    }

    return TRUE;
}

static LRESULT CALLBACK DPI_SetAutoAdjustSubclass_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_DPICHANGED) {
        PDPI_SETAUTOADJUSTSUBCLASS_REF pstRef = (PDPI_SETAUTOADJUSTSUBCLASS_REF)dwRefData;
        pstRef->dwOldDPIX = pstRef->dwNewDPIX;
        pstRef->dwOldDPIY = pstRef->dwNewDPIY;
        pstRef->dwNewDPIX = LOWORD(wParam);
        pstRef->dwNewDPIY = HIWORD(wParam);

        // Adjust rectangle if this is top-level window
        if (!(GetWindowLongPtr(hDlg, GWL_STYLE) & WS_CHILD)) {
            // System suggested RECT in lParam includes DWM shadow area,
            // so we had to calculate new RECT on our own
            RECT rcWnd;
            UI_GetWindowRect(hDlg, &rcWnd);
            RECT rcClient;
            GetClientRect(hDlg, &rcClient);
            rcClient.right = Math_RoundInt((rcClient.right * (FLOAT)pstRef->dwNewDPIX / pstRef->dwOldDPIX - rcClient.right) / 2);
            rcClient.bottom = Math_RoundInt((rcClient.bottom * (FLOAT)pstRef->dwNewDPIY / pstRef->dwOldDPIY - rcClient.bottom) / 2);
            rcWnd.top -= rcClient.bottom;
            rcWnd.bottom += rcClient.bottom;
            rcWnd.left -= rcClient.right;
            rcWnd.right += rcClient.right;
            // Correct to (0, 0) if overflow
            if (rcWnd.left < 0) {
                rcWnd.right += -rcWnd.left;
                rcWnd.left = 0;
            }
            if (rcWnd.top < 0) {
                rcWnd.bottom += -rcWnd.top;
                rcWnd.top = 0;
            }
            UI_SetWindowRect(hDlg, &rcWnd);
        }

        // Adjust DPI for child
        DPI_APPLYTOCHILD_REF dwChildRef;
        dwChildRef.bUpdateDPI = TRUE;
        dwChildRef.dwOldDPIX = pstRef->dwOldDPIX;
        dwChildRef.dwOldDPIY = pstRef->dwOldDPIX;
        dwChildRef.dwNewDPIX = LOWORD(wParam);
        dwChildRef.dwNewDPIY = HIWORD(wParam);
        dwChildRef.ptParent.x = dwChildRef.ptParent.y = 0;
        ClientToScreen(hDlg, &dwChildRef.ptParent);

        // Adjust font for child
        dwChildRef.bUpdateFont = FALSE;
        if (pstRef->hFont) {
            ENUMLOGFONTEXDVW FontInfo;
            if (GDI_GetFont(pstRef->hFont, &FontInfo)) {
                DPI_ScaleInt(&FontInfo.elfEnumLogfontEx.elfLogFont.lfHeight, pstRef->dwOldDPIY, pstRef->dwNewDPIY);
                HFONT hFont = CreateFontIndirectExW(&FontInfo);
                if (hFont) {
                    DeleteObject(pstRef->hFont);
                    pstRef->hFont = hFont;
                    dwChildRef.bUpdateFont = TRUE;
                    dwChildRef.hFont = hFont;
                    dwChildRef.bRedrawFontNow = TRUE;
                }
            }
        }

        // Apply to child windows
        UI_EnumChildWindows(hDlg, DPI_Subclass_DlgProc_ApplyToChild, (LPARAM)&dwChildRef);
        UI_Redraw(hDlg);
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    } else if (uMsg == WM_DESTROY) {
        PDPI_SETAUTOADJUSTSUBCLASS_REF pstRef = (PDPI_SETAUTOADJUSTSUBCLASS_REF)dwRefData;
        if (pstRef->hFont) {
            DeleteObject(pstRef->hFont);
        }
        Mem_Free(pstRef);
    }
    return DefSubclassProc(hDlg, uMsg, wParam, lParam);
}

BOOL NTAPI DPI_SetAutoAdjustSubclass(HWND Dialog, _In_opt_ HFONT Font) {
    PDPI_SETAUTOADJUSTSUBCLASS_REF pstRef;
    pstRef = Mem_Alloc(sizeof(DPI_SETAUTOADJUSTSUBCLASS_REF));
    if (!pstRef) {
        return FALSE;
    }
    pstRef->dwNewDPIX = pstRef->dwNewDPIY = pstRef->dwOldDPIX = pstRef->dwOldDPIY = USER_DEFAULT_SCREEN_DPI;
    pstRef->hFont = Font;
    if (SetWindowSubclass(Dialog, DPI_SetAutoAdjustSubclass_DlgProc, 0, (DWORD_PTR)pstRef)) {
        UINT DPIX, DPIY;
        DPI_FromWindow(Dialog, &DPIX, &DPIY);
        RECT rcDlg;
        UI_GetWindowRect(Dialog, &rcDlg);
        DPI_ScaleRect(&rcDlg, USER_DEFAULT_SCREEN_DPI, DPIX, USER_DEFAULT_SCREEN_DPI, DPIY);
        SendMessage(Dialog, WM_DPICHANGED, MAKEDWORD(DPIX, DPIY), (LPARAM)&rcDlg);
        return TRUE;
    }
    Mem_Free(pstRef);
    return FALSE;
}
