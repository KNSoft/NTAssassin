#include "NTAssassin\NTAssassin.h"

HDC NTAPI UI_BeginPaint(HWND Window, PUI_WINDBPAINT Paint) {
    Paint->DC = CreateCompatibleDC(BeginPaint(Window, &Paint->Paint));
    GetClientRect(Window, &Paint->Rect);
    Paint->Bitmap = CreateCompatibleBitmap(Paint->Paint.hdc, Paint->Rect.right, Paint->Rect.bottom);
    SelectObject(Paint->DC, Paint->Bitmap);
    return Paint->DC;
}

VOID NTAPI UI_EndPaint(HWND Window, PUI_WINDBPAINT Paint) {
    BitBlt(Paint->Paint.hdc, 0, 0, Paint->Rect.right, Paint->Rect.bottom, Paint->DC, 0, 0, SRCCOPY);
    DeleteDC(Paint->DC);
    DeleteObject(Paint->Bitmap);
    EndPaint(Window, &Paint->Paint);
}

PFNDwmGetWindowAttribute pfnDwmGetWindowAttribute = NULL;

BOOL NTAPI UI_GetWindowRect(HWND Window, PRECT Rect) {
    if (NT_GetKUSD()->NtMajorVersion >= 6 ) {
        if (!pfnDwmGetWindowAttribute)
            pfnDwmGetWindowAttribute = (PFNDwmGetWindowAttribute)Proc_GetProcAddr(Sys_LoadDll(SysLoadDllDwmapi), "DwmGetWindowAttribute");
        if (pfnDwmGetWindowAttribute && pfnDwmGetWindowAttribute(Window, DWMWA_EXTENDED_FRAME_BOUNDS, Rect, sizeof(*Rect)) == S_OK)
            return TRUE;
    }
    return GetWindowRect(Window, Rect);
}

BOOL NTAPI UI_GetRelativeRect(HWND Window, HWND RefWindow, PRECT Rect) {
    POINT   pt;
    HANDLE  hParent;
    RECT    rcWnd;
    BOOL    bSucc;
    bSucc = UI_GetWindowRect(Window, &rcWnd);
    if (bSucc) {
        pt.x = rcWnd.left;
        pt.y = rcWnd.top;
        hParent = IF_NULL(RefWindow, GetParent(Window));
        bSucc = ScreenToClient(IF_NULL(hParent, GetDesktopWindow()), &pt);
        if (bSucc) {
            Rect->right = rcWnd.right + pt.x - rcWnd.left;
            Rect->bottom = rcWnd.bottom + pt.y - rcWnd.top;
            Rect->left = pt.x;
            Rect->top = pt.y;
        }
    }
    return bSucc;
}

VOID NTAPI UI_EnumChildWindows(HWND ParentWindow, WNDENUMPROC WindowEnumProc, LPARAM Param) {
    HWND hWndChild = GetWindow(ParentWindow, GW_CHILD);
    while (hWndChild && WindowEnumProc(hWndChild, Param))
        hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
}

PFNDwmIsCompositionEnabled pfnDwmIsCompositionEnabled = NULL;

BOOL NTAPI UI_IsDWMComposited() {
    BOOL    bEnabled;
    if (!pfnDwmIsCompositionEnabled) {
        pfnDwmIsCompositionEnabled = (PFNDwmIsCompositionEnabled)Proc_GetProcAddr(Sys_LoadDll(SysLoadDllDwmapi), "DwmIsCompositionEnabled");
    }
    return pfnDwmIsCompositionEnabled ?
        (SUCCEEDED(pfnDwmIsCompositionEnabled(&bEnabled)) ? bEnabled : FALSE) :
        FALSE;
}

DWORD NTAPI UI_GetWindowCloackedState(HWND Window) {
    DWORD   dwCloackedState;
    if (NT_GetKUSD()->NtMajorVersion > 6 ||
        (NT_GetKUSD()->NtMajorVersion == 6) && NT_GetKUSD()->NtMinorVersion > 1) {
        if (!pfnDwmGetWindowAttribute)
            pfnDwmGetWindowAttribute = (PFNDwmGetWindowAttribute)Proc_GetProcAddr(Sys_LoadDll(SysLoadDllDwmapi), "DwmGetWindowAttribute");
        if (pfnDwmGetWindowAttribute && pfnDwmGetWindowAttribute(Window, DWMWA_CLOAKED, &dwCloackedState, sizeof(dwCloackedState)) == S_OK)
            return dwCloackedState;
    }
    return 0;
}

PFNGetWindowDisplayAffinity pfnGetWindowDisplayAffinity = NULL;

