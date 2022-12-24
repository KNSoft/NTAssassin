#include "include\NTAssassin\NTANT_UCRT.h"

#include "include\NTAssassin\NTANT.h"

#pragma warning(disable: __WARNING_USING_UNINIT_VAR)

/* UCRT routines types */

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

/* UCRT routine definitions */

typedef struct _NT_UCRT_ROUTINE {
    PCSTR FuncName;
    PVOID Addr;
} NT_UCRT_ROUTINE, *PNT_UCRT_ROUTINE;

#define DEFINE_NT_UCRT_ROUTINE(x) { #x, NULL }
#define CALL_NT_UCRT_ROUTINE(x) ((PFN##x)(g_UCRTRoutines[UCRT_##x].Addr))

static NT_UCRT_ROUTINE g_UCRTRoutines[] = {
    DEFINE_NT_UCRT_ROUTINE(wcslen),
    DEFINE_NT_UCRT_ROUTINE(strlen),
    DEFINE_NT_UCRT_ROUTINE(wcsstr),
    DEFINE_NT_UCRT_ROUTINE(strstr),
    DEFINE_NT_UCRT_ROUTINE(wcscmp),
    DEFINE_NT_UCRT_ROUTINE(strcmp),
    DEFINE_NT_UCRT_ROUTINE(vswprintf_s),
    DEFINE_NT_UCRT_ROUTINE(vsprintf_s),
    DEFINE_NT_UCRT_ROUTINE(memset),
    DEFINE_NT_UCRT_ROUTINE(memcpy_s)
};

enum {
    UCRT_wcslen = 0,
    UCRT_strlen,
    UCRT_wcsstr,
    UCRT_strstr,
    UCRT_wcscmp,
    UCRT_strcmp,
    UCRT_vswprintf_s,
    UCRT_vsprintf_s,
    UCRT_memset,
    UCRT_memcpy_s
};

/* Initializate once */

static ULONG g_UCRTRoutinesUninitialized = ARRAYSIZE(g_UCRTRoutines);
static RTL_RUN_ONCE g_InitOnce = RTL_RUN_ONCE_INIT;

static BOOL NT_InitUCRT_IsUCRTFirstLetter(CHAR ch)
{
    return ch == 'w' ||
        ch == 's' ||
        ch == 'v' ||
        ch == 'm' ||
        ch == '_';
}

static BOOL NT_InitUCRT_StrEqual(PCSTR String1, PCSTR String2)
{
    ULONG u = 0;
    while (String1[u] == String2[u]) {
        if (String1[u] == ANSI_NULL) {
            return TRUE;
        }
        u++;
    }
    return FALSE;
}

static _Noreturn _Analysis_noreturn_ VOID NT_InitUCRT_Failed()
{
    #if defined(_DEBUG)
    __debugbreak();
    #endif
    __fastfail(FAST_FAIL_FATAL_APP_EXIT);
}

static
_Function_class_(RTL_RUN_ONCE_INIT_FN)
_IRQL_requires_same_
ULONG NTAPI NT_InitUCRTOnce(_Inout_ PRTL_RUN_ONCE RunOnce, _Inout_opt_ PVOID Parameter, _Inout_opt_ PVOID * Context)
{
    HMODULE hNtDLL = NT_GetNtdllHandle();
    PIMAGE_DATA_DIRECTORY pExportDir = &MOVE_PTR(hNtDLL, ((PIMAGE_DOS_HEADER)hNtDLL)->e_lfanew, IMAGE_NT_HEADERS)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    PIMAGE_EXPORT_DIRECTORY pExportTable = MOVE_PTR(hNtDLL, pExportDir->VirtualAddress, IMAGE_EXPORT_DIRECTORY);
    PDWORD padwNamesRVA = MOVE_PTR(hNtDLL, pExportTable->AddressOfNames, DWORD);

    ULONG ulIndex;
    for (ulIndex = 0; ulIndex < pExportTable->NumberOfNames; ulIndex++) {
        PSTR pszFuncName = MOVE_PTR(hNtDLL, padwNamesRVA[ulIndex], CHAR);
        if (NT_InitUCRT_IsUCRTFirstLetter(pszFuncName[0])) {
            ULONG u;
            for (u = 0; u < ARRAYSIZE(g_UCRTRoutines); u++) {
                if (!g_UCRTRoutines[u].Addr && NT_InitUCRT_StrEqual(pszFuncName, g_UCRTRoutines[u].FuncName)) {
                    g_UCRTRoutines[u].Addr = MOVE_PTR(hNtDLL, MOVE_PTR(hNtDLL, pExportTable->AddressOfFunctions, DWORD)[MOVE_PTR(hNtDLL, pExportTable->AddressOfNameOrdinals, WORD)[ulIndex]], VOID);
                    g_UCRTRoutinesUninitialized--;
                    if (!g_UCRTRoutinesUninitialized) {
                        return TRUE;
                    }
                }
            }
        }
    }
    NT_InitUCRT_Failed();
}

