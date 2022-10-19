#include "include\NTAssassin\NTAEH.h"
#include "include\NTAssassin\NTASys.h"
#include "include\NTAssassin\NTADlg.h"

DWORD NTAPI EH_SetLastNTError(NTSTATUS Status) {
    DWORD dwErrCode;
    dwErrCode = RtlNtStatusToDosErrorNoTeb(Status);
    EH_SetLastError(dwErrCode);
    return dwErrCode;
}

PCWSTR NTAPI EH_GetErrorInfo(DWORD Error) {
    DWORD dwError = Error;
    if (HRESULT_SEVERITY(dwError) == SEVERITY_ERROR &&
        HRESULT_FACILITY(dwError) == FACILITY_WIN32)
        dwError = HRESULT_CODE(dwError);
    return Sys_GetMessage(Sys_LoadDll(SysDllNameKernel32), dwError);
}

PCWSTR NTAPI EH_GetStatusInfo(NTSTATUS Status) {
    return Sys_GetMessage(NT_GetNtdllHandle(), Status);
}

PCWSTR NTAPI EH_GetStatusErrorInfo(NTSTATUS Status) {
    DWORD dwError = RtlNtStatusToDosErrorNoTeb(Status);
    if (dwError == ERROR_MR_MID_NOT_FOUND)
        return EH_GetStatusInfo(Status);
    return Sys_GetMessage(Sys_LoadDll(SysDllNameKernel32), dwError);
}

VOID NTAPI EH_ErrorMsgBox(HWND Owner, PCWSTR Title, DWORD Error) {
    DWORD dwError = Error;
    if (HRESULT_SEVERITY(dwError) == SEVERITY_ERROR &&
        HRESULT_FACILITY(dwError) == FACILITY_WIN32)
        dwError = HRESULT_CODE(dwError);
    Dlg_MsgBox(
        Owner,
        Sys_GetMessage(Sys_LoadDll(SysDllNameKernel32), dwError),
        Title,
        dwError == ERROR_SUCCESS ? 0 : MB_ICONERROR
    );
}

VOID NTAPI EH_StatusMsgBox(HWND Owner, PCWSTR Title, NTSTATUS Status) {
    UINT uType;
    if (NT_INFORMATION(Status))
        uType = MB_ICONINFORMATION;
    else if (NT_WARNING(Status))
        uType = MB_ICONWARNING;
    else if (NT_ERROR(Status))
        uType = MB_ICONERROR;
    else
        uType = 0;
    Dlg_MsgBox(
        Owner,
        EH_GetStatusInfo(Status),
        Title,
        uType
    );
}

DWORD NTAPI EH_HrToWin32(HRESULT hr) {
    DWORD dwError = HRESULT_CODE(hr);
    if (hr != MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, dwError)) {
        if (!IS_ERROR(hr)) {
            dwError = ERROR_SUCCESS;
        } else if ((DWORD)hr & FACILITY_NT_BIT) {
            dwError = RtlNtStatusToDosErrorNoTeb(hr & (~FACILITY_NT_BIT));
        } else {
            if (hr == E_NOINTERFACE) {
                dwError = ERROR_INVALID_FUNCTION;
            } else if (hr == E_NOTIMPL) {
                dwError = ERROR_CALL_NOT_IMPLEMENTED;
            } else if (hr == E_UNEXPECTED) {
                dwError = ERROR_INVALID_DATA;
            } else {
                dwError = hr;
            }
        }
    }
    return dwError;
}
