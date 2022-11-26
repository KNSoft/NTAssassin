#include "include\NTAssassin\NTADlg.h"

#include "include\NTAssassin\NTAUI.h"
#include "include\NTAssassin\NTAStr.h"
#include "include\NTAssassin\NTADPI.h"
#include "include\NTAssassin\NTAEH.h"

#define DRE_MAX_NUM_CCH 16
#define DRE_ITEM_W      64

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

typedef struct _DLG_RECTEDITOR {
    HWND    Owner;
    PCWSTR* Texts;  // ["Title", "Reset", "OK", "Left", "Top", "Right", "Bottom", "Width", "Height"]
    PRECT   Rect;
} DLG_RECTEDITOR, *PDLG_RECTEDITOR;

static BOOL Dlg_RectEditor_GetValue(HWND hDlg, INT iCtlID, PINT pValue)
{
    WCHAR szNum[DRE_MAX_NUM_CCH];
    return UI_GetDlgItemText(hDlg, iCtlID, szNum) && Str_ToIntW(szNum, pValue);
}

static VOID Dlg_RectEditor_SetValue(HWND hCtl, INT iNum)
{
    WCHAR szNum[DRE_MAX_NUM_CCH];
    BOOL bSucc = Str_DecFromInt(iNum, szNum);
    UI_SendMsg(hCtl, WM_SETTEXT, 0, bSucc ? szNum : NULL);
}

static VOID Dlg_RectEditor_CreateCtl(HWND hDlg, INT nIDText, INT nIDEdit, LPCWSTR lpszText, INT iX, INT iY, INT iNum)
{
    HWND    hCtl;

    // Create text control
    hCtl = CreateWindowExW(WS_EX_RIGHT,
        WC_STATICW,
        NULL,
        SS_SIMPLE | SS_RIGHT | SS_CENTERIMAGE | WS_CHILD | WS_VISIBLE,
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

static INT_PTR CALLBACK Dlg_RectEditor_DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG) {
        PDLG_RECTEDITOR lpstDRE;
        RECT            rcClient;
        HWND            hBtnReset, hBtnOK;
        PCWSTR          lpsz;
        INT             iX, iY;

        // Initialize
        lpstDRE = (PDLG_RECTEDITOR)lParam;
        lpsz = lpstDRE->Texts[0] ? lpstDRE->Texts[0] : L"Rectangle Editor";
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
            lpsz = lpstDRE->Texts[3] ? lpstDRE->Texts[3] : L"Left",
            DLG_PADDING_X,
            iY,
            lpstDRE->Rect->left
        );

        Dlg_RectEditor_CreateCtl(
            hDlg,
            IDC_TEXT_TOP,
            IDC_EDIT_TOP,
            lpstDRE->Texts[4] ? lpstDRE->Texts[4] : L"Top",
            iX,
            DLG_PADDING_Y,
            lpstDRE->Rect->top
        );

        Dlg_RectEditor_CreateCtl(
            hDlg,
            IDC_TEXT_RIGHT,
            IDC_EDIT_RIGHT,
            lpsz = lpstDRE->Texts[5] ? lpstDRE->Texts[5] : L"Right",
            rcClient.right - 2 * (DLG_PADDING_X + DRE_ITEM_W),
            iY,
            lpstDRE->Rect->right
        );

        Dlg_RectEditor_CreateCtl(
            hDlg,
            IDC_TEXT_BOTTOM,
            IDC_EDIT_BOTTOM,
            lpsz = lpstDRE->Texts[6] ? lpstDRE->Texts[6] : L"Bottom",
            iX,
            rcClient.bottom - 2 * (DLG_PADDING_Y + DLG_CONTROL_H),
            lpstDRE->Rect->bottom
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
        lpsz = lpstDRE->Texts[1] ? lpstDRE->Texts[1] : L"Reset";
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
        lpsz = lpstDRE->Texts[2] ? lpstDRE->Texts[2] : L"OK";
        SendMessageW(hBtnOK, WM_SETTEXT, 0, (LPARAM)lpsz);

        // DPI Aware
        if (IsProcessDPIAware()) {
            DPI_SetAutoAdjustSubclass(hDlg, NULL);
        }

        return FALSE;
    } else if (uMsg == WM_COMMAND) {
        if (wParam == MAKEWPARAM(IDC_RESETBTN, 0)) {
            PDLG_RECTEDITOR lpstDRE = (PDLG_RECTEDITOR)GetWindowLongPtr(hDlg, DWLP_USER);
            Dlg_RectEditor_SetValue(GetDlgItem(hDlg, IDC_EDIT_LEFT), lpstDRE->Rect->left);
            Dlg_RectEditor_SetValue(GetDlgItem(hDlg, IDC_EDIT_TOP), lpstDRE->Rect->top);
            Dlg_RectEditor_SetValue(GetDlgItem(hDlg, IDC_EDIT_RIGHT), lpstDRE->Rect->right);
            Dlg_RectEditor_SetValue(GetDlgItem(hDlg, IDC_EDIT_BOTTOM), lpstDRE->Rect->bottom);
        } else if (wParam == MAKEWPARAM(IDC_OKBTN, 0)) {
            PDLG_RECTEDITOR lpstDRE = (PDLG_RECTEDITOR)GetWindowLongPtr(hDlg, DWLP_USER);
            RECT            rc;
            // Verify inputs
            if (Dlg_RectEditor_GetValue(hDlg, IDC_EDIT_LEFT, &rc.left) &&
                Dlg_RectEditor_GetValue(hDlg, IDC_EDIT_TOP, &rc.top) &&
                Dlg_RectEditor_GetValue(hDlg, IDC_EDIT_RIGHT, &rc.right) &&
                Dlg_RectEditor_GetValue(hDlg, IDC_EDIT_BOTTOM, &rc.bottom) &&
                rc.left <= rc.right && rc.top <= rc.bottom) {
                lpstDRE->Rect->left = rc.left;
                lpstDRE->Rect->top = rc.top;
                lpstDRE->Rect->right = rc.right;
                lpstDRE->Rect->bottom = rc.bottom;
                EndDialog(hDlg, TRUE);
            } else {
                EH_ErrorMsgBox(hDlg, lpstDRE->Texts[0] ? lpstDRE->Texts[0] : L"Rectangle Editor", ERROR_INVALID_PARAMETER);
            }
        }
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    } else if (uMsg == WM_CLOSE) {
        EndDialog(hDlg, FALSE);
        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
    }
    return 0;
}

BOOL NTAPI Dlg_RectEditor(HWND Owner, _In_opt_ PCWSTR* Strings, _Inout_ PRECT Rect)
{
    DLG_RECTEDITOR  stDRD = { Owner, Strings, Rect };
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
        Dlg_RectEditor_DlgProc,
        (LPARAM)&stDRD
    ) == TRUE;
    return bRet;
}
