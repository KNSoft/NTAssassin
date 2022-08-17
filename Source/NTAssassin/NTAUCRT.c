#include "include\NTAssassin\NTAUCRT.h"
#include "include\NTAssassin\NTANT.h"

#pragma warning(disable: 6001)

typedef _Check_return_ size_t(__cdecl* PFNwcslen)(
    _In_z_ wchar_t const* _String
    );

typedef _Check_return_ size_t(__cdecl* PFNstrlen)(
    _In_z_ char const* _Str
    );

typedef _Success_(return >= 0) int(__CRTDECL * PFNvswprintf_s)(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t* const _Buffer,
    _In_										  size_t         const _BufferCount,
    _In_z_ _Printf_format_string_				  wchar_t const* const _Format,
    va_list _ArgList
    );

typedef _Success_(return >= 0) int(__CRTDECL * PFN_vsnprintf)(
    _Out_writes_opt_(_BufferCount) _Post_maybez_ char*       const _Buffer,
    _In_                                        size_t      const _BufferCount,
    _In_z_ _Printf_format_string_               char const* const _Format,
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

static PVOID UCRT_GetProcAddr(_In_z_ PCSTR ProcName) {
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

static PFNwcslen pfnwcslen = NULL;
_Check_return_ size_t __cdecl UCRT_wcslen(_In_z_ wchar_t const* _String) {
    if (!pfnwcslen)
        pfnwcslen = (PFNwcslen)UCRT_GetProcAddr("wcslen");
    return pfnwcslen(_String);
}

static PFNstrlen pfnstrlen = NULL;
_Check_return_ size_t __cdecl UCRT_strlen(_In_z_ char const* _Str) {
    if (!pfnstrlen)
        pfnstrlen = (PFNstrlen)UCRT_GetProcAddr("strlen");
    return pfnstrlen(_Str);
}

static PFNwcscmp pfnwcscmp = NULL;
_Check_return_ int __cdecl UCRT_wcscmp(_In_z_ wchar_t const* _String1, _In_z_ wchar_t const* _String2) {
    if (!pfnwcscmp)
        pfnwcscmp = (PFNwcscmp)UCRT_GetProcAddr("wcscmp");
    return pfnwcscmp(_String1, _String2);
}

static PFNstrcmp pfnstrcmp = NULL;
_Check_return_ int __cdecl UCRT_strcmp(_In_z_ char const* _Str1, _In_z_ char const* _Str2) {
    if (!pfnstrcmp)
        pfnstrcmp = (PFNstrcmp)UCRT_GetProcAddr("strcmp");
    return pfnstrcmp(_Str1, _Str2);
}

static PFNvswprintf_s pfnvswprintf_s = NULL;
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

static PFN_vsnprintf pfn_vsnprintf = NULL;
_Success_(return >= 0) _Check_return_opt_ int __CRTDECL UCRT__vsnprintf(
    _Out_writes_opt_(_BufferCount) _Post_maybez_ char*       const _Buffer,
    _In_                                        size_t      const _BufferCount,
    _In_z_ _Printf_format_string_               char const* const _Format,
    va_list           _ArgList
) {
    if (!pfn_vsnprintf)
        pfn_vsnprintf = (PFN_vsnprintf)UCRT_GetProcAddr("_vsnprintf");
    return pfn_vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
}

static PFNmemset pfnmemset = NULL;
_Post_equal_to_(_Dst)
_At_buffer_(
    (unsigned char*)_Dst,
    _Iter_,
    _Size,
    _Post_satisfies_(((unsigned char*)_Dst)[_Iter_] == _Val)
) void* __cdecl UCRT_memset(
    _Out_writes_bytes_all_(_Size) void*  _Dst,
    _In_                          int    _Val,
    _In_                          size_t _Size
) {
    if (!pfnmemset)
        pfnmemset = (PFNmemset)UCRT_GetProcAddr("memset");
    return pfnmemset(_Dst, _Val, _Size);
}

static PFNmemcpy_s pfnmemcpy_s = NULL;
_Success_(return == 0)
_Check_return_opt_
errno_t __CRTDECL UCRT_memcpy_s(
    _Out_writes_bytes_to_opt_(_DestinationSize, _SourceSize) void*       const _Destination,
    _In_                                                     rsize_t     const _DestinationSize,
    _In_reads_bytes_opt_(_SourceSize)                        void const* const _Source,
    _In_                                                     rsize_t     const _SourceSize
) {
    if (!pfnmemcpy_s)
        pfnmemcpy_s = (PFNmemcpy_s)UCRT_GetProcAddr("memcpy_s");
    return pfnmemcpy_s(_Destination, _DestinationSize, _Source, _SourceSize);
}

#pragma warning(default: 6001)