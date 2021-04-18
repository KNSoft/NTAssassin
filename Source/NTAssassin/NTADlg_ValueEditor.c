#include "NTAssassin.h"

#define PADDING_X   14
#define PADDING_Y   20
#define CONTROL_H   28
#define BUTTON_W    124

#define IDC_EDIT        1001
#define IDC_LISTVIEW    1002
#define IDC_RESETBTN    1003
#define IDC_OKBTN       1004

INT Dlg_ValueEditor_FormatValueEx(PDLG_VALUEEDITOR lpstDVE, LPWSTR lpszBuff, INT iCchBuff, QWORD qwValue) {
    INT     iTemp;
    if (lpstDVE->Flags & DVE_VALUE_HEXQWORD)
        iTemp = Str_CchPrintfExW(lpszBuff, iCchBuff, L"0x%016llX", qwValue);
    else
        iTemp = Str_CchPrintfExW(lpszBuff, iCchBuff, L"0x%08X", (DWORD)qwValue);
    return iTemp;
}

#define Dlg_ValueEditor_FormatValue(lpstDVE, lpszBuff, qwValue) Dlg_ValueEditor_FormatValueEx(lpstDVE, lpszBuff, ARRAYSIZE(lpszBuff), qwValue)

VOID Dlg_ValueEditor_SetValue(PDLG_VALUEEDITOR lpstDVE, HWND hDlg, QWORD qwValue) {
    WCHAR   szValue[MAX_QWORD_IN_HEX_CCH + 2];
    UI_SetDlgItemText(
        hDlg,
        IDC_EDIT,
        Dlg_ValueEditor_FormatValue(lpstDVE, szValue, qwValue) ? szValue : NULL);
    lpstDVE->Reserved = qwValue;
}

VOID Dlg_ValueEditor_AddItems(HWND hListView, PDLG_VALUEEDITOR lpstDVE) {
    LVITEMW stLVI;
    WCHAR   szValue[MAX_QWORD_IN_HEX_CCH + 2];
    QWORD   qwTemp = lpstDVE->qwValue;
    UINT    u;
    INT     i;
    stLVI.iItem = MAXINT;
    for (u = 0; u < lpstDVE->uConsts; u++) {
        stLVI.iSubItem = 0;
        stLVI.mask = LVIF_TEXT | LVIF_PARAM;
        stLVI.lParam = (LPARAM)&lpstDVE->lpstConsts[u];
        stLVI.pszText = (LPWSTR)lpstDVE->lpstConsts[u].Name;
        stLVI.iItem = (INT)SendMessageW(hListView, LVM_INSERTITEMW, 0, (LPARAM)&stLVI);
        if (stLVI.iItem != -1) {
            stLVI.mask = LVIF_TEXT;
            if ((qwTemp & lpstDVE->lpstConsts[u].Value) == lpstDVE->lpstConsts[u].Value) {
                ListView_SetCheckState(hListView, stLVI.iItem, TRUE);
                qwTemp &= ~lpstDVE->lpstConsts[u].Value;
            }
            stLVI.iSubItem++;
            i = Dlg_ValueEditor_FormatValue(lpstDVE, szValue, lpstDVE->lpstConsts[u].Value);
            stLVI.pszText = i ? szValue : NULL;
            SendMessageW(hListView, LVM_SETITEMW, 0, (LPARAM)&stLVI);
            stLVI.iSubItem++;
            stLVI.pszText = (LPWSTR)lpstDVE->lpstConsts[u].Info;
            SendMessageW(hListView, LVM_SETITEMW, 0, (LPARAM)&stLVI);
        }
        stLVI.iItem++;
    }
    if (qwTemp) {
        stLVI.mask = LVIF_TEXT | LVIF_PARAM;
        stLVI.iSubItem = 0;
        stLVI.lParam = 0;
        stLVI.pszText = (LPWSTR)(lpstDVE->lpstr[6] ? lpstDVE->lpstr[6] : L"(Unknow)");
        if (SendMessageW(hListView, LVM_INSERTITEMW, 0, (LPARAM)&stLVI) != -1) {
            ListView_SetCheckState(hListView, stLVI.iItem, TRUE);
            stLVI.mask = LVIF_TEXT;
            stLVI.iSubItem++;
            i = Dlg_ValueEditor_FormatValue(lpstDVE, szValue, qwTemp);
            stLVI.pszText = i ? szValue : NULL;
            SendMessageW(hListView, LVM_SETITEMW, 0, (LPARAM)&stLVI);
        }
    }
}

