﻿#include "Include\NTAUI.h"

#include <dwmapi.h>

#include "Include\NTARProc.h"

#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "UxTheme.lib")

HDC NTAPI UI_BeginPaint(HWND Window, _Out_ PUI_WINDBPAINT Paint)
{
    Paint->DC = CreateCompatibleDC(BeginPaint(Window, &Paint->Paint));
    GetClientRect(Window, &Paint->Rect);
    Paint->Bitmap = CreateCompatibleBitmap(Paint->Paint.hdc, Paint->Rect.right, Paint->Rect.bottom);
    SelectObject(Paint->DC, Paint->Bitmap);
    return Paint->DC;
}

VOID NTAPI UI_EndPaint(HWND Window, _In_ PUI_WINDBPAINT Paint)
{
    BitBlt(Paint->Paint.hdc, 0, 0, Paint->Rect.right, Paint->Rect.bottom, Paint->DC, 0, 0, SRCCOPY);
    DeleteDC(Paint->DC);
    DeleteObject(Paint->Bitmap);
    EndPaint(Window, &Paint->Paint);
}

BOOL NTAPI UI_GetWindowRect(HWND Window, _Out_ PRECT Rect)
{
    return DwmGetWindowAttribute(Window, DWMWA_EXTENDED_FRAME_BOUNDS, Rect, sizeof(*Rect)) == S_OK ||
        GetWindowRect(Window, Rect);
}

