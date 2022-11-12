#include "include\NTAssassin\NTADlg.h"

#include "include\NTAssassin\NTACtl.h"
#include "include\NTAssassin\NTAUI.h"
#include "include\NTAssassin\NTAStr.h"
#include "include\NTAssassin\NTADPI.h"

#define PADDING_X   14
#define PADDING_Y   20
#define CONTROL_H   28
#define BUTTON_W    124

#define IDC_EDIT        1001
#define IDC_LISTVIEW    1002
#define IDC_RESETBTN    1003
#define IDC_OKBTN       1004

typedef struct _DLG_VALUEEDITOR {
    HWND                    Owner;
    DWORD                   Flags;
    PCWSTR*                 Texts;          // ["Title", "Reset", "OK", "Member", "Value", "Info", "Unknow"]
    PDLG_VALUEEDITOR_CONST  Consts;
    UINT                    NumOfConsts;
    QWORD                   Value;
    QWORD                   PreviousValue;  // Reserved, do not use
} DLG_VALUEEDITOR, *PDLG_VALUEEDITOR;

static INT Dlg_ValueEditor_FormatValueEx(PDLG_VALUEEDITOR pstDVE, PWSTR pszBuff, INT iCchBuff, QWORD qwValue) {
    INT iTemp;
    if (pstDVE->Flags & DVE_VALUE_HEXQWORD) {
        iTemp = Str_PrintfExW(pszBuff, iCchBuff, L"0x%016llX", qwValue);
    } else if (pstDVE->Flags & DVE_VALUE_HEXDWORD) {
        iTemp = Str_PrintfExW(pszBuff, iCchBuff, L"0x%08X", (DWORD)qwValue);
    } else {
        iTemp = 0;
    }
    return iTemp;
}

#define Dlg_ValueEditor_FormatValue(pstDVE, pszBuff, qwValue) Dlg_ValueEditor_FormatValueEx(pstDVE, pszBuff, ARRAYSIZE(pszBuff), qwValue)

static VOID Dlg_ValueEditor_SetValue(PDLG_VALUEEDITOR pstDVE, HWND hDlg, QWORD qwValue) {
    WCHAR szValue[MAX_QWORD_IN_HEX_CCH + 2];
    UI_SetDlgItemText(
        hDlg,
        IDC_EDIT,
        Dlg_ValueEditor_FormatValue(pstDVE, szValue, qwValue) ? szValue : NULL);
    pstDVE->PreviousValue = qwValue;
}

static VOID Dlg_ValueEditor_AddItems(HWND hListView, PDLG_VALUEEDITOR pstDVE) {
    LVITEMW stLVI;
    WCHAR   szValue[MAX_QWORD_IN_HEX_CCH + 2];
    QWORD   qwTemp = pstDVE->Value;
    UINT    u;
    INT     i;
    stLVI.iItem = MAXINT;
    for (u = 0; u < pstDVE->NumOfConsts; u++) {
        stLVI.iSubItem = 0;
        stLVI.mask = LVIF_TEXT | LVIF_PARAM;
        stLVI.lParam = (LPARAM)&pstDVE->Consts[u];
        stLVI.pszText = (PWSTR)pstDVE->Consts[u].Name;
        stLVI.iItem = (INT)SendMessageW(hListView, LVM_INSERTITEMW, 0, (LPARAM)&stLVI);
        if (stLVI.iItem != -1) {
            stLVI.mask = LVIF_TEXT;
            if ((qwTemp & pstDVE->Consts[u].Value) == pstDVE->Consts[u].Value) {
                ListView_SetCheckState(hListView, stLVI.iItem, TRUE);
                qwTemp &= ~pstDVE->Consts[u].Value;
            }
            stLVI.iSubItem++;
            i = Dlg_ValueEditor_FormatValue(pstDVE, szValue, pstDVE->Consts[u].Value);
            stLVI.pszText = i ? szValue : NULL;
            SendMessageW(hListView, LVM_SETITEMW, 0, (LPARAM)&stLVI);
            stLVI.iSubItem++;
            stLVI.pszText = (LPWSTR)pstDVE->Consts[u].Info;
            SendMessageW(hListView, LVM_SETITEMW, 0, (LPARAM)&stLVI);
        }
        stLVI.iItem++;
    }
    if (qwTemp) {
        stLVI.mask = LVIF_TEXT | LVIF_PARAM;
        stLVI.iSubItem = 0;
        stLVI.lParam = 0;
        stLVI.pszText = (LPWSTR)(pstDVE->Texts[6] ? pstDVE->Texts[6] : L"(Unknow)");
        if (SendMessageW(hListView, LVM_INSERTITEMW, 0, (LPARAM)&stLVI) != -1) {
            ListView_SetCheckState(hListView, stLVI.iItem, TRUE);
            stLVI.mask = LVIF_TEXT;
            stLVI.iSubItem++;
            i = Dlg_ValueEditor_FormatValue(pstDVE, szValue, qwTemp);
            stLVI.pszText = i ? szValue : NULL;
            SendMessageW(hListView, LVM_SETITEMW, 0, (LPARAM)&stLVI);
        }
    }
}

