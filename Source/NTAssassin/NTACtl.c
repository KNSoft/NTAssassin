#include "include\NTAssassin\NTACtl.h"
#include "include\NTAssassin\NTAI18N.h"
#include "include\NTAssassin\NTANT.h"
#include "include\NTAssassin\NTAStr.h"
#include "include\NTAssassin\NTAUI.h"

HMENU NTAPI Ctl_CreateMenuEx(_Inout_ PCTL_MENU Menus, _In_ UINT Count, _In_opt_ HMENU Parent) {
    HMENU hMenu = Parent ? Parent : CreateMenu();
    if (hMenu) {
        UINT        i, uFlags;
        PCWSTR      pszText;
        UINT_PTR    uIDNewItem;
        for (i = 0; i < Count; i++) {
            pszText = Menus[i].Text;
            if (IS_INTRESOURCE(pszText))
                pszText = I18N_GetString((DWORD_PTR)Menus[i].Text);
            if (Menus[i].SubMenuCount) {
                if ((Menus[i].Handle = CreatePopupMenu()) != NULL &&
                    (uIDNewItem = (UINT_PTR)Ctl_CreateMenuEx(Menus[i].SubMenu, Menus[i].SubMenuCount, Menus[i].Handle)) != 0) {
                    uFlags = Menus[i].Flags | MF_POPUP;
                } else {
                    break;
                }
            } else {
                Menus[i].Handle = NULL;
                uIDNewItem = Menus[i].ID;
                uFlags = Menus[i].Flags;
            }
            if (!AppendMenuW(hMenu, uFlags, uIDNewItem, pszText)) {
                break;
            }
        }
        if (i < Count) {
            DWORD dwError = NT_GetLastError();
            Ctl_DestroyMenuEx(Menus, Count, hMenu);
            NT_SetLastError(dwError);
            hMenu = NULL;
        }
    }
    return hMenu;
}

VOID NTAPI Ctl_DestroyMenuEx(_In_ PCTL_MENU Menus, _In_ UINT Count, HMENU Menu) {
    UINT i;
    if (Menu) {
        DestroyMenu(Menu);
    }
    for (i = 0; i < Count; i++) {
        Ctl_DestroyMenuEx(Menus[i].SubMenu, Menus[i].SubMenuCount, Menus[i].Handle);
    }
}

static LRESULT NTAPI Ctl_PropertySheetPageDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    TCHAR   szClassName[MAX_CLASSNAME_CCH];
    if (uMsg == WM_CTLCOLORDLG) {
        return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
    } else if (uMsg == WM_CTLCOLORSTATIC) {
        SetBkMode((HDC)wParam, TRANSPARENT);
        if (GetClassName((HWND)lParam, szClassName, ARRAYSIZE(szClassName))) {
            if (Str_EqualW(szClassName, WC_EDITW)) {
                if (GetWindowLongPtr((HWND)lParam, GWL_STYLE) & ES_READONLY) {
                    return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
                }
            }  
        } 
        return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
    }
    return DefSubclassProc(hDlg, uMsg, wParam, lParam);
}

static LRESULT NTAPI Ctl_PropertySheetDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_NOTIFY) {
        LPNMHDR lpnmhdr = (LPNMHDR)lParam;
        INT     iSelected;
        TCITEM  stTCI;
        RECT    rcTab;
        if (lpnmhdr->idFrom == dwRefData) {
            if (lpnmhdr->code == TCN_SELCHANGING || lpnmhdr->code == TCN_SELCHANGE) {
                iSelected = (INT)SendMessage(lpnmhdr->hwndFrom, TCM_GETCURSEL, 0, 0);
                if (iSelected != -1) {
                    stTCI.mask = TCIF_PARAM;
                    if (SendMessage(lpnmhdr->hwndFrom, TCM_GETITEMW, iSelected, (LPARAM)&stTCI)) {
                        if (lpnmhdr->code == TCN_SELCHANGING) {
                            ShowWindow((HWND)stTCI.lParam, SW_HIDE);
                        } else {
                            UI_GetRelativeRect(lpnmhdr->hwndFrom, hDlg, &rcTab);
                            SendMessage(lpnmhdr->hwndFrom, TCM_ADJUSTRECT, FALSE, (LPARAM)&rcTab);
                            SetWindowPos((HWND)stTCI.lParam, NULL, rcTab.left, rcTab.top, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, SWP_NOZORDER | SWP_SHOWWINDOW);
                        }
                    }
                }
            }
        }
    }
    return DefSubclassProc(hDlg, uMsg, wParam, lParam);
}

