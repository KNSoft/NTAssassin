#include "include\NTAssassin\NTANT_UCRT.h"

#include "include\NTAssassin\NTANT.h"

#pragma warning(disable: __WARNING_USING_UNINIT_VAR)

typedef _Check_return_ size_t(__cdecl* PFNwcslen)(
    _In_z_ wchar_t const* _String
    );

typedef _Check_return_ size_t(__cdecl* PFNstrlen)(
    _In_z_ char const* _Str
    );

typedef _Check_return_ _Ret_maybenull_
_When_(return != NULL, _Ret_range_(_Str, _Str + _String_length_(_Str) - 1))
wchar_t _CONST_RETURN*(__cdecl * PFNwcsstr)(
    _In_z_ wchar_t const* _Str,
    _In_z_ wchar_t const* _SubStr
    );

typedef _Check_return_ _Ret_maybenull_
_When_(return != NULL, _Ret_range_(_Str, _Str + _String_length_(_Str) - 1))
char _CONST_RETURN*(__cdecl * PFNstrstr)(
    _In_z_ char const* _Str,
    _In_z_ char const* _SubStr
    );

typedef _Success_(return >= 0) _Check_return_ int(__CRTDECL * PFNvswprintf_s)(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t* const _Buffer,
    _In_										  size_t         const _BufferCount,
    _In_z_ _Printf_format_string_				  wchar_t const* const _Format,
    va_list _ArgList
    );

typedef _Success_(return >= 0) _Check_return_ int (__CRTDECL * PFNvsprintf_s)(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
    _In_                                          size_t      const _BufferCount,
    _In_z_ _Printf_format_string_                 char const* const _Format,
    va_list           _ArgList
    );

typedef _Check_return_ int(__cdecl* PFNwcscmp)(
    _In_z_ wchar_t const* _String1,
    _In_z_ wchar_t const* _String2
    );

typedef _Check_return_ int(__cdecl* PFNstrcmp)(
    _In_z_ char const* _Str1,
    _In_z_ char const* _Str2
    );

typedef _Post_equal_to_(_Dst)
_At_buffer_(
    (unsigned char*)_Dst,
    _Iter_,
    _Size,
    _Post_satisfies_(((unsigned char*)_Dst)[_Iter_] == _Val)
) void*(__cdecl* PFNmemset)(
    _Out_writes_bytes_all_(_Size) void*  _Dst,
    _In_                          int    _Val,
    _In_                          size_t _Size
    );

typedef _Success_(return == 0)
_Check_return_opt_
errno_t(__CRTDECL * PFNmemcpy_s)(
    _Out_writes_bytes_to_opt_(_DestinationSize, _SourceSize) void*       const _Destination,
    _In_                                                     rsize_t     const _DestinationSize,
    _In_reads_bytes_opt_(_SourceSize)                        void const* const _Source,
    _In_                                                     rsize_t     const _SourceSize
    );

static HMODULE hNtDLL = NULL;
static PIMAGE_DATA_DIRECTORY pExportDir = NULL;
static PIMAGE_EXPORT_DIRECTORY pExportTable = NULL;
static PDWORD padwNamesRVA = NULL;

static PVOID NT_GetProcAddr(_In_z_ PCSTR ProcName)
{
    PVOID pFunc = NULL;
    if (!hNtDLL) {
        hNtDLL = NT_GetNtdllHandle();
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
        if (pszFuncName[u] == ProcName[u]) {
            DWORD dwProcRVA = MOVE_PTR(hNtDLL, pExportTable->AddressOfFunctions, DWORD)[MOVE_PTR(hNtDLL, pExportTable->AddressOfNameOrdinals, WORD)[uIndex]];
            if (dwProcRVA < pExportDir->VirtualAddress || dwProcRVA >= pExportDir->VirtualAddress + pExportDir->Size) {
                pFunc = MOVE_PTR(hNtDLL, dwProcRVA, VOID);
                break;
            }
        }
    }
    return pFunc;
}

static PFNwcslen pfnwcslen = NULL;
_Check_return_ size_t __cdecl NT_wcslen(_In_z_ wchar_t const* _String)
{
    if (!pfnwcslen)
        pfnwcslen = (PFNwcslen)NT_GetProcAddr("wcslen");
    return pfnwcslen(_String);
}

static PFNstrlen pfnstrlen = NULL;
_Check_return_ size_t __cdecl NT_strlen(_In_z_ char const* _Str)
{
    if (!pfnstrlen)
        pfnstrlen = (PFNstrlen)NT_GetProcAddr("strlen");
    return pfnstrlen(_Str);
}

