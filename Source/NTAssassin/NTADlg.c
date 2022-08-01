#include "include\NTAssassin\NTAssassin.h"

LPDLGTEMPLATEW NTAPI Dlg_InitTemplate(_Out_ PDLG_TEMPLATE Template, DWORD Style, DWORD ExtendedStyle, INT X, INT Y, INT Width, INT Height) {
    Template->wMenu = Template->wClass = Template->wTitle = Template->Template.cdit = 0;
    Template->Template.style = Style;
    Template->Template.dwExtendedStyle = ExtendedStyle;
    Template->Template.x = X;
    Template->Template.y = Y;
    Template->Template.cx = Width;
    Template->Template.cy = Height;
    return &Template->Template;
}

BOOL NTAPI Dlg_GetOpenFileNameEx(HWND Owner, PCWSTR Filter, PWSTR File, DWORD CchFile, PCWSTR DefExt) {
    OPENFILENAMEW   stOpenOFNW = { sizeof(OPENFILENAMEW) };
    stOpenOFNW.hwndOwner = Owner;
    stOpenOFNW.lpstrFilter = Filter;
    stOpenOFNW.lpstrFile = File;
    stOpenOFNW.nMaxFile = CchFile;
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

BOOL NTAPI Dlg_ChooseColor(HWND Owner, _Inout_ LPCOLORREF Color) {
    COLORREF acrCustClr[16] = {
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
    stChooseColor.rgbResult = *Color;
    bRet = ChooseColor(&stChooseColor);
    if (bRet)
        *Color = stChooseColor.rgbResult;
    return bRet;
}

BOOL NTAPI Dlg_ChooseFont(HWND Owner, _Out_ PLOGFONTW Font, _Inout_opt_ LPCOLORREF Color) {
    CHOOSEFONTW stChooseFontW = { sizeof(CHOOSEFONTW) };
    BOOL        bRet;
    stChooseFontW.hwndOwner = Owner;
    if (Color) {
        stChooseFontW.Flags = CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST | CF_EFFECTS;
        stChooseFontW.rgbColors = *Color;
    } else
        stChooseFontW.Flags = CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST;
    stChooseFontW.lpLogFont = Font;
    bRet = ChooseFontW(&stChooseFontW);
    if (bRet && Color)
        *Color = stChooseFontW.rgbColors;
    return bRet;
}

BOOL NTAPI Dlg_ScreenSnapshot(_In_ PDLG_SCREENSNAPSHOT ScreenSnapshot) {
    WNDCLASSEXW stWndClsExCaptureW = { sizeof(WNDCLASSEXW), 0, NULL, 0, 0, NULL, NULL, NULL, NULL, NULL, L"NTAssassin.Dlg.ScreenSnapshotClass", NULL };
    ATOM        atomClass;
    HWND        hWnd;
    BOOL        bRet = FALSE;
    stWndClsExCaptureW.style = ScreenSnapshot->dwClassStyle;
    stWndClsExCaptureW.lpfnWndProc = ScreenSnapshot->pfnWndProc;
    stWndClsExCaptureW.hCursor = ScreenSnapshot->hCursor;
    stWndClsExCaptureW.hInstance = ScreenSnapshot->hInstance;
    atomClass = RegisterClassExW(&stWndClsExCaptureW);
    if (atomClass) {
        // Get virtual screen position
        if (GDI_CreateSnapshot(NULL, &ScreenSnapshot->Snapshot)) {
            // Create window and enter message loop
            hWnd = CreateWindowExW(
                ScreenSnapshot->dwExStyle,
                MAKEINTRESOURCEW(atomClass),
                L"NTAssassin.Dlg.ScreenSnapshotClass",
                ScreenSnapshot->dwStyle,
                ScreenSnapshot->Snapshot.Position.x,
                ScreenSnapshot->Snapshot.Position.y,
                ScreenSnapshot->Snapshot.Size.cx,
                ScreenSnapshot->Snapshot.Size.cy,
                HWND_DESKTOP,
                NULL,
                ScreenSnapshot->hInstance,
                ScreenSnapshot->lParam);
            bRet = hWnd && UI_MessageLoop(NULL);
            GDI_DeleteSnapshot(&ScreenSnapshot->Snapshot);
        }
        UnregisterClassW(MAKEINTRESOURCEW(atomClass), stWndClsExCaptureW.hInstance);
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
    DLG_RESIZEDPROC pfnResizedProc;
} DLG_SETRESIZINGSUBCLASS_REF, *PDLG_SETRESIZINGSUBCLASS_REF;

static LRESULT CALLBACK Dlg_SetResizingSubclass_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_DPICHANGED) {
        PDLG_SETRESIZINGSUBCLASS_REF    pstRef = (PDLG_SETRESIZINGSUBCLASS_REF)dwRefData;
        pstRef->dwOldDPIX = pstRef->dwNewDPIX;
        pstRef->dwOldDPIY = pstRef->dwNewDPIY;
        pstRef->dwNewDPIX = LOWORD(wParam);
        pstRef->dwNewDPIY = HIWORD(wParam);
        DPI_ScaleInt(&pstRef->lMinWidth, pstRef->dwOldDPIX, pstRef->dwNewDPIX);
        DPI_ScaleInt(&pstRef->lMinHeight, pstRef->dwOldDPIY, pstRef->dwNewDPIY);
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    } else if (uMsg == WM_SIZING) {
        PDLG_SETRESIZINGSUBCLASS_REF    pstRef = (PDLG_SETRESIZINGSUBCLASS_REF)dwRefData;
        PRECT pRect = (RECT*)lParam;
        if (pstRef->lMinWidth && pRect->right < pRect->left + pstRef->lMinWidth)
            pRect->right = pRect->left + pstRef->lMinWidth;
        if (pstRef->lMinHeight && pRect->bottom < pRect->top + pstRef->lMinHeight)
            pRect->bottom = pRect->top + pstRef->lMinHeight;
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
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
    } else if (uMsg == WM_DESTROY) {
        Mem_Free((PVOID)dwRefData);
    }
    return DefSubclassProc(hDlg, uMsg, wParam, lParam);
}

BOOL NTAPI Dlg_SetResizingSubclass(HWND Dialog, LONG MinWidth, LONG MinHeight, DLG_RESIZEDPROC ResizedProc) {
    PDLG_SETRESIZINGSUBCLASS_REF pstRef;
    pstRef = Mem_Alloc(sizeof(DLG_SETRESIZINGSUBCLASS_REF));
    if (!pstRef) {
        return FALSE;
    }
    pstRef->lMinWidth = MinWidth;
    pstRef->lMinHeight = MinHeight;
    DPI_FromWindow(Dialog, &pstRef->dwOldDPIX, &pstRef->dwOldDPIY);
    pstRef->dwNewDPIX = pstRef->dwOldDPIX;
    pstRef->dwNewDPIY = pstRef->dwOldDPIY;
    pstRef->pfnResizedProc = ResizedProc;
    return SetWindowSubclass(Dialog, Dlg_SetResizingSubclass_DlgProc, 0, (DWORD_PTR)pstRef);
}

typedef struct _DLG_SETTREEVIEWPROPERTYSHEETSUBCLASS_REF {
    HWND                        Dialog;
    HWND                        TreeView;
    RECT                        SheetRect;
    PDLG_TREEVIEWPROPSHEETPAGE  Sheets;
    UINT                        Count;
} DLG_SETTREEVIEWPROPERTYSHEETSUBCLASS_REF, *PDLG_SETTREEVIEWPROPERTYSHEETSUBCLASS_REF;

static LRESULT CALLBACK Dlg_SetTreeViewPropertySheetSubclass_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_NOTIFY) {
        PDLG_SETTREEVIEWPROPERTYSHEETSUBCLASS_REF pstRef = (PDLG_SETTREEVIEWPROPERTYSHEETSUBCLASS_REF)dwRefData;
        LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;
        if (pnmtv->hdr.hwndFrom == pstRef->TreeView &&
            pnmtv->hdr.code == TVN_SELCHANGED) {
            ShowWindow((HWND)pnmtv->itemOld.lParam, SW_HIDE);
            ShowWindow((HWND)pnmtv->itemNew.lParam, SW_SHOW);
        }
    } else if (uMsg == WM_DESTROY) {
        Mem_Free((PVOID)dwRefData);
    }
    return DefSubclassProc(hDlg, uMsg, wParam, lParam);
}