BOOL NTAPI UI_GetWindowDisplayAffinity(HWND Window, PDWORD Affinity) {
    if (NT_GetKUSD()->NtMajorVersion > 6 ||
        (NT_GetKUSD()->NtMajorVersion == 6) && NT_GetKUSD()->NtMinorVersion > 0) {
        if (!pfnGetWindowDisplayAffinity) {
            pfnGetWindowDisplayAffinity = (PFNGetWindowDisplayAffinity)Proc_GetProcAddr(Sys_LoadDll(SysLoadDllUser32), "GetWindowDisplayAffinity");
            if (!pfnGetWindowDisplayAffinity)
                return FALSE;
        }
        return UI_IsDWMComposited() ? pfnGetWindowDisplayAffinity(Window, Affinity) : FALSE;
    }
    return FALSE;
}

PFNSetWindowDisplayAffinity pfnSetWindowDisplayAffinity = NULL;

BOOL NTAPI UI_SetWindowDisplayAffinity(HWND Window, DWORD Affinity) {
    if (NT_GetKUSD()->NtMajorVersion > 6 ||
        (NT_GetKUSD()->NtMajorVersion == 6) && NT_GetKUSD()->NtMinorVersion > 0) {
        if (!pfnSetWindowDisplayAffinity)
            pfnSetWindowDisplayAffinity = (PFNSetWindowDisplayAffinity)Proc_GetProcAddr(Sys_LoadDll(SysLoadDllUser32), "SetWindowDisplayAffinity");
        return pfnSetWindowDisplayAffinity ? pfnSetWindowDisplayAffinity(Window, Affinity) : FALSE;
    }
    return FALSE;
}

PFNSetWindowTheme pfnSetWindowTheme = NULL;

BOOL NTAPI UI_SetTheme(HWND Window) {
    if (NT_GetKUSD()->NtMajorVersion >= 6) {
        if (!pfnSetWindowTheme)
            pfnSetWindowTheme = (PFNSetWindowTheme)Proc_GetProcAddr(Sys_LoadDll(SysLoadDllUxTheme), "SetWindowTheme");
        return pfnSetWindowTheme ? pfnSetWindowTheme(Window, L"Explorer", NULL) == S_OK : FALSE;
    }
    return FALSE;
}

VOID NTAPI UI_SetWindowIcon(HWND Window, HICON Icon) {
    SendMessage(Window, WM_SETICON, ICON_BIG, (LPARAM)Icon);
    SendMessage(Window, WM_SETICON, ICON_SMALL, (LPARAM)Icon);
}

BOOL NTAPI UI_ShellExec(PCWSTR File, PCWSTR Param, UI_SHELLEXEC_VERB Verb, INT ShowCmd, PHANDLE ProcessHandle) {
    SHELLEXECUTEINFOW   stSEIW = { sizeof(SHELLEXECUTEINFOW) };
    BOOL                bRet;
    if (Verb == UIShellExecExplore) {
        WCHAR               szFile[MAX_PATH];
        SFGAOF              dwAttr;
        PIDLIST_ABSOLUTE    pidlDir, pidlFile;
        UINT                uIndexSlash;
        HRESULT             hr;
        uIndexSlash = Str_CchCopyW(szFile, File);
        while (uIndexSlash > 0)
            if (szFile[--uIndexSlash] == '\\')
                break;
        if (uIndexSlash > 0) {
            szFile[uIndexSlash] = '\0';
            if (SHParseDisplayName(szFile, NULL, &pidlDir, 0, &dwAttr) == S_OK) {
                szFile[uIndexSlash] = '\\';
                if (SHParseDisplayName(szFile, NULL, &pidlFile, 0, &dwAttr) == S_OK) {
                    hr = SHOpenFolderAndSelectItems(pidlDir, 1, &pidlFile, 0);
                    CoTaskMemFree(pidlFile);
                    if (hr == S_OK) {
                        CoTaskMemFree(pidlDir);
                        return TRUE;
                    }
                }
                CoTaskMemFree(pidlDir);
            }
        }
    }
    stSEIW.fMask = (ProcessHandle ? SEE_MASK_NOCLOSEPROCESS : SEE_MASK_DEFAULT) | SEE_MASK_INVOKEIDLIST;
    stSEIW.lpFile = File;
    stSEIW.lpParameters = Param;
    if (Verb == UIShellExecOpen)
        stSEIW.lpVerb = L"open";
    else if (Verb == UIShellExecExplore)
        stSEIW.lpVerb = L"explore";
    else if (Verb == UIShellExecRunAs)
        stSEIW.lpVerb = L"runas";
    else if (Verb == UIShellExecProperties)
        stSEIW.lpVerb = L"properties";
    else if (Verb == UIShellExecEdit)
        stSEIW.lpVerb = L"edit";
    else if (Verb == UIShellExecPrint)
        stSEIW.lpVerb = L"print";
    else if (Verb == UIShellExecFind)
        stSEIW.lpVerb = L"find";
    else
        return FALSE;
    stSEIW.nShow = ShowCmd;
    bRet = ShellExecuteExW(&stSEIW);
    if (ProcessHandle && bRet)
        *ProcessHandle = stSEIW.hProcess;
    return bRet;
}

