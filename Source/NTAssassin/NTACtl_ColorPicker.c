#include "include\NTAssassin\NTACtl.h"
#include "include\NTAssassin\NTAUI.h"
#include "include\NTAssassin\NTADlg.h"
#include "include\NTAssassin\NTAStr.h"
#include "include\NTAssassin\NTAGDI.h"

#define CTL_COLORPICKER_COLOR TEXT("NTAssassin.Ctl.ColorPicker")

static LRESULT CALLBACK Ctl_SetColorPickerSubclass_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_PAINT && IsWindowEnabled(hWnd)) {
        UI_WINDBPAINT   ps;
        TCHAR           szColor[HEXRGB_CCH];
        COLORREF        cr;
        UI_BeginPaint(hWnd, &ps);
        cr = Ctl_GetColorPickerValue(hWnd);
        GDI_FillSolidRect(ps.DC, &ps.Rect, cr);
        SetBkMode(ps.DC, TRANSPARENT);
        SelectBrush(ps.DC, GetStockBrush(BLACK_BRUSH));
        GDI_FrameRect(ps.DC, &ps.Rect, -1, PATCOPY);
        SetTextColor(ps.DC, UI_InverseRGB(cr));
        if (cr == CTL_COLORPICKER_NOCOLOR) {
            szColor[0] = '?';
            szColor[1] = '\0';
        } else if (!Str_RGBToHex(cr, szColor))
            szColor[0] = '\0';
        SelectFont(ps.DC, SendMessage(hWnd, WM_GETFONT, 0, 0));
        DrawText(ps.DC, szColor, -1, &ps.Rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        UI_EndPaint(hWnd, &ps);
        return 0;
    } else if (uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONUP) {
        COLORREF    cr;
        cr = Ctl_GetColorPickerValue(hWnd);
        if (Dlg_ChooseColor(hWnd, &cr)) {
            cr &= 0xFFFFFF;
            Ctl_SetColorPickerValue(hWnd, cr);
            SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetWindowLongPtr(hWnd, GWL_ID), BN_CLICKED), (LPARAM)hWnd);
        }
        return 0;
    } else if (uMsg == WM_ENABLE && wParam == TRUE) {
        UI_Redraw(hWnd);
        return 0;
    } else
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL NTAPI Ctl_SetColorPickerSubclass(HWND ButtonCtl, COLORREF Color) {
    SetProp(ButtonCtl, CTL_COLORPICKER_COLOR, (HANDLE)(DWORD_PTR)Color);
    return SetWindowSubclass(ButtonCtl, Ctl_SetColorPickerSubclass_WndProc, 0, 0);
}

COLORREF NTAPI Ctl_GetColorPickerValue(HWND ButtonCtl) {
    return (COLORREF)(DWORD_PTR)GetProp(ButtonCtl, CTL_COLORPICKER_COLOR);
}

BOOL NTAPI Ctl_SetColorPickerValue(HWND ButtonCtl, COLORREF Color) {
    BOOL    bRet;
    bRet = SetProp(ButtonCtl, CTL_COLORPICKER_COLOR, (HANDLE)(DWORD_PTR)Color);
    if (bRet)
        UI_Redraw(ButtonCtl);
    return bRet;
}