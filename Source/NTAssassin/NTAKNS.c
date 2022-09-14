#include "include\NTAssassin\NTAKNS.h"
#include "include\NTAssassin\NTAStr.h"
#include "include\NTAssassin\NTAUI.h"
#include "include\NTAssassin\NTAMem.h"
#include "include\NTAssassin\NTADPI.h"
#include "include\NTAssassin\NTAShell.h"
#include "include\NTAssassin\NTASys.h"

#define KNS_MAX_VER_CCH 48
#define KNS_DEFAULT_DLGFONTSIZE 20

static PKNS_INFO   lpKNSInfo;

static DLGPROC     lpfnMainDlgProc;
static HWND        hMainDlg;

static HFONT       hFontBig, hFontSmall;
static HCURSOR     hCurArrow, hCurPointer;
static HICON       hIconDefault = NULL;
static WCHAR       szVersion[KNS_MAX_VER_CCH];

BOOL NTAPI KNS_GetVersionStringEx(PWSTR StrVersion, UINT ChCount) {
    PKNS_VERSION_INFO   lpstVer = &lpKNSInfo->Version;
    LPWSTR              lpszEdition;
    if (lpstVer->Type == KNS_VERSION_ALPHA)
        lpszEdition = L"Alpha";
    else if (lpstVer->Type == KNS_VERSION_BETA)
        lpszEdition = L"Beta";
    else if (lpstVer->Type == KNS_VERSION_RC)
        lpszEdition = L"RC";
    else if (lpstVer->Type == KNS_VERSION_GA)
        lpszEdition = L"GA";
    else if (lpstVer->Type == KNS_VERSION_RELEASE)
        lpszEdition = L"Release";
    else
        lpszEdition = L"";
    return Str_PrintfExW(StrVersion, ChCount, L"V%u.%u.%u.%u %s", lpstVer->Major, lpstVer->Minor, lpstVer->Revision, lpstVer->Build, lpszEdition) > 0;
}

HICON NTAPI KNS_GetIcon() {
    return hIconDefault;
}

typedef struct _KNS_SETBANNERSUBCLASS_REF {
    HWND        hStatic;
    LONG        lWidth;
    LONG        lHeight;
    HICON       hIcon;
    LONG        lIconHotspot;
    HFONT       hFontBig;
    HFONT       hFontSmall;
    LPCWSTR     lpszName;
    LPCWSTR     lpszComment;
} KNS_SETBANNERSUBCLASS_REF, * PKNS_SETBANNERSUBCLASS_REF;