INT_PTR CALLBACK Dlg_ValueEditor_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITDIALOG) {
        PDLG_VALUEEDITOR    lpstDVE;
        RECT                rcClient;
        HWND                hEdit, hListView, hBtnReset, hBtnOK;
        PCWSTR              lpsz;
        INT                 iX, iY;

        // Initialize
        lpstDVE = (PDLG_VALUEEDITOR)lParam;
        lpstDVE->Reserved = lpstDVE->qwValue;
        lpsz = lpstDVE->lpstr[0] ? lpstDVE->lpstr[0] : (lpstDVE->Flags & DVE_TYPE_COMBINATION ? L"Combined Value Editor" : L"Value Editor");
        SendMessageW(hDlg, WM_SETTEXT, 0, (LPARAM)lpsz);
        SetWindowLongPtr(hDlg, DWLP_USER, lParam);

        // Create controls
        GetClientRect(hDlg, &rcClient);

        // Create Edit
        hEdit = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            WC_EDITW,
            0,
            ES_READONLY | WS_CHILD | WS_TABSTOP | WS_VISIBLE,
            PADDING_X,
            PADDING_Y,
            rcClient.right - 2 * PADDING_X,
            CONTROL_H,
            hDlg,
            (HMENU)IDC_EDIT,
            NULL,
            0);
        Dlg_ValueEditor_SetValue(lpstDVE, hDlg, lpstDVE->qwValue);

        // Create Buttons
        iX = rcClient.right - 2 * (PADDING_X + BUTTON_W);
        iY = rcClient.bottom - PADDING_Y - CONTROL_H;
        hBtnReset = CreateWindowExW(
            0,
            WC_BUTTONW,
            0,
            WS_CHILD | WS_TABSTOP | WS_VISIBLE,
            iX,
            iY,
            BUTTON_W,
            CONTROL_H,
            hDlg,
            (HMENU)IDC_RESETBTN,
            NULL,
            0);
        lpsz = lpstDVE->lpstr[1] ? lpstDVE->lpstr[1] : L"Reset";
        SendMessageW(hBtnReset, WM_SETTEXT, 0, (LPARAM)lpsz);

        iX += PADDING_X + BUTTON_W;
        hBtnOK = CreateWindowExW(
            0,
            WC_BUTTONW,
            0,
            BS_DEFPUSHBUTTON | WS_CHILD | WS_TABSTOP | WS_VISIBLE,
            iX,
            iY,
            BUTTON_W,
            CONTROL_H,
            hDlg,
            (HMENU)IDC_OKBTN,
            NULL,
            0);
        lpsz = lpstDVE->lpstr[2] ? lpstDVE->lpstr[2] : L"OK";
        SendMessageW(hBtnOK, WM_SETTEXT, 0, (LPARAM)lpsz);

        // Create View
        hListView = CreateWindowExW(
            0,
            WC_LISTVIEW,
            0,
            LVS_REPORT | LVS_ALIGNLEFT | LVS_SINGLESEL | WS_BORDER | WS_CHILD | WS_TABSTOP | WS_VISIBLE,
            PADDING_X,
            2 * PADDING_Y + CONTROL_H,
            rcClient.right - 2 * PADDING_X,
            rcClient.bottom - 2 * (2 * PADDING_Y + CONTROL_H),
            hDlg,
            (HMENU)IDC_LISTVIEW,
            NULL,
            0);
        CTL_LISTCTL_COLUME aListCol[] = {
            { (UINT_PTR)(lpstDVE->lpstr[3] ? lpstDVE->lpstr[3] : L"Member"), 200 },
            { (UINT_PTR)(lpstDVE->lpstr[4] ? lpstDVE->lpstr[4] : L"Value"), 210 },
            { (UINT_PTR)(lpstDVE->lpstr[5] ? lpstDVE->lpstr[5] : L"Info"), 540 }
        };
        Ctl_InitListCtl(hListView, aListCol, LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
        Dlg_ValueEditor_AddItems(hListView, lpstDVE);
        SetFocus(hListView);

        // DPI Aware
        if (DPI_IsAware())
            DPI_SetAutoAdjustSubclass(hDlg, NULL);

        return FALSE;
    } else if (uMsg == WM_NOTIFY) {
        LPNMHDR lpnm = (LPNMHDR)lParam;
        if (lpnm->idFrom == IDC_LISTVIEW) {
            LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)lParam;
            if (lpnmlv->hdr.idFrom == IDC_LISTVIEW &&
                lpnmlv->hdr.code == LVN_ITEMCHANGING &&
                lpnmlv->iItem != -1 &&
                lpnmlv->uChanged == LVIF_STATE) {
                BOOL    bAllow;
                LVITEMW stLVI;
                stLVI.mask = LVIF_PARAM;
                stLVI.iItem = lpnmlv->iItem;
                stLVI.iSubItem = 0;
                SendMessageW(lpnmlv->hdr.hwndFrom, LVM_GETITEMW, 0, (LPARAM)&stLVI);
                bAllow = SendMessageW(lpnmlv->hdr.hwndFrom, LVM_GETITEMW, 0, (LPARAM)&stLVI) ?
                    (stLVI.lParam ? TRUE : FALSE) :
                    FALSE;
                if (!bAllow) {
                    SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
                    return TRUE;
                }
            } else if (lpnmlv->hdr.idFrom == IDC_LISTVIEW &&
                lpnmlv->hdr.code == LVN_ITEMCHANGED &&
                lpnmlv->iItem != -1 &&
                lpnmlv->uChanged == LVIF_STATE) {
                LVITEMW stLVI;
                stLVI.mask = LVIF_PARAM;
                stLVI.iItem = lpnmlv->iItem;
                stLVI.iSubItem = 0;
                if (SendMessageW(lpnmlv->hdr.hwndFrom, LVM_GETITEMW, 0, (LPARAM)&stLVI) && stLVI.lParam) {
                    PDLG_VALUEEDITOR_CONST  lpstConst = (PDLG_VALUEEDITOR_CONST)stLVI.lParam;
                    PDLG_VALUEEDITOR        lpstDVE = (PDLG_VALUEEDITOR)GetWindowLongPtr(hDlg, DWLP_USER);
                    BOOL                    bNewChecked = (lpnmlv->uNewState & LVIS_STATEIMAGEMASK) >> 12 == 2;
                    BOOL                    bOldChecked = (lpnmlv->uOldState & LVIS_STATEIMAGEMASK) >> 12 == 2;
                    if (bNewChecked != bOldChecked) {
                        lpstDVE->Reserved = COMBINE_FLAGS(lpstDVE->Reserved, lpstConst->Value, bNewChecked);
                        Dlg_ValueEditor_SetValue(lpstDVE, hDlg, lpstDVE->Reserved);
                    }
                }
            }
        }
    } else if (uMsg == WM_COMMAND) {
        if (wParam == MAKEWPARAM(IDC_RESETBTN, 0)) {
            HWND                hListView = GetDlgItem(hDlg, IDC_LISTVIEW);
            PDLG_VALUEEDITOR    lpstDVE = (PDLG_VALUEEDITOR)GetWindowLongPtr(hDlg, DWLP_USER);
            SendMessage(hListView, LVM_DELETEALLITEMS, 0, 0);
            Dlg_ValueEditor_AddItems(hListView, lpstDVE);
            lpstDVE->Reserved = lpstDVE->qwValue;
        } else if (wParam == MAKEWPARAM(IDC_OKBTN, 0)) {
            PDLG_VALUEEDITOR    lpstDVE = (PDLG_VALUEEDITOR)GetWindowLongPtr(hDlg, DWLP_USER);
            lpstDVE->qwValue = lpstDVE->Reserved;
            EndDialog(hDlg, TRUE);
        }
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    } else if (uMsg == WM_CLOSE) {
        EndDialog(hDlg, FALSE);
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    }
    return 0;
}

BOOL NTAPI Dlg_ValueEditorEx(HWND Owner, DWORD Flags, PCWSTR *Strings, PQWORD ValuePointer, PDLG_VALUEEDITOR_CONST Consts, UINT ConstCount) {
    DLG_VALUEEDITOR stDVE = { Owner, Flags, Strings, Consts, ConstCount, *ValuePointer, *ValuePointer };
    BOOL            bSucc;
    DLG_TEMPLATE    stDlgTemplate;
    bSucc = DialogBoxIndirectParamW(
        NULL,
        Dlg_InitTemplate(
            &stDlgTemplate,
            DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION,
            WS_EX_CONTROLPARENT | WS_EX_WINDOWEDGE,
            0,
            0,
            250,
            150),
        Owner,
        Dlg_ValueEditor_DlgProc,
        (LPARAM)&stDVE
    ) == TRUE;
    if (bSucc)
        *ValuePointer = stDVE.qwValue;
    return bSucc;
}