BOOL NTAPI UI_SetWindowRect(HWND Window, _In_ PRECT Rect)
{
    RECT rcDwmDiff, rcOrgDiff;
    return DwmGetWindowAttribute(Window, DWMWA_EXTENDED_FRAME_BOUNDS, &rcDwmDiff, sizeof(rcDwmDiff)) == S_OK &&
        GetWindowRect(Window, &rcOrgDiff) ?
        SetWindowPos(
            Window,
            NULL,
            Rect->left + rcOrgDiff.left - rcDwmDiff.left,
            Rect->top + rcOrgDiff.top - rcDwmDiff.top,
            (Rect->right + rcOrgDiff.right - rcDwmDiff.right) - (Rect->left + rcOrgDiff.left - rcDwmDiff.left),
            (Rect->bottom + rcOrgDiff.bottom - rcDwmDiff.bottom) - (Rect->top + rcOrgDiff.top - rcDwmDiff.top),
            SWP_NOZORDER | SWP_NOACTIVATE) :
        SetWindowPos(
            Window,
            NULL,
            Rect->left,
            Rect->top,
            Rect->right - Rect->left,
            Rect->bottom - Rect->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
}

_Success_(return != FALSE)
BOOL NTAPI UI_GetRelativeRect(HWND Window, HWND RefWindow, _Out_ PRECT Rect)
{
    POINT   pt;
    HANDLE  hParent;
    RECT    rcWnd;
    BOOL    bRet;
    bRet = UI_GetWindowRect(Window, &rcWnd);
    if (bRet)
    {
        pt.x = rcWnd.left;
        pt.y = rcWnd.top;
        hParent = RefWindow ? RefWindow : GetParent(Window);
        bRet = ScreenToClient(hParent ? hParent : GetDesktopWindow(), &pt);
        if (bRet)
        {
            Rect->right = rcWnd.right + pt.x - rcWnd.left;
            Rect->bottom = rcWnd.bottom + pt.y - rcWnd.top;
            Rect->left = pt.x;
            Rect->top = pt.y;
        }
    }
    return bRet;
}

VOID NTAPI UI_EnumChildWindows(HWND ParentWindow, _In_ WNDENUMPROC WindowEnumProc, LPARAM Param)
{
    HWND hWndChild = GetWindow(ParentWindow, GW_CHILD);
    while (hWndChild && WindowEnumProc(hWndChild, Param))
    {
        hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
    }
}

BOOL NTAPI UI_IsDWMComposited()
{
    BOOL bEnabled;
    return SUCCEEDED(DwmIsCompositionEnabled(&bEnabled)) && bEnabled;
}

DWORD NTAPI UI_GetWindowCloackedState(HWND Window)
{
    DWORD dwCloackedState;
    return (SharedUserData->NtMajorVersion > 6 ||
        (SharedUserData->NtMajorVersion == 6 && SharedUserData->NtMinorVersion > 1)) &&
        DwmGetWindowAttribute(Window, DWMWA_CLOAKED, &dwCloackedState, sizeof(dwCloackedState)) == S_OK ? dwCloackedState : 0;
}

BOOL NTAPI UI_SetTheme(HWND Window)
{
    return SetWindowTheme(Window, L"Explorer", NULL) == S_OK;
}

BOOL NTAPI UI_EnableWindowStyle(HWND Window, INT StyleIndex, LONG_PTR StyleFlag, BOOL EnableState)
{
    LONG_PTR    lStyle;
    BOOL        bRet;
    bRet = FALSE;
    if (StyleIndex == GWL_STYLE || StyleIndex == GWL_EXSTYLE)
    {
        WIE_SetLastError(ERROR_SUCCESS);
        lStyle = GetWindowLongPtr(Window, StyleIndex);
        if (lStyle || WIE_GetLastError() == ERROR_SUCCESS)
        {
            WIE_SetLastError(ERROR_SUCCESS);
            if (EnableState)
            {
                SetFlag(lStyle, StyleFlag);
            } else
            {
                ClearFlag(lStyle, StyleFlag);
            }
            return SetWindowLongPtr(Window, StyleIndex, lStyle) || WIE_GetLastError() == ERROR_SUCCESS;
        }
    }
    return bRet;
}

HANDLE NTAPI UI_OpenProc(DWORD DesiredAccess, HWND Window)
{
    DWORD dwProcessId;
    GetWindowThreadProcessId(Window, &dwProcessId);
    return RProc_Open(DesiredAccess, dwProcessId);
}

BOOL NTAPI UI_SetNoNotifyFlag(HWND Window, BOOL EnableState)
{
    return EnableState ?
        SetProp(Window, UI_NONOTIFYPROP, (HANDLE)TRUE) :
        (BOOL)(DWORD_PTR)RemoveProp(Window, UI_NONOTIFYPROP);
}

BOOL NTAPI UI_GetNoNotifyFlag(HWND Window)
{
    return (BOOL)(DWORD_PTR)GetProp(Window, UI_NONOTIFYPROP);
}

LRESULT NTAPI UI_SetWndTextNoNotify(HWND Window, _In_opt_ PCWSTR Text)
{
    LRESULT lResult;
    UI_SetNoNotifyFlag(Window, TRUE);
    lResult = SendMessageW(Window, WM_SETTEXT, 0, (LPARAM)Text);
    UI_SetNoNotifyFlag(Window, FALSE);
    return lResult;
}

_Success_(return > 0) UINT NTAPI UI_GetWindowTextExW(HWND Window, _Out_writes_z_(TextCch) PWSTR Text, UINT TextCch)
{
    UINT cCh = (UINT)SendMessageW(Window, WM_GETTEXT, TextCch, (LPARAM)Text);
    if (cCh >= TextCch)
    {
        cCh = 0;
    }
    Text[cCh] = UNICODE_NULL;
    return cCh;
}

_Success_(return > 0)
UINT NTAPI UI_GetWindowTextExA(HWND Window, _Out_writes_z_(TextCch) PSTR Text, UINT TextCch)
{
    UINT cCh = (UINT)SendMessageA(Window, WM_GETTEXT, TextCch, (LPARAM)Text);
    if (cCh >= TextCch)
    {
        cCh = 0;
    }
    Text[cCh] = ANSI_NULL;
    return cCh;
}

_Success_(return != FALSE)
BOOL NTAPI UI_GetWindowLong(HWND Window, BOOL ClassLong, INT Index, _Out_ PLONG_PTR Result)
{
    LONG_PTR    lResult;
    BOOL        bRet;
    // GetWindowLongPtr may crash in some cases
    WIE_SetLastError(ERROR_SUCCESS);
    BOOL bUnicode = IsWindowUnicode(Window);
    if (bUnicode && !ClassLong)
    {
        lResult = GetWindowLongPtrW(Window, Index);
    } else if (bUnicode && ClassLong)
    {
        lResult = GetClassLongPtrW(Window, Index);
    } else if (!bUnicode && !ClassLong)
    {
        lResult = GetWindowLongPtrA(Window, Index);
    } else
    {
        lResult = GetClassLongPtrA(Window, Index);
    }
    bRet = lResult ? TRUE : WIE_GetLastError() == ERROR_SUCCESS;
    *Result = lResult;
    return bRet;
}

_Success_(return != FALSE)
BOOL NTAPI UI_MessageLoop(HWND Window, _Out_opt_ PUINT_PTR ExitCode)
{
    BOOL    bRet;
    MSG     stMsg;
    while (TRUE)
    {
        bRet = GetMessage(&stMsg, Window, 0, 0);
        if (bRet != 0 && bRet != -1)
        {
            TranslateMessage(&stMsg);
            DispatchMessage(&stMsg);
        } else
        {
            bRet = bRet == 0;
            if (bRet && ExitCode)
            {
                *ExitCode = stMsg.wParam;
            }
            return bRet;
        }
    }
}

VOID NTAPI UI_GetScreenPos(_Out_opt_ PPOINT Point, _Out_opt_ PSIZE Size)
{
    if (Point)
    {
        Point->x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        Point->y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    }
    if (Size)
    {
        Size->cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        Size->cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }
}

BOOL NTAPI UI_AllowDrop(HWND Window)
{
    return ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD) &&
        ChangeWindowMessageFilter(WM_COPYGLOBALDATA, MSGFLT_ADD);
}
