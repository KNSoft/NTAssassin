#include "NTAssassin\NTAssassin.h"

#define DRE_MAX_NUM_CCH 16
#define DRE_ITEM_W 64

#define IDC_STATIC_POS  1001
#define IDC_TEXT_LEFT   1002
#define IDC_EDIT_LEFT   1003
#define IDC_TEXT_TOP    1004
#define IDC_EDIT_TOP    1005
#define IDC_TEXT_RIGHT  1006
#define IDC_EDIT_RIGHT  1007
#define IDC_TEXT_BOTTOM 1008
#define IDC_EDIT_BOTTOM 1009
#define IDC_TEXT_WIDTH  1010
#define IDC_EDIT_WIDTH  1011
#define IDC_TEXT_HEIGHT 1012
#define IDC_EDIT_HEIGHT 1013

#define IDC_RESETBTN    1014
#define IDC_OKBTN       1015

VOID Dlg_RectEditor_GetValue(HWND hDlg, INT iCtlID, PINT pValue) {
    WCHAR           szNum[DRE_MAX_NUM_CCH];
    INT             i;
    UNICODE_STRING  stStrNum;
    stStrNum.Buffer = szNum;
    i = (INT)UI_GetDlgItemText(hDlg, iCtlID, szNum);
    if (i) {
        szNum[i++] = '\0';
        stStrNum.MaximumLength = (USHORT)(i * sizeof(WCHAR));
        stStrNum.Length = stStrNum.MaximumLength - sizeof(WCHAR);
        if (!NT_SUCCESS(RtlUnicodeStringToInteger(&stStrNum, 0, pValue)))
            *pValue = 0;
    }
}

VOID Dlg_RectEditor_SetValue(HWND hCtl, INT iNum) {
    WCHAR           szNum[DRE_MAX_NUM_CCH];
    UNICODE_STRING  stStrNum;
    stStrNum.MaximumLength = sizeof(szNum);
    stStrNum.Length = sizeof(szNum) - sizeof(WCHAR);
    stStrNum.Buffer = szNum;
    UI_SendMsg(hCtl, WM_SETTEXT, 0, NT_SUCCESS(RtlIntegerToUnicodeString(iNum, 10, &stStrNum)) ? szNum : 0);
}

VOID Dlg_RectEditor_CreateCtl(HWND hDlg, INT nIDText, INT nIDEdit, LPCWSTR lpszText, INT iX, INT iY, INT iNum) {
    HWND    hCtl;

    // Create text control
    hCtl = CreateWindowExW(WS_EX_RIGHT,
        WC_STATICW,
        NULL,
        SS_SIMPLE | SS_RIGHT | WS_CHILD | WS_VISIBLE,
        iX,
        iY,
        DRE_ITEM_W,
        DLG_CONTROL_H,
        hDlg,
        (HMENU)(INT_PTR)nIDText,
        NULL,
        0);
    UI_SetWindowText(hCtl, lpszText);

    // Create edit control
    hCtl = CreateWindowExW(WS_EX_CLIENTEDGE,
        WC_EDITW,
        NULL,
        WS_CHILD | WS_VISIBLE,
        iX + DRE_ITEM_W + DLG_PADDING_X,
        iY,
        DRE_ITEM_W,
        DLG_CONTROL_H,
        hDlg,
        (HMENU)(INT_PTR)nIDEdit,
        NULL,
        0);
    Dlg_RectEditor_SetValue(hCtl, iNum);
    UI_SendMsg(hCtl, EM_SETLIMITTEXT, DRE_MAX_NUM_CCH - 1, 0);
}

