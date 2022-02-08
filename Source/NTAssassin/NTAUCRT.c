#include "include\NTAssassin\NTAssassin.h"

typedef _Check_return_ size_t(__cdecl* PFNwcslen)(
    _In_z_ wchar_t const* _String
    );

typedef _Check_return_ size_t(__cdecl* PFNstrlen)(
    _In_z_ char const* _Str
    );

typedef _Success_(return >= 0) int (__CRTDECL * PFNvswprintf_s)(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t* const _Buffer,
    _In_										  size_t         const _BufferCount,
    _In_z_ _Printf_format_string_				  wchar_t const* const _Format,
    va_list _ArgList
    );

typedef _Success_(return >= 0) int(__CRTDECL * PFNvsprintf_s)(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) char* const _Buffer,
    _In_                                          size_t      const _BufferCount,
    _In_z_ _Printf_format_string_                 char const* const _Format,
    va_list _ArgList
    );

typedef _Check_return_ int(__cdecl* PFNwcscmp)(
    _In_z_ wchar_t const* _String1,
    _In_z_ wchar_t const* _String2
    );

typedef _Check_return_ int(__cdecl* PFNstrcmp)(
    _In_z_ char const* _Str1,
    _In_z_ char const* _Str2
    );

HMODULE hNtDLL = NULL;
PIMAGE_DATA_DIRECTORY pExportDir = NULL;
PIMAGE_EXPORT_DIRECTORY pExportTable = NULL;
PDWORD padwNamesRVA = NULL;

PVOID UCRT_GetProcAddr(_In_z_ PCSTR ProcName) {
    PVOID pFunc = NULL;
    if (!hNtDLL) {
        hNtDLL = Proc_GetNtdllHandle();
        pExportDir = &MOVE_PTR(hNtDLL, ((PIMAGE_DOS_HEADER)hNtDLL)->e_lfanew, IMAGE_NT_HEADERS)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        pExportTable = MOVE_PTR(hNtDLL, pExportDir->VirtualAddress, IMAGE_EXPORT_DIRECTORY);
        padwNamesRVA = MOVE_PTR(hNtDLL, pExportTable->AddressOfNames, DWORD);
    }
    UINT uIndex;
    for (uIndex = 0; uIndex < pExportTable->NumberOfNames; uIndex++) {
        PSTR pszFuncName = MOVE_PTR(hNtDLL, padwNamesRVA[uIndex], CHAR);
        UINT u = 0;
        while (pszFuncName[u] == ProcName[u] && pszFuncName[u] != '\0')
            u++;
        if (pszFuncName[u] == '\0') {
            DWORD dwProcRVA = MOVE_PTR(hNtDLL, pExportTable->AddressOfFunctions, DWORD)[MOVE_PTR(hNtDLL, pExportTable->AddressOfNameOrdinals, WORD)[uIndex]];
            if (dwProcRVA < pExportDir->VirtualAddress || dwProcRVA >= pExportDir->VirtualAddress + pExportDir->Size) {
                pFunc = MOVE_PTR(hNtDLL, dwProcRVA, VOID);
                break;
            }
        }
    }
    return pFunc;
}

PFNwcslen pfnwcslen = NULL;
_Check_return_ size_t __cdecl UCRT_wcslen(_In_z_ wchar_t const* _String) {
    if (!pfnwcslen)
        pfnwcslen = (PFNwcslen)UCRT_GetProcAddr("wcslen");
    return pfnwcslen(_String);
}

PFNstrlen pfnstrlen = NULL;
_Check_return_ size_t __cdecl UCRT_strlen(_In_z_ char const* _Str) {
    if (!pfnstrlen)
        pfnstrlen = (PFNstrlen)UCRT_GetProcAddr("strlen");
    return pfnstrlen(_Str);
}

PFNwcscmp pfnwcscmp = NULL;
_Check_return_ int __cdecl UCRT_wcscmp(_In_z_ wchar_t const* _String1, _In_z_ wchar_t const* _String2) {
    if (!pfnwcscmp)
        pfnwcscmp = (PFNwcscmp)UCRT_GetProcAddr("wcscmp");
    return pfnwcscmp(_String1, _String2);
}

PFNstrcmp pfnstrcmp = NULL;
_Check_return_ int __cdecl UCRT_strcmp(_In_z_ char const* _Str1, _In_z_ char const* _Str2) {
    if (!pfnstrcmp)
        pfnstrcmp = (PFNstrcmp)UCRT_GetProcAddr("strcmp");
    return pfnstrcmp(_Str1, _Str2);
}

PFNvswprintf_s pfnvswprintf_s = NULL;
_Success_(return >= 0) int __CRTDECL UCRT_vswprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t* const _Buffer,
    _In_                                          size_t         const _BufferCount,
    _In_z_ _Printf_format_string_                 wchar_t const* const _Format,
    va_list              _ArgList
) {
    if (!pfnvswprintf_s)
        pfnvswprintf_s = (PFNvswprintf_s)UCRT_GetProcAddr("vswprintf_s");
    return pfnvswprintf_s(_Buffer, _BufferCount, _Format, _ArgList);
}

PFNvsprintf_s pfnvsprintf_s = NULL;
_Success_(return >= 0) int __CRTDECL UCRT_vsprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) char* const _Buffer,
    _In_                                          size_t      const _BufferCount,
    _In_z_ _Printf_format_string_                 char const* const _Format,
    va_list           _ArgList
) {
    if (!pfnvsprintf_s)
        pfnvsprintf_s = (PFNvsprintf_s)UCRT_GetProcAddr("vsprintf_s");
    return pfnvsprintf_s(_Buffer, _BufferCount, _Format, _ArgList);
}