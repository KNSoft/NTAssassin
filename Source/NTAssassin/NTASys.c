#include "include\NTAssassin\NTAssassin.h"

HMODULE hSysDlls[SysDllNameMax] = { NULL };
PWSTR   pszSysDllNames[] = {
    // ntdll.dll always is the first module initialized
    L"kernel32.dll",
    L"user32.dll",
    L"UxTheme.dll",
    L"Dwmapi.dll",
    L"Shcore.dll"
};

HMODULE NTAPI Sys_LoadDll(SYS_DLL_NAME SysDll) {
    if (SysDll >= 0 && SysDll < SysDllNameMax) {
        if (!hSysDlls[SysDll])
            hSysDlls[SysDll] = SysDll == SysDllNameNTDll ? Proc_GetNtdllHandle() : Proc_LoadDll(pszSysDllNames[SysDll - 1], FALSE);
        return hSysDlls[SysDll];
    } else
        return NULL;
}

PVOID NTAPI Sys_LoadAPI(SYS_DLL_NAME SysDll, PSTR APIName) {
    HMODULE hDll = Sys_LoadDll(SysDll);
    return hDll ? Proc_GetProcAddr(hDll, APIName) : NULL;
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
    return Sys_GetMessage(Sys_LoadDll(SysDllNameKernel32), dwError);
}

PCWSTR NTAPI Sys_GetStatusInfo(NTSTATUS Status) {
    return Sys_GetMessage(Proc_GetNtdllHandle(), Status);
}

PCWSTR NTAPI Sys_GetStatusErrorInfo(NTSTATUS Status) {
    DWORD   dwError = RtlNtStatusToDosError(Status);
    if (dwError == ERROR_MR_MID_NOT_FOUND)
        return Sys_GetStatusInfo(Status);
    return Sys_GetMessage(Sys_LoadDll(SysDllNameKernel32), dwError);
}

VOID NTAPI Sys_ErrorMsgBox(HWND Owner, PCWSTR Title, DWORD Error) {
    DWORD   dwError = Error;
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

BOOL Sys_EqualGUID(REFGUID GUID1, REFGUID GUID2) {
    return GUID1->Data1 == GUID2->Data1 &&
        GUID1->Data2 == GUID2->Data2 &&
        GUID1->Data3 == GUID2->Data3 &&
        *(PQWORD)(GUID1->Data4) == *(PQWORD)(GUID2->Data4);
}

VOID Sys_CopyGUID(LPGUID Dest, REFGUID Src) {
    Dest->Data1 = Src->Data1;
    Dest->Data2 = Src->Data2;
    Dest->Data3 = Src->Data3;
    *(PQWORD)(Dest->Data4) = *(PQWORD)(Src->Data4);
}

#define SYS_REG_SERVICES_PATH L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services"

BOOL NTAPI Sys_RegDriver(PCWSTR Name, PCWSTR ImagePath, BOOL Volatile) {
    HANDLE              hKey;
    WCHAR               szRegSvc[MAX_REG_KEYNAME_CCH];
    UNICODE_STRING      strRegSvc, strKeyName;
    OBJECT_ATTRIBUTES   obRegSvc;
    ULONG               ulDisposition;
    NTSTATUS            lStatus;
    BOOL                bRet = FALSE;

    SIZE_T sIndex = Str_CopyW(szRegSvc, SYS_REG_SERVICES_PATH);
    szRegSvc[sIndex++] = '\\';
    sIndex += Str_CopyExW(szRegSvc + sIndex, ARRAYSIZE(szRegSvc) - sIndex, Name);
    if (sIndex > ARRAYSIZE(SYS_REG_SERVICES_PATH) && sIndex < ARRAYSIZE(szRegSvc)) {
        Str_InitW(&strRegSvc, szRegSvc);
        NT_InitObject(&obRegSvc, NULL, &strRegSvc, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE);
        lStatus = NtCreateKey(&hKey, KEY_SET_VALUE, &obRegSvc, 0, NULL, Volatile ? REG_OPTION_VOLATILE : REG_OPTION_NON_VOLATILE, &ulDisposition);
        if (NT_SUCCESS(lStatus)) {
            Str_InitW(&strKeyName, (PWSTR)L"ImagePath");
            lStatus = NtSetValueKey(hKey, &strKeyName, 0, REG_EXPAND_SZ, (PVOID)ImagePath, (ULONG)(Str_SizeW(ImagePath) + sizeof(WCHAR)));
            if (!NT_SUCCESS(lStatus))
                goto Label_1;
            Str_InitW(&strKeyName, L"Type");
            DWORD dwType = SERVICE_KERNEL_DRIVER;
            lStatus = NtSetValueKey(hKey, &strKeyName, 0, REG_DWORD, &dwType, sizeof(dwType));
            if (!NT_SUCCESS(lStatus))
                goto Label_1;
            bRet = TRUE;
            goto Label_0;
        Label_1:
            if (ulDisposition == REG_CREATED_NEW_KEY) {
                NtDeleteKey(hKey);
            }
        Label_0:
            NtClose(hKey);
        }
    }
    return bRet;
}