INT_PTR CALLBACK Dlg_RectEditor_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITDIALOG) {
        PDLG_RECTEDITOR lpstDRE;
        RECT            rcClient;
        HWND            hBtnReset, hBtnOK;
        PCWSTR          lpsz;
        INT             iX, iY;

        // Initialize
        lpstDRE = (PDLG_RECTEDITOR)lParam;
        lpsz = lpstDRE->lpstr[0] ? lpstDRE->lpstr[0] : L"Rectangle Editor";
        SendMessageW(hDlg, WM_SETTEXT, 0, (LPARAM)lpsz);
        SetWindowLongPtr(hDlg, DWLP_USER, lParam);

        // Create controls
        GetClientRect(hDlg, &rcClient);

        // Create static and edit controls for rectangle
        iX = rcClient.right / 2 - DRE_ITEM_W - DLG_PADDING_X;
        iY = (rcClient.bottom - DLG_CONTROL_H) / 2;

        Dlg_RectEditor_CreateCtl(
            hDlg,
            IDC_TEXT_LEFT,
            IDC_EDIT_LEFT,
            lpsz = lpstDRE->lpstr[3] ? lpstDRE->lpstr[3] : L"Left",
            DLG_PADDING_X,
            iY,
            lpstDRE->lprc->left
        );

        Dlg_RectEditor_CreateCtl(
            hDlg,
            IDC_TEXT_TOP,
            IDC_EDIT_TOP,
            lpstDRE->lpstr[4] ? lpstDRE->lpstr[4] : L"Top",
            iX,
            DLG_PADDING_Y,
            lpstDRE->lprc->top
        );

        Dlg_RectEditor_CreateCtl(
            hDlg,
            IDC_TEXT_RIGHT,
            IDC_EDIT_RIGHT,
            lpsz = lpstDRE->lpstr[5] ? lpstDRE->lpstr[5] : L"Right",
            rcClient.right - 2 * (DLG_PADDING_X + DRE_ITEM_W),
            iY,
            lpstDRE->lprc->right
        );

        Dlg_RectEditor_CreateCtl(
            hDlg,
            IDC_TEXT_BOTTOM,
            IDC_EDIT_BOTTOM,
            lpsz = lpstDRE->lpstr[6] ? lpstDRE->lpstr[6] : L"Bottom",
            iX,
            rcClient.bottom - 2 * (DLG_PADDING_Y + DLG_CONTROL_H),
            lpstDRE->lprc->bottom
        );

        // Create Buttons
        iX = rcClient.right - 2 * (DLG_PADDING_X + DLG_BUTTON_W);
        iY = rcClient.bottom - DLG_PADDING_Y - DLG_CONTROL_H;
        hBtnReset = CreateWindowExW(
            0,
            WC_BUTTONW,
            0,
            WS_CHILD | WS_TABSTOP | WS_VISIBLE,
            iX,
            iY,
            DLG_BUTTON_W,
            DLG_CONTROL_H,
            hDlg,
            (HMENU)IDC_RESETBTN,
            NULL,
            0);
        lpsz = lpstDRE->lpstr[1] ? lpstDRE->lpstr[1] : L"Reset";
        SendMessageW(hBtnReset, WM_SETTEXT, 0, (LPARAM)lpsz);

        iX += DLG_PADDING_X + DLG_BUTTON_W;
        hBtnOK = CreateWindowExW(0,
            WC_BUTTONW,
            0,
            BS_DEFPUSHBUTTON | WS_CHILD | WS_TABSTOP | WS_VISIBLE,
            iX,
            iY,
            DLG_BUTTON_W,
            DLG_CONTROL_H,
            hDlg,
            (HMENU)IDC_OKBTN,
            NULL,
            0);
        lpsz = lpstDRE->lpstr[2] ? lpstDRE->lpstr[2] : L"OK";
        SendMessageW(hBtnOK, WM_SETTEXT, 0, (LPARAM)lpsz);

        // DPI Aware
        if (DPI_IsAware())
            DPI_SetAutoAdjustSubclass(hDlg, NULL);

        return FALSE;
    } else if (uMsg == WM_COMMAND) {
        if (wParam == MAKEWPARAM(IDC_RESETBTN, 0)) {
            PDLG_RECTEDITOR lpstDRE = (PDLG_RECTEDITOR)GetWindowLongPtr(hDlg, DWLP_USER);
            Dlg_RectEditor_SetValue(GetDlgItem(hDlg, IDC_EDIT_LEFT), lpstDRE->lprc->left);
            Dlg_RectEditor_SetValue(GetDlgItem(hDlg, IDC_EDIT_TOP), lpstDRE->lprc->top);
            Dlg_RectEditor_SetValue(GetDlgItem(hDlg, IDC_EDIT_RIGHT), lpstDRE->lprc->right);
            Dlg_RectEditor_SetValue(GetDlgItem(hDlg, IDC_EDIT_BOTTOM), lpstDRE->lprc->bottom);
        } else if (wParam == MAKEWPARAM(IDC_OKBTN, 0)) {
            PDLG_RECTEDITOR lpstDRE = (PDLG_RECTEDITOR)GetWindowLongPtr(hDlg, DWLP_USER);
            RECT            rc;
            Dlg_RectEditor_GetValue(hDlg, IDC_EDIT_LEFT, &rc.left);
            Dlg_RectEditor_GetValue(hDlg, IDC_EDIT_TOP, &rc.top);
            Dlg_RectEditor_GetValue(hDlg, IDC_EDIT_RIGHT, &rc.right);
            Dlg_RectEditor_GetValue(hDlg, IDC_EDIT_BOTTOM, &rc.bottom);
            // Verify inputs
            if (rc.left <= rc.right && rc.top <= rc.bottom) {
                lpstDRE->lprc->left = rc.left;
                lpstDRE->lprc->top = rc.top;
                lpstDRE->lprc->right = rc.right;
                lpstDRE->lprc->bottom = rc.bottom;
                EndDialog(hDlg, TRUE);
            } else
                Sys_ErrorMsgBox(hDlg, lpstDRE->lpstr[0] ? lpstDRE->lpstr[0] : L"Rectangle Editor", ERROR_INVALID_PARAMETER);
        }
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    } else if (uMsg == WM_CLOSE) {
        EndDialog(hDlg, FALSE);
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    }
    return 0;
}

BOOL NTAPI Dlg_RectEditor(HWND Owner, PCWSTR *Strings, PRECT Rect) {
    DLG_RECTEDITOR  stDRD = { Owner, Strings, Rect };
    DLG_TEMPLATE    stDlgTemplate;
    return DialogBoxIndirectParam(
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
        Dlg_RectEditor_DlgProc,
        (LPARAM)&stDRD
    ) == TRUE;
}