BOOL NTAPI UI_EnableWindowStyle(HWND Window, INT StyleIndex, LONG_PTR StyleFlag, BOOL EnableState) {
    LONG_PTR    lStyle;
    BOOL        bRet;
    bRet = FALSE;
    if (StyleIndex == GWL_STYLE || StyleIndex == GWL_EXSTYLE) {
        NT_ClearLastError();
        lStyle = GetWindowLongPtr(Window, StyleIndex);
        if (lStyle || NT_LastErrorSucceed()) {
            NT_ClearLastError();
            lStyle = SetWindowLongPtr(Window, StyleIndex, COMBINE_FLAGS(lStyle, StyleFlag, EnableState));
            return lStyle || NT_LastErrorSucceed();
        }
    }
    return bRet;
}

HANDLE NTAPI UI_OpenProc(DWORD DesiredAccess, HWND Window) {
    DWORD   dwProcessId;
    GetWindowThreadProcessId(Window, &dwProcessId);
    return RProc_Open(DesiredAccess, dwProcessId);
}

UINT NTAPI UI_GetWindowModuleFileNameEx(HWND Window, PWSTR FileName, UINT FileNameCch) {
    HANDLE  hProc;
    UINT    cCh;
    hProc = UI_OpenProc(NT_GetKUSD()->NtMajorVersion >= 6 ? PROCESS_QUERY_LIMITED_INFORMATION : PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, Window);
    if (hProc) {
        cCh = RProc_GetFullImageNameEx(hProc, FileName, FileNameCch);
        NtClose(hProc);
        return cCh;
    }
    return 0;
}

BOOL NTAPI UI_SetNoNotifyFlag(HWND Window, BOOL EnableState) {
    return EnableState ?
        SetProp(Window, UI_NONOTIFYPROP, (HANDLE)TRUE) :
        (BOOL)(DWORD_PTR)RemoveProp(Window, UI_NONOTIFYPROP);
}

BOOL NTAPI UI_GetNoNotifyFlag(HWND Window) {
    return (BOOL)(DWORD_PTR)GetProp(Window, UI_NONOTIFYPROP);
}

LRESULT NTAPI UI_SetWndTextNoNotify(HWND Window, PCWSTR Text) {
    LRESULT lResult;
    UI_SetNoNotifyFlag(Window, TRUE);
    lResult = SendMessageW(Window, WM_SETTEXT, 0, (LPARAM)Text);
    UI_SetNoNotifyFlag(Window, FALSE);
    return lResult;
}

UINT NTAPI UI_GetWindowTextEx(HWND Window, PWSTR Text, UINT TextCch) {
    UINT    cCh = (UINT)SendMessageW(Window, WM_GETTEXT, TextCch, (LPARAM)Text);
    if (cCh >= TextCch)
        cCh = 0;
    Text[cCh] = '\0';
    return cCh;
}

DWORD NTAPI UI_GetWindowLong(HWND Window, BOOL ClassLong, INT Index, PLONG_PTR Result) {
    LONG_PTR    lResult;
    DWORD       dwError;
    __try {
        NT_ClearLastError();
        lResult = ClassLong ? (LONG_PTR)GetClassLongPtr(Window, Index) : GetWindowLongPtr(Window, Index);
        dwError = lResult ? ERROR_SUCCESS : NT_GetLastError();
    } __except (NT_SEH_NopHandler(NULL)) {
        lResult = 0;
        dwError = ERROR_READ_FAULT;
    }
    *Result = lResult;
    return dwError;
}

BOOL NTAPI UI_MessageLoop(HWND Window) {
    BOOL    bRet;
    MSG     stMsg;
    while (TRUE) {
        bRet = GetMessage(&stMsg, Window, 0, 0);
        if (bRet != 0 && bRet != -1) {
            TranslateMessage(&stMsg);
            DispatchMessage(&stMsg);
        } else
            return bRet == 0;
    }
}

VOID NTAPI UI_GetScreenPos(PPOINT Point, PSIZE Size) {
    if (Point) {
        Point->x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        Point->y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    }
    if (Size) {
        Size->cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        Size->cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }
}