BOOL NTAPI Ctl_SetPropertySheetEx(HWND Dialog, INT TabCtlID, _In_ PCTL_PROPSHEETPAGE Sheets, _In_ UINT SheetCount, LPARAM Param) {
    BOOL    bRet = TRUE;
    HWND    hTab = GetDlgItem(Dialog, TabCtlID);
    UINT    i;
    TCITEM  stTCI;
    NMHDR   stnmhdr;
    stTCI.mask = TCIF_TEXT | TCIF_PARAM;
    SendMessage(hTab, TCM_DELETEALLITEMS, 0, 0);
    for (i = 0; i < SheetCount; i++) {
        Sheets[i].Handle = CreateDialogParam(Sheets[i].Instance, MAKEINTRESOURCE(Sheets[i].DlgResID), Dialog, Sheets[i].DlgProc, Param);
        if (Sheets[i].Handle &&
            SetWindowSubclass(Sheets[i].Handle, Ctl_PropertySheetPageDialogProc, 0, 0)) {
            stTCI.pszText = (LPTSTR)(IS_INTRESOURCE(Sheets[i].Title) ? I18N_GetString((DWORD_PTR)Sheets[i].Title) : Sheets[i].Title);
            stTCI.lParam = (LPARAM)Sheets[i].Handle;
            SendMessage(hTab, TCM_INSERTITEM, i, (LPARAM)&stTCI);
        } else {
            bRet = FALSE;
            break;
        }
    }
    if (bRet) {
        bRet = SetWindowSubclass(Dialog, Ctl_PropertySheetDialogProc, 0, TabCtlID);
        if (SheetCount) {
            SendMessage(hTab, TCM_SETCURSEL, 0, 0);
            stnmhdr.hwndFrom = hTab;
            stnmhdr.idFrom = TabCtlID;
            stnmhdr.code = TCN_SELCHANGE;
            SendMessage(Dialog, WM_NOTIFY, TabCtlID, (LPARAM)&stnmhdr);
        }
    }
    return bRet;
}

BOOL NTAPI Ctl_InitListCtlEx(HWND List, _In_ PCTL_LISTCTL_COLUME Cols, _In_ UINT ColCount, LONG_PTR ExStyle) {
    BOOL        bRet = TRUE;
    UINT        i;
    LVCOLUMN    stLVCol = { LVCF_TEXT | LVCF_WIDTH };
    if (ExStyle) {
        SendMessage(List, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, ExStyle);
    }
    for (i = 0; i < ColCount; i++) {
        stLVCol.cx = Cols[i].Width;
        stLVCol.pszText = (LPTSTR)(IS_INTRESOURCE(Cols[i].Title) ? I18N_GetString((UINT_PTR)Cols[i].Title) : Cols[i].Title);
        if (SendMessage(List, LVM_INSERTCOLUMN, i, (LPARAM)&stLVCol) == -1) {
            bRet = FALSE;
            break;
        }
    }
    UI_SetTheme(List);
    return bRet;
}

BOOL NTAPI Ctl_InitComboBoxEx(HWND ComboBox, _In_ PCTL_COMBOBOXCTL_ITEM Items, _In_ UINT ItemCount, BOOL SetParam) {
    PCTSTR      psz;
    UINT        u;
    LONG_PTR    i;
    for (u = 0; u < ItemCount; u++) {
        psz = Items[u].String;
        if (IS_INTRESOURCE(psz)) {
            psz = I18N_GetString((UINT_PTR)psz);
        }
        i = SendMessage(ComboBox, CB_ADDSTRING, 0, (LPARAM)psz);
        if (i >= 0) {
            if (SetParam) {
                SendMessage(ComboBox, CB_SETITEMDATA, i, Items[u].Param);
            }
        } else {
            // CB_ERR or CB_ERRSPACE...
            break;
        }
    }
    return !(u < ItemCount);
}

HTREEITEM NTAPI Ctl_EnumTreeViewItems(HWND TreeView, BOOL BFS, _In_ CTL_TREEVIEWITEMENUMPROC TreeItemEnumProc, LPARAM Param) {
    UINT        uDepth = 0;
    HTREEITEM   hItem, hItemTemp;
    hItem = (HTREEITEM)SendMessage(TreeView, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    do {
        if (!TreeItemEnumProc(TreeView, hItem, uDepth, Param)) {
            return hItem;
        }
        hItemTemp = hItem;
        hItem = (HTREEITEM)SendMessage(TreeView, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItemTemp);
        if (hItem) {
            uDepth++;
            continue;
        }
        hItem = (HTREEITEM)SendMessage(TreeView, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItemTemp);
        if (hItem)
            continue;
        while (--uDepth) {
            hItem = (HTREEITEM)SendMessage(TreeView, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItemTemp);
            hItemTemp = (HTREEITEM)SendMessage(TreeView, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
            if (hItemTemp) {
                hItem = hItemTemp;
                break;
            } else
                hItemTemp = hItem;
        }
    } while (uDepth);
    return NULL;
}

LRESULT NTAPI Ctl_ComboBoxSetSelect(HWND ComboBox, INT ItemIndex) {
    LRESULT lResult = SendMessage(ComboBox, CB_SETCURSEL, ItemIndex, 0);
    INT_PTR iID;
    if (UI_GetWindowLong(ComboBox, FALSE, GWLP_ID, &iID))
        SendMessage(GetParent(ComboBox), WM_COMMAND, MAKEWPARAM(iID, CBN_SELCHANGE), (LPARAM)ComboBox);
    return lResult;
}