static INT_PTR CALLBACK Dlg_ValueEditor_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITDIALOG) {
        PDLG_VALUEEDITOR    lpstDVE;
        RECT                rcClient;
        HWND                hEdit, hListView, hBtnReset, hBtnOK;
        PCWSTR              lpsz;
        INT                 iX, iY;

        // Initialize
        lpstDVE = (PDLG_VALUEEDITOR)lParam;
        lpstDVE->PreviousValue = lpstDVE->Value;
        lpsz = lpstDVE->Texts[0] ? lpstDVE->Texts[0] : (lpstDVE->Flags & DVE_TYPE_COMBINATION ? L"Combined Value Editor" : L"Value Editor");
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
        Dlg_ValueEditor_SetValue(lpstDVE, hDlg, lpstDVE->Value);

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
        lpsz = lpstDVE->Texts[1] ? lpstDVE->Texts[1] : L"Reset";
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
        lpsz = lpstDVE->Texts[2] ? lpstDVE->Texts[2] : L"OK";
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

        // Fixme: Width of colume not scaled according to DPI
        CTL_LISTCTL_COLUME aListCol[] = {
            { (UINT_PTR)(lpstDVE->Texts[3] ? lpstDVE->Texts[3] : L"Member"), 200 },
            { (UINT_PTR)(lpstDVE->Texts[4] ? lpstDVE->Texts[4] : L"Value"), 210 },
            { (UINT_PTR)(lpstDVE->Texts[5] ? lpstDVE->Texts[5] : L"Info"), 540 }
        };
        Ctl_InitListCtl(hListView, aListCol, LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
        Dlg_ValueEditor_AddItems(hListView, lpstDVE);
        SetFocus(hListView);

        // DPI Aware
        if (IsProcessDPIAware()) {
            DPI_SetAutoAdjustSubclass(hDlg, NULL);
        }

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
                    PDLG_VALUEEDITOR_CONST  pstConst = (PDLG_VALUEEDITOR_CONST)stLVI.lParam;
                    PDLG_VALUEEDITOR        pstDVE = (PDLG_VALUEEDITOR)GetWindowLongPtr(hDlg, DWLP_USER);
                    BOOL                    bNewChecked = (lpnmlv->uNewState & LVIS_STATEIMAGEMASK) >> 12 == 2;
                    BOOL                    bOldChecked = (lpnmlv->uOldState & LVIS_STATEIMAGEMASK) >> 12 == 2;
                    if (bNewChecked != bOldChecked) {
                        pstDVE->PreviousValue = COMBINE_FLAGS(pstDVE->PreviousValue, pstConst->Value, bNewChecked);
                        Dlg_ValueEditor_SetValue(pstDVE, hDlg, pstDVE->PreviousValue);
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
            lpstDVE->PreviousValue = lpstDVE->Value;
        } else if (wParam == MAKEWPARAM(IDC_OKBTN, 0)) {
            PDLG_VALUEEDITOR pstDVE = (PDLG_VALUEEDITOR)GetWindowLongPtr(hDlg, DWLP_USER);
            pstDVE->Value = pstDVE->PreviousValue;
            EndDialog(hDlg, TRUE);
        }
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    } else if (uMsg == WM_CLOSE) {
        EndDialog(hDlg, FALSE);
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    }
    return 0;
}

BOOL NTAPI Dlg_ValueEditorEx(HWND Owner, _In_ DWORD Flags, _In_opt_ PCWSTR* Strings, _Inout_ PQWORD Value, _In_ PDLG_VALUEEDITOR_CONST Consts, _In_ UINT ConstCount) {
    DLG_VALUEEDITOR stDVE = { Owner, Flags, Strings, Consts, ConstCount, *Value, *Value };
    DLG_TEMPLATE    stDlgTemplate;
    BOOL bRet = DialogBoxIndirectParam(
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
    if (bRet) {
        *Value = stDVE.Value;
    }

    return bRet;
}