VOID Dlg_SetTreeViewPropertySheetSubclass_InitSheet(PDLG_SETTREEVIEWPROPERTYSHEETSUBCLASS_REF pstRef, HTREEITEM TreeItem, PDLG_TREEVIEWPROPSHEETPAGE Sheets, UINT Count) {
    TVINSERTSTRUCT stTVIInsert = { 0, TVI_LAST, { TVIF_TEXT | TVIF_PARAM } };
    UINT i;
    for (i = 0; i < Count; i++) {
        Sheets[i].Handle = CreateDialogParam(Sheets[i].Instance, MAKEINTRESOURCE(Sheets[i].DlgResID), pstRef->Dialog, Sheets[i].DlgProc, Sheets[i].Param);
        if (Sheets[i].Handle) {
            stTVIInsert.hParent = TreeItem;
            stTVIInsert.item.pszText = Sheets[i].DisplayName;
            stTVIInsert.item.lParam = (LPARAM)Sheets[i].Handle;
            Sheets[i].TreeItem = (HTREEITEM)SendMessage(pstRef->TreeView, TVM_INSERTITEM, 0, (LPARAM)&stTVIInsert);
            if (Sheets[i].TreeItem) {
                SetWindowPos(
                    Sheets[i].Handle,
                    NULL,
                    pstRef->SheetRect.left,
                    pstRef->SheetRect.top,
                    pstRef->SheetRect.right - pstRef->SheetRect.left,
                    pstRef->SheetRect.bottom - pstRef->SheetRect.top,
                    SWP_NOZORDER | SWP_HIDEWINDOW);
                if (Sheets[i].Count) {
                    Dlg_SetTreeViewPropertySheetSubclass_InitSheet(pstRef, Sheets[i].TreeItem, Sheets[i].SubItems, Sheets[i].Count);
                }
            } else {
                EndDialog(Sheets[i].Handle, -1);
                Sheets[i].Handle = NULL;
            }
        }
    }
}

BOOL NTAPI Dlg_SetTreeViewPropertySheetSubclass(HWND Dialog, HWND TreeView, PRECT SheetRect, _In_ PDLG_TREEVIEWPROPSHEETPAGE Sheets, UINT Count) {
    PDLG_SETTREEVIEWPROPERTYSHEETSUBCLASS_REF pstRef;
    pstRef = Mem_Alloc(sizeof(DLG_SETTREEVIEWPROPERTYSHEETSUBCLASS_REF));
    if (!pstRef) {
        return FALSE;
    }
    pstRef->Dialog = Dialog;
    pstRef->TreeView = TreeView;
    pstRef->SheetRect.left = SheetRect->left;
    pstRef->SheetRect.top = SheetRect->top;
    pstRef->SheetRect.right = SheetRect->right;
    pstRef->SheetRect.bottom = SheetRect->bottom;
    pstRef->Sheets = Sheets;
    pstRef->Count = Count;
    Dlg_SetTreeViewPropertySheetSubclass_InitSheet(pstRef, (HTREEITEM)TVI_ROOT, Sheets, Count);
    return SetWindowSubclass(Dialog, Dlg_SetTreeViewPropertySheetSubclass_DlgProc, 0, (DWORD_PTR)pstRef);
}