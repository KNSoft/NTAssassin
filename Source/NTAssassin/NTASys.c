#include "NTAssassin\NTAssassin.h"

HMODULE hSysDlls[SysLoadDllMax] = { NULL };
PWSTR   pszSysDllNames[] = {
    // ntdll.dll always is the first module initialized
    L"kernel32.dll",
    L"user32.dll",
    L"UxTheme.dll",
    L"Dwmapi.dll",
    L"Shcore.dll"
};

HMODULE NTAPI Sys_LoadDll(SYS_LOADDLL_NAME SysDll) {
    if (SysDll >= 0 && SysDll < SysLoadDllMax) {
        if (!hSysDlls[SysDll])
            hSysDlls[SysDll] = SysDll == SysLoadDllNTDll ? Proc_GetNtdllHandle() : Proc_LoadDll(pszSysDllNames[SysDll - 1], FALSE);
        return hSysDlls[SysDll];
    } else
        return NULL;
}

PCWSTR NTAPI Sys_GetMessage(HMODULE ModuleHandle, DWORD MessageId) {
    PMESSAGE_RESOURCE_ENTRY lpstMRE;
    return NT_SUCCESS(
        RtlFindMessage(
            ModuleHandle,
            (ULONG)(ULONG_PTR)RT_MESSAGETABLE,
            0,
            MessageId,
            &lpstMRE)) &&
        lpstMRE->Flags & MESSAGE_RESOURCE_UNICODE ?
        (PCWSTR)lpstMRE->Text :
        NULL;
}

PCWSTR NTAPI Sys_GetErrorInfo(DWORD Error) {
    DWORD   dwError = Error;
    if (HRESULT_SEVERITY(dwError) == SEVERITY_ERROR &&
        HRESULT_FACILITY(dwError) == FACILITY_WIN32)
        dwError = HRESULT_CODE(dwError);
    return Sys_GetMessage(Sys_LoadDll(SysLoadDllKernel32), dwError);
}

PCWSTR NTAPI Sys_GetStatusInfo(NTSTATUS Status) {
    return Sys_GetMessage(Proc_GetNtdllHandle(), Status);
}

PCWSTR NTAPI Sys_GetStatusErrorInfo(NTSTATUS Status) {
    DWORD   dwError = RtlNtStatusToDosError(Status);
    if (dwError == ERROR_MR_MID_NOT_FOUND)
        return Sys_GetStatusInfo(Status);
    return Sys_GetMessage(Sys_LoadDll(SysLoadDllKernel32), dwError);
}

VOID NTAPI Sys_ErrorMsgBox(HWND Owner, PCWSTR Title, DWORD Error) {
    DWORD   dwError = Error;
    if (HRESULT_SEVERITY(dwError) == SEVERITY_ERROR &&
        HRESULT_FACILITY(dwError) == FACILITY_WIN32)
        dwError = HRESULT_CODE(dwError);
    Dlg_MsgBox(
        Owner,
        Sys_GetMessage(Sys_LoadDll(SysLoadDllKernel32), dwError),
        Title,
        dwError == ERROR_SUCCESS ? 0 : MB_ICONERROR
    );
}

VOID NTAPI Sys_StatusMsgBox(HWND Owner, PCWSTR Title, NTSTATUS Status) {
    UINT    uType;
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
        Sys_GetStatusErrorInfo(Status),
        Title,
        uType
    );
}