static PFNwcsstr pfnwcsstr = NULL;
_Check_return_ _Ret_maybenull_
_When_(return != NULL, _Ret_range_(_Str, _Str + _String_length_(_Str) - 1))
wchar_t _CONST_RETURN* __cdecl NT_wcsstr(
    _In_z_ wchar_t const* _Str,
    _In_z_ wchar_t const* _SubStr)
{
    if (!pfnwcsstr)
        pfnwcsstr = (PFNwcsstr)NT_GetProcAddr("wcsstr");
    return pfnwcsstr(_Str, _SubStr);
}

static PFNstrstr pfnstrstr = NULL;
_Check_return_ _Ret_maybenull_
_When_(return != NULL, _Ret_range_(_Str, _Str + _String_length_(_Str) - 1))
char _CONST_RETURN* __cdecl NT_strstr(
    _In_z_ char const* _Str,
    _In_z_ char const* _SubStr)
{
    if (!pfnstrstr)
        pfnstrstr = (PFNstrstr)NT_GetProcAddr("strstr");
    return pfnstrstr(_Str, _SubStr);
}

static PFNwcscmp pfnwcscmp = NULL;
_Check_return_ int __cdecl NT_wcscmp(_In_z_ wchar_t const* _String1, _In_z_ wchar_t const* _String2)
{
    if (!pfnwcscmp)
        pfnwcscmp = (PFNwcscmp)NT_GetProcAddr("wcscmp");
    return pfnwcscmp(_String1, _String2);
}

static PFNstrcmp pfnstrcmp = NULL;
_Check_return_ int __cdecl NT_strcmp(_In_z_ char const* _Str1, _In_z_ char const* _Str2)
{
    if (!pfnstrcmp)
        pfnstrcmp = (PFNstrcmp)NT_GetProcAddr("strcmp");
    return pfnstrcmp(_Str1, _Str2);
}

static PFNvswprintf_s pfnvswprintf_s = NULL;
_Success_(return >= 0) _Check_return_ int __CRTDECL NT_vswprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t* const _Buffer,
    _In_                                          size_t         const _BufferCount,
    _In_z_ _Printf_format_string_                 wchar_t const* const _Format,
    va_list              _ArgList
)
{
    if (!pfnvswprintf_s)
        pfnvswprintf_s = (PFNvswprintf_s)NT_GetProcAddr("vswprintf_s");
    return pfnvswprintf_s(_Buffer, _BufferCount, _Format, _ArgList);
}

static PFNvsprintf_s pfnvsprintf_s = NULL;
_Success_(return >= 0) _Check_return_ int __CRTDECL NT_vsprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
    _In_                                          size_t      const _BufferCount,
    _In_z_ _Printf_format_string_                 char const* const _Format,
    va_list           _ArgList
)
{
    if (!pfnvsprintf_s)
        pfnvsprintf_s = (PFNvsprintf_s)NT_GetProcAddr("vsprintf_s");
    return pfnvsprintf_s(_Buffer, _BufferCount, _Format, _ArgList);
}

static PFNmemset pfnmemset = NULL;
_Post_equal_to_(_Dst)
_At_buffer_(
    (unsigned char*)_Dst,
    _Iter_,
    _Size,
    _Post_satisfies_(((unsigned char*)_Dst)[_Iter_] == _Val)
) void* __cdecl NT_memset(
    _Out_writes_bytes_all_(_Size) void*  _Dst,
    _In_                          int    _Val,
    _In_                          size_t _Size
)
{
    if (!pfnmemset)
        pfnmemset = (PFNmemset)NT_GetProcAddr("memset");
    return pfnmemset(_Dst, _Val, _Size);
}

static PFNmemcpy_s pfnmemcpy_s = NULL;
_Success_(return == 0)
_Check_return_opt_
errno_t __CRTDECL NT_memcpy_s(
    _Out_writes_bytes_to_opt_(_DestinationSize, _SourceSize) void*       const _Destination,
    _In_                                                     rsize_t     const _DestinationSize,
    _In_reads_bytes_opt_(_SourceSize)                        void const* const _Source,
    _In_                                                     rsize_t     const _SourceSize
)
{
    if (!pfnmemcpy_s)
        pfnmemcpy_s = (PFNmemcpy_s)NT_GetProcAddr("memcpy_s");
    return pfnmemcpy_s(_Destination, _DestinationSize, _Source, _SourceSize);
}

#pragma warning(default: __WARNING_USING_UNINIT_VAR)