static VOID NT_InitUCRT()
{
    if (!g_UCRTRoutinesUninitialized) {
        return;
    }
    if (!NT_SUCCESS(RtlRunOnceExecuteOnce(&g_InitOnce, NT_InitUCRTOnce, NULL, NULL))) {
        NT_InitUCRT_Failed();
    }
}

_Check_return_ size_t __cdecl NT_wcslen(_In_z_ wchar_t const* _String)
{
    NT_InitUCRT();
    return CALL_NT_UCRT_ROUTINE(wcslen)(_String);
}

_Check_return_ size_t __cdecl NT_strlen(_In_z_ char const* _Str)
{
    NT_InitUCRT();
    return CALL_NT_UCRT_ROUTINE(strlen)(_Str);
}

_Check_return_ _Ret_maybenull_
_When_(return != NULL, _Ret_range_(_Str, _Str + _String_length_(_Str) - 1))
wchar_t _CONST_RETURN* __cdecl NT_wcsstr(
    _In_z_ wchar_t const* _Str,
    _In_z_ wchar_t const* _SubStr)
{
    NT_InitUCRT();
    return CALL_NT_UCRT_ROUTINE(wcsstr)(_Str, _SubStr);
}

_Check_return_ _Ret_maybenull_
_When_(return != NULL, _Ret_range_(_Str, _Str + _String_length_(_Str) - 1))
char _CONST_RETURN* __cdecl NT_strstr(
    _In_z_ char const* _Str,
    _In_z_ char const* _SubStr)
{
    NT_InitUCRT();
    return CALL_NT_UCRT_ROUTINE(strstr)(_Str, _SubStr);
}

_Check_return_ int __cdecl NT_wcscmp(_In_z_ wchar_t const* _String1, _In_z_ wchar_t const* _String2)
{
    NT_InitUCRT();
    return CALL_NT_UCRT_ROUTINE(wcscmp)(_String1, _String2);
}

_Check_return_ int __cdecl NT_strcmp(_In_z_ char const* _Str1, _In_z_ char const* _Str2)
{
    NT_InitUCRT();
    return CALL_NT_UCRT_ROUTINE(strcmp)(_Str1, _Str2);
}

_Success_(return >= 0) _Check_return_ int __CRTDECL NT_vswprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t* const _Buffer,
    _In_                                          size_t         const _BufferCount,
    _In_z_ _Printf_format_string_                 wchar_t const* const _Format,
    va_list              _ArgList
)
{
    NT_InitUCRT();
    return CALL_NT_UCRT_ROUTINE(vswprintf_s)(_Buffer, _BufferCount, _Format, _ArgList);
}

_Success_(return >= 0) _Check_return_ int __CRTDECL NT_vsprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
    _In_                                          size_t      const _BufferCount,
    _In_z_ _Printf_format_string_                 char const* const _Format,
    va_list           _ArgList
)
{
    NT_InitUCRT();
    return CALL_NT_UCRT_ROUTINE(vsprintf_s)(_Buffer, _BufferCount, _Format, _ArgList);
}

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
    NT_InitUCRT();
    return CALL_NT_UCRT_ROUTINE(memset)(_Dst, _Val, _Size);
}

_Success_(return == 0)
_Check_return_opt_
errno_t __CRTDECL NT_memcpy_s(
    _Out_writes_bytes_to_opt_(_DestinationSize, _SourceSize) void*       const _Destination,
    _In_                                                     rsize_t     const _DestinationSize,
    _In_reads_bytes_opt_(_SourceSize)                        void const* const _Source,
    _In_                                                     rsize_t     const _SourceSize
)
{
    NT_InitUCRT();
    return CALL_NT_UCRT_ROUTINE(memcpy_s)(_Destination, _DestinationSize, _Source, _SourceSize);
}

#pragma warning(default: __WARNING_USING_UNINIT_VAR)