static LRESULT CALLBACK KNS_SetBannerSubclass_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    PKNS_SETBANNERSUBCLASS_REF  pstRef = (PKNS_SETBANNERSUBCLASS_REF)dwRefData;
    RECT                        rcText;
    if (uMsg == WM_PAINT) {
        UI_WINDBPAINT   stPaint;
        HDC             hDC;
        hDC = UI_BeginPaint(hWnd, &stPaint);
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, RGB(255, 255, 255));
        GDI_FillSolidRect(hDC, &stPaint.Rect, lpKNSInfo->UI.MainColor);
        rcText.left = rcText.top = KNS_BANNER_MARGIN;
        if (pstRef->hIcon)
            rcText.left += (pstRef->lIconHotspot * 2 + KNS_BANNER_MARGIN);
        rcText.right = pstRef->lWidth - KNS_BANNER_MARGIN;
        rcText.bottom = pstRef->lHeight - KNS_BANNER_MARGIN;
        if (pstRef->hIcon)
            GDI_DrawIcon(
                hDC,
                pstRef->hIcon,
                KNS_BANNER_MARGIN,
                (pstRef->lHeight - pstRef->lHeight * KNS_BANNER_ICON_SCALE) / 2,
                pstRef->lHeight * KNS_BANNER_ICON_SCALE,
                pstRef->lHeight * KNS_BANNER_ICON_SCALE
            );
        if (pstRef->lpszName) {
            SelectObject(hDC, pstRef->hFontBig);
            DrawTextW(hDC, pstRef->lpszName, -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
        UI_EndPaint(hWnd, &stPaint);
    } else if (uMsg == WM_WINDOWPOSCHANGED) {
        PWINDOWPOS lpstWndPos = (PWINDOWPOS)lParam;
        if (pstRef->lWidth != lpstWndPos->cx || pstRef->lHeight != lpstWndPos->cy) {
            pstRef->lWidth = lpstWndPos->cx;
            pstRef->lHeight = lpstWndPos->cy;

            // Scale fonts
            HFONT hFont;
            ENUMLOGFONTEXDVW FontInfo;
            if (GDI_GetDefaultFont(&FontInfo, 0)) {
                FontInfo.elfEnumLogfontEx.elfLogFont.lfWidth = 0;
                FontInfo.elfEnumLogfontEx.elfLogFont.lfHeight = pstRef->lHeight * KNS_BANNER_FONTBIG_SCALE;
                hFont = CreateFontIndirectExW(&FontInfo);
                if (hFont) {
                    if (pstRef->hFontBig) {
                        DeleteObject(pstRef->hFontBig);
                    }
                    pstRef->hFontBig = hFont;
                }
                FontInfo.elfEnumLogfontEx.elfLogFont.lfHeight = pstRef->lHeight * KNS_BANNER_FONTSMALL_SCALE;
                hFont = CreateFontIndirectExW(&FontInfo);
                if (hFont) {
                    if (pstRef->hFontSmall) {
                        DeleteObject(pstRef->hFontSmall);
                    }
                    pstRef->hFontSmall = hFont;
                }
            }

            // Scale icon
            ICONINFO stIconInfo;
            stIconInfo.fIcon = TRUE;
            stIconInfo.xHotspot = 0;
            HICON hIcon = LoadImageW(NT_GetImageBase(), MAKEINTRESOURCEW(lpKNSInfo->UI.IconResID), IMAGE_ICON, pstRef->lHeight * KNS_BANNER_ICON_SCALE, pstRef->lHeight * KNS_BANNER_ICON_SCALE, 0);
            if (hIcon) {
                if (GetIconInfo(hIcon, &stIconInfo) && stIconInfo.xHotspot) {
                    if (pstRef->hIcon) {
                        DestroyIcon(pstRef->hIcon);
                    }
                    pstRef->lIconHotspot = stIconInfo.xHotspot;
                    pstRef->hIcon = hIcon;
                    DeleteObject(stIconInfo.hbmMask);
                    DeleteObject(stIconInfo.hbmColor);
                } else {
                    DestroyIcon(hIcon);
                    hIcon = NULL;
                }
            }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

VOID KNS_SetBannerSubclass(HWND hBanner) {
    PKNS_SETBANNERSUBCLASS_REF  pstRef;
    RECT                        rcBanner;
    WINDOWPOS                   stWndPos;
    pstRef = Mem_Alloc(sizeof(KNS_SETBANNERSUBCLASS_REF));
    pstRef->hStatic = hBanner;
    pstRef->hFontBig = pstRef->hFontSmall = NULL;
    pstRef->hIcon = NULL;
    pstRef->lpszComment = szVersion;
    pstRef->lpszName = lpKNSInfo->Name;
    UI_GetWindowRect(hBanner, &rcBanner);
    stWndPos.cx = rcBanner.right - rcBanner.left;
    stWndPos.cy = rcBanner.bottom - rcBanner.top;
    SetWindowSubclass(hBanner, KNS_SetBannerSubclass_Proc, 0, (DWORD_PTR)pstRef);
    stWndPos.x = rcBanner.left;
    stWndPos.y = rcBanner.top;
    stWndPos.hwnd = hBanner;
    stWndPos.hwndInsertAfter = NULL;
    stWndPos.flags = SWP_NOZORDER;
    SendMessageW(hBanner, WM_WINDOWPOSCHANGED, 0, (LPARAM)&stWndPos);
    UI_Redraw(hBanner);
}

static INT_PTR CALLBACK KNS_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITDIALOG) {
        if (hIconDefault) {
            SendMessageW(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIconDefault);
            SendMessageW(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIconDefault);
        }
        if (lpKNSInfo->UI.BannerCtlID != -1)
            KNS_SetBannerSubclass(GetDlgItem(hDlg, lpKNSInfo->UI.BannerCtlID));
    }
    return lpfnMainDlgProc(hDlg, uMsg, wParam, lParam);
}

VOID NTAPI KNS_SetDialogSubclass(HWND Dialog, DLG_RESIZEDPROC ResizedProc) {
    DPI_SetAutoAdjustSubclass(Dialog, NULL);
    if (ResizedProc) {
        Dlg_SetResizingSubclass(Dialog, TRUE, ResizedProc);
    }
}

UINT_PTR NTAPI KNS_Startup(PKNS_INFO KNSInfo) {
    lpKNSInfo = KNSInfo;
    lpfnMainDlgProc = KNSInfo->UI.DlgProc;
    hCurArrow = LoadImageW(NULL, MAKEINTRESOURCE(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
    hCurPointer = LoadImageW(NULL, MAKEINTRESOURCE(OCR_HAND), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
    if (KNSInfo->I18N.LangCount)
        I18N_InitEx(KNSInfo->I18N.Langs, KNSInfo->I18N.LangCount, KNSInfo->I18N.ItemCount, NULL);
    if (!KNS_GetVersionString(szVersion))
        szVersion[0] = '\0';
    if (lpKNSInfo->UI.IconResID != -1)
        hIconDefault = LoadImageW(NT_GetImageBase(), MAKEINTRESOURCEW(lpKNSInfo->UI.IconResID), IMAGE_ICON, 0, 0, 0);
    ENUMLOGFONTEXDVW FontInfo;
    if (GDI_GetDefaultFont(&FontInfo, 0)) {
        FontInfo.elfEnumLogfontEx.elfLogFont.lfWidth = 0;
        FontInfo.elfEnumLogfontEx.elfLogFont.lfHeight = ICON_SIDE;
        hFontBig = CreateFontIndirectExW(&FontInfo);
        FontInfo.elfEnumLogfontEx.elfLogFont.lfHeight = ICON_SIDE / 2;
        hFontSmall = CreateFontIndirectExW(&FontInfo);
    } else {
        hFontBig = hFontSmall = NULL;
    }
    hMainDlg = CreateDialogParamW(NT_GetImageBase(), MAKEINTRESOURCEW(KNSInfo->UI.DlgResID), NULL, KNS_DlgProc, 0);
    HACCEL hAccel;
    if (KNSInfo->UI.DlgAccResID != -1) {
        hAccel = LoadAcceleratorsW(NT_GetImageBase(), MAKEINTRESOURCEW(KNSInfo->UI.DlgAccResID));
    } else {
        hAccel = NULL;
    }
    UINT_PTR ulRet;
    ulRet = Dlg_MessageLoop(NULL, hMainDlg, hAccel, &ulRet) ? ulRet : (UINT_PTR)NT_GetLastError();
    if (hFontBig) {
        DeleteObject(hFontBig);
    }
    if (hFontSmall) {
        DeleteObject(hFontSmall);
    }
    if (hAccel) {
        DestroyAcceleratorTable(hAccel);
    }
    return ulRet;
}

INT NTAPI KNS_MsgBox(HWND Owner, PCWSTR Text, PCWSTR Title, UINT Type) {
    return Dlg_MsgBox(
        Owner ? Owner : hMainDlg,
        IS_INTRESOURCE(Text) ? I18N_GetString((UINT_PTR)Text) : Text,
        Title ? Title : lpKNSInfo->Name,
        Type);
}

VOID NTAPI KNS_ErrorMsgBox(HWND Owner, DWORD Error) {
    Sys_ErrorMsgBox(Owner, lpKNSInfo->Name, Error);
}

VOID NTAPI KNS_StatusMsgBox(HWND Owner, NTSTATUS Status) {
    Sys_StatusMsgBox(Owner, lpKNSInfo->Name, Status);
}

VOID NTAPI KNS_LastErrorMsgBox(HWND Owner) {
    Sys_LastErrorMsgBox(Owner, lpKNSInfo->Name);
}

VOID NTAPI KNS_LastStatusMsgBox(HWND Owner) {
    Sys_LastStatusMsgBox(Owner, lpKNSInfo->Name);
}

VOID NTAPI KNS_OpenHomePage() {
    Shell_Exec(lpKNSInfo->OnlineService.HomePage, NULL, ShellExecOpen, SW_SHOWNORMAL, NULL);
}

#define PADDING_X   14
#define PADDING_Y   20

static RECT rcDlgAbout, rcHomePage;

static INT_PTR CALLBACK KNS_DlgAbout_DlgProcW(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITDIALOG) {
        GetClientRect(hDlg, &rcDlgAbout);
        rcHomePage.left = PADDING_X;
        rcHomePage.bottom = rcDlgAbout.bottom - PADDING_Y;
        rcHomePage.top = rcHomePage.bottom - ICON_SIDE / 2;
        rcHomePage.right = rcDlgAbout.right - PADDING_X;
        return TRUE;
    } else if (uMsg == WM_PAINT) {
        UI_WINDBPAINT pt;
        UI_BeginPaint(hDlg, &pt);
        // Background
        GDI_FillSolidRect(pt.DC, &pt.Rect, lpKNSInfo->UI.MainColor);
        SetTextColor(pt.DC, RGB(255, 255, 255));
        SetBkColor(pt.DC, lpKNSInfo->UI.MainColor);
        SetBkMode(pt.DC, TRANSPARENT);
        // Icon
        DrawIconEx(pt.DC, PADDING_X, PADDING_Y, hIconDefault, ICON_SIDE, ICON_SIDE, 0, NULL, DI_NORMAL);
        // Name
        if (lpKNSInfo->Name) {
            RECT rcName = { PADDING_X * 2 + ICON_SIDE, PADDING_Y ,rcDlgAbout.right - PADDING_X, PADDING_Y + ICON_SIDE };
            SelectObject(pt.DC, hFontBig);
            DrawTextW(pt.DC, lpKNSInfo->Name, -1, &rcName, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
        // Version
        if (szVersion[0]) {
            RECT rcVer = { PADDING_X , PADDING_Y + ICON_SIDE ,rcDlgAbout.right - PADDING_X, PADDING_Y * 2 + PADDING_Y / 2 + ICON_SIDE };
            SelectObject(pt.DC, hFontSmall);
            DrawTextW(pt.DC, szVersion, -1, &rcVer, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
        // Homepage
        DrawTextW(pt.DC, lpKNSInfo->OnlineService.HomePage, -1, &rcHomePage, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        GDI_FrameRect(pt.DC, &rcHomePage, 1, PATCOPY);
        UI_EndPaint(hDlg, &pt);
    } else if (uMsg == WM_MOUSEMOVE) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        SetCursor(UI_PtInRect(&rcHomePage, pt) ? hCurPointer : hCurArrow);
    } else if (uMsg == WM_LBUTTONUP) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (UI_PtInRect(&rcHomePage, pt))
            KNS_OpenHomePage();
        else {
            EndDialog(hDlg, FALSE);
            SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
        }
    } else if (uMsg == WM_CLOSE || uMsg == WM_KEYUP || uMsg == WM_RBUTTONUP) {
        EndDialog(hDlg, FALSE);
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    }
    return 0;
}

VOID NTAPI KNS_DlgAbout(HWND hwndOwner) {
    DLG_TEMPLATE stDlgTemplate;
    DialogBoxIndirectParamW(
        NULL,
        Dlg_InitTemplate(
            &stDlgTemplate,
            DS_CENTER | WS_POPUP,
            WS_EX_CONTROLPARENT,
            0,
            0,
            200,
            100),
        hwndOwner,
        KNS_DlgAbout_DlgProcW, 0);
}