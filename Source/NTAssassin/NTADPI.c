#include "include\NTAssassin\NTADPI.h"
#include "include\NTAssassin\NTASys.h"
#include "include\NTAssassin\NTANT.h"
#include "include\NTAssassin\NTAProc.h"
#include "include\NTAssassin\NTAMath.h"
#include "include\NTAssassin\NTAUI.h"
#include "include\NTAssassin\NTAMem.h"
#include "include\NTAssassin\NTAGDI.h"

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

        // Adjust rectangle, Win11 has fixed the DWM shadow problem
        PRECT prc = (PRECT)lParam;
        if (NT_GetKUSD()->NtMajorVersion >= 10 || NT_GetKUSD()->NtBuildNumber >= 22000) {
            SetWindowPos(hDlg, NULL, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, SWP_NOZORDER | SWP_NOACTIVATE);
        } else {
            UI_SetWindowRect(hDlg, prc);
        }

        // Adjust DPI for child
        DPI_APPLYTOCHILD_REF ChildRef;
        ChildRef.bUpdateDPI = TRUE;
        ChildRef.dwOldDPIX = pstRef->dwOldDPIX;
        ChildRef.dwOldDPIY = pstRef->dwOldDPIX;
        ChildRef.dwNewDPIX = LOWORD(wParam);
        ChildRef.dwNewDPIY = HIWORD(wParam);
        ChildRef.ptParent.x = ChildRef.ptParent.y = 0;
        ClientToScreen(hDlg, &ChildRef.ptParent);

        // Adjust font for child
        ChildRef.bUpdateFont = FALSE;
        if (pstRef->hFont) {
            ENUMLOGFONTEXDVW FontInfo;
            if (GDI_GetFont(pstRef->hFont, &FontInfo)) {
                FontInfo.elfEnumLogfontEx.elfLogFont.lfWidth = 0;
                DPI_ScaleInt(&FontInfo.elfEnumLogfontEx.elfLogFont.lfHeight, pstRef->dwOldDPIY, pstRef->dwNewDPIY);
                HFONT hFont = CreateFontIndirectExW(&FontInfo);
                if (hFont) {
                    DeleteObject(pstRef->hFont);
                    pstRef->hFont = hFont;
                    ChildRef.bUpdateFont = TRUE;
                    ChildRef.hFont = hFont;
                    ChildRef.bRedrawFontNow = TRUE;
                }
            }
        }

        // Apply to child windows
        UI_EnumChildWindows(hDlg, DPI_Subclass_DlgProc_ApplyToChild, (LPARAM)&ChildRef);
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

BOOL NTAPI DPI_SetAutoAdjustSubclass(_In_ HWND Dialog, _In_opt_ PENUMLOGFONTEXDVW FontInfo) {
    PDPI_SETAUTOADJUSTSUBCLASS_REF pstRef;
    RECT rcDlg;
    UINT DPIX, DPIY;
    LONG lDelta;
    pstRef = Mem_Alloc(sizeof(DPI_SETAUTOADJUSTSUBCLASS_REF));
    if (!pstRef) {
        return FALSE;
    }
    pstRef->dwNewDPIX = pstRef->dwNewDPIY = pstRef->dwOldDPIX = pstRef->dwOldDPIY = USER_DEFAULT_SCREEN_DPI;
    if (UI_GetWindowRect(Dialog, &rcDlg) &&
        SetWindowSubclass(Dialog, DPI_SetAutoAdjustSubclass_DlgProc, 0, (DWORD_PTR)pstRef)) {
        DPI_FromWindow(Dialog, &DPIX, &DPIY);
        pstRef->hFont = FontInfo ? CreateFontIndirectExW(FontInfo) : GDI_CreateDefaultFont();
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
    } else {
        Mem_Free(pstRef);
        return FALSE;
    }
}

_Success_(return != FALSE)
BOOL NTAPI DPI_GetAutoAdjustSubclass(_In_ HWND Dialog, _Out_opt_ PDWORD NewDPIX, _Out_opt_ PDWORD NewDPIY, _Out_opt_ HFONT * Font) {
    PDPI_SETAUTOADJUSTSUBCLASS_REF pstRef;
    if (GetWindowSubclass(Dialog, DPI_SetAutoAdjustSubclass_DlgProc, 0, (PDWORD_PTR)&pstRef)) {
        if (NewDPIX) {
            *NewDPIX = pstRef->dwNewDPIX;
        }
        if (NewDPIY) {
            *NewDPIY = pstRef->dwNewDPIY;
        }
        if (Font) {
            *Font = pstRef->hFont;
        }
        return TRUE;
    } else {
        return FALSE;
    }
}
