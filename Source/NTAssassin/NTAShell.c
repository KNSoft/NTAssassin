#include "include\NTAssassin\NTAssassin.h"

BOOL NTAPI Shell_Exec(PCWSTR File, PCWSTR Param, SHELL_EXEC_VERB Verb, INT ShowCmd, PHANDLE ProcessHandle) {
    SHELLEXECUTEINFOW   stSEIW = { sizeof(SHELLEXECUTEINFOW) };
    BOOL                bRet;
    if (Verb == ShellExecExplore) {
        WCHAR               szFile[MAX_PATH];
        SFGAOF              dwAttr;
        PIDLIST_ABSOLUTE    pidlDir, pidlFile;
        UINT                uIndexSlash;
        HRESULT             hr;
        DWORD               dwError;
        uIndexSlash = (UINT)Str_CopyW(szFile, File);
        while (uIndexSlash > 0 && szFile[--uIndexSlash] != L'\\');
        if (uIndexSlash > 0) {
            szFile[uIndexSlash] = L'\0';
            hr = SHParseDisplayName(szFile, NULL, &pidlDir, 0, &dwAttr);
            if (hr == S_OK) {
                szFile[uIndexSlash] = L'\\';
                hr = SHParseDisplayName(szFile, NULL, &pidlFile, 0, &dwAttr);
                if (hr == S_OK) {
                    hr = SHOpenFolderAndSelectItems(pidlDir, 1, &pidlFile, 0);
                    CoTaskMemFree(pidlFile);
                    if (hr == S_OK) {
                        CoTaskMemFree(pidlDir);
                        return TRUE;
                    }
                }
                CoTaskMemFree(pidlDir);
            }
            dwError = WIN32_FROM_HRESULT(hr);
        } else {
            dwError = ERROR_BAD_PATHNAME;
        }
        NT_SetLastError(dwError);
        return FALSE;
    }
    stSEIW.fMask = (ProcessHandle ? SEE_MASK_NOCLOSEPROCESS : SEE_MASK_DEFAULT) | SEE_MASK_INVOKEIDLIST;
    stSEIW.lpFile = File;
    stSEIW.lpParameters = Param;
    if (Verb == ShellExecOpen) {
        stSEIW.lpVerb = L"open";
    } else if (Verb == ShellExecExplore) {
        stSEIW.lpVerb = L"explore";
    } else if (Verb == ShellExecRunAs) {
        stSEIW.lpVerb = L"runas";
    } else if (Verb == ShellExecProperties) {
        stSEIW.lpVerb = L"properties";
    }  else if (Verb == ShellExecEdit) {
        stSEIW.lpVerb = L"edit";
    } else if (Verb == ShellExecPrint) {
        stSEIW.lpVerb = L"print";
    } else if (Verb == ShellExecFind) {
        stSEIW.lpVerb = L"find";
    } else {
        return FALSE;
    }
    stSEIW.nShow = ShowCmd;
    bRet = ShellExecuteExW(&stSEIW);
    if (ProcessHandle && bRet) {
        *ProcessHandle = stSEIW.hProcess;
    }
    return bRet;
}