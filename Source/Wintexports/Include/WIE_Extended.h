#pragma once

#ifndef _WIE_WINDOWS_
#include "WIE_Windows.h"
#endif

#pragma region WinSDK

#define DECLSPEC_EXPORT __declspec(dllexport)
#define EXPORT_STD_FUNC #pragma comment(linker, "/EXPORT:"__FUNCTION__"="__FUNCDNAME__)
typedef unsigned __int64 QWORD;
typedef QWORD near *PQWORD;
typedef QWORD far *LPQWORD;

#if _WIN64
#define IS_WIN64 TRUE
#else
#define IS_WIN64 FALSE
#endif

// Makes a DWORD value by LOWORD and HIWORD
#define MAKEDWORD(l, h) ((DWORD)MAKELONG(l, h))
#define MAKEQWORD(l, h) ((QWORD)(((DWORD)(((DWORD_PTR)(l)) & 0xffffffff)) | ((QWORD)((DWORD)(((DWORD_PTR)(h)) & 0xffffffff))) << 32))

// Clear high 32-bit of HWND
#if _WIN64
#define PURGE_HWND(hWnd) ((HWND)((DWORD_PTR)(hWnd) & 0xFFFFFFFF))
#else
#define PURGE_HWND(hWnd) (hWnd)
#endif

// Reserves low 32-bit only
#define PURGE_PTR32(p) ((PVOID)((ULONG_PTR)(p) & 0xFFFFFFFF))

// Gets equality of two value after masked
#define IS_EQUAL_MASKED(val1, val2, mask) (!(((val1) ^ (val2)) & (mask)))

// Gets is the value is within the valid range of an atom
#define IS_ATOM(val) (((ULONG_PTR)(val) & 0xFFFF) > 0 && ((ULONG_PTR)(val) & 0xFFFF) < MAXINTATOM)

#pragma endregion WinSDK

#pragma region VS Macros

#if defined(_M_IX86)
#define VS_PLATFORMTARGET "x86"
#elif defined(_M_X64)
#define VS_PLATFORMTARGET "x64"
#endif

#if defined(_DEBUG)
#define VS_CONFIGURATION "Debug"
#else
#define VS_CONFIGURATION "Release"
#endif

#pragma endregion VS Macros

#pragma region Pseudo Handles

#define CURRENT_PROCESS_HANDLE                  NtCurrentProcess()
#define CURRENT_THREAD_HANDLE                   NtCurrentThread()
#define CURRENT_SESSION_HANDLE                  NtCurrentSession()
#define CURRENT_PROCESS_TOKEN_HANDLE            GetCurrentProcessToken()
#define CURRENT_THREAD_TOKEN_HANDLE             GetCurrentThreadToken()
#define CURRENT_THREAD_EFFECTIVETOKEN_HANDLE    GetCurrentThreadEffectiveToken()

#pragma endregion Pseudo Handles

#pragma region TEB/PEB

#if defined(_M_X64)
#define WIE_ReadTEB(m) (\
    RTL_FIELD_SIZE(TEB, m) == sizeof(DWORD64) ? __readgsqword(UFIELD_OFFSET(TEB, m)) : (\
        RTL_FIELD_SIZE(TEB, m) == sizeof(DWORD) ? __readgsdword(UFIELD_OFFSET(TEB, m)) : (\
            RTL_FIELD_SIZE(TEB, m) == sizeof(WORD) ? __readgsword(UFIELD_OFFSET(TEB, m)) : \
                __readgsbyte(UFIELD_OFFSET(TEB, m))\
        )\
    )\
)
#define WIE_WriteTEB(m, val) (\
    RTL_FIELD_SIZE(TEB, m) == sizeof(DWORD64) ? __writegsqword(UFIELD_OFFSET(TEB, m), (DWORD64)(val)) : (\
        RTL_FIELD_SIZE(TEB, m) == sizeof(DWORD) ? __writegsdword(UFIELD_OFFSET(TEB, m), (DWORD)(val)) : (\
            RTL_FIELD_SIZE(TEB, m) == sizeof(WORD) ? __writegsword(UFIELD_OFFSET(TEB, m), (WORD)(val)) : \
                __writegsbyte(UFIELD_OFFSET(TEB, m), (BYTE)(val))\
        )\
    )\
)
#elif defined(_M_IX86)
#define WIE_ReadTEB(m) (\
    RTL_FIELD_SIZE(TEB, m) == sizeof(DWORD) ? __readfsdword(UFIELD_OFFSET(TEB, m)) : (\
        RTL_FIELD_SIZE(TEB, m) == sizeof(WORD) ? __readfsword(UFIELD_OFFSET(TEB, m)) : \
            __readfsbyte(UFIELD_OFFSET(TEB, m))\
    )\
)
#define WIE_WriteTEB(m, val) (\
    RTL_FIELD_SIZE(TEB, m) == sizeof(DWORD) ? __writefsdword(UFIELD_OFFSET(TEB, m), (DWORD)(val)) : (\
        RTL_FIELD_SIZE(TEB, m) == sizeof(WORD) ? __writefsword(UFIELD_OFFSET(TEB, m), (WORD)(val)) : \
            __writefsbyte(UFIELD_OFFSET(TEB, m), (BYTE)(val))\
    )\
)
#else
#define WIE_ReadTEB(m) (NtCurrentTeb()->m)
#define WIE_WriteTEB(m, val) (NtCurrentTeb()->m = (val))
#endif

#define NtCurrentPeb() ((PPEB)WIE_ReadTEB(ProcessEnvironmentBlock))

#pragma endregion TEB/PEB

#pragma region Last Error/Status

/// <summary>
/// Gets or sets the last error
/// </summary>
#define WIE_GetLastError() ((DWORD)WIE_ReadTEB(LastErrorValue))
#define WIE_SetLastError(Error) WIE_WriteTEB(LastErrorValue, Error)

/// <summary>
/// Gets or sets the last status
/// </summary>
#define WIE_GetLastStatus() ((NTSTATUS)(WIE_ReadTEB(LastStatusValue)))
#define WIE_SetLastStatus(Status) WIE_WriteTEB(LastStatusValue, Status)

#define NT_FACILITY(Status) (((Status) >> 16) & 0xFFF)

#pragma endregion Last Error/Status

#pragma region Current runtime information

#define CURRENT_PROCESS_ID ((DWORD)(DWORD_PTR)WIE_ReadTEB(ClientId.UniqueProcess))
#define CURRENT_THREAD_ID ((DWORD)(DWORD_PTR)WIE_ReadTEB(ClientId.UniqueThread))
#define CURRENT_DIRECTORY_HANDLE (NtCurrentPeb()->ProcessParameters->CurrentDirectory.Handle)
#define CURRENT_IMAGE_BASE (NtCurrentPeb()->ImageBaseAddress)
#define CURRENT_NTDLL_BASE (CONTAINING_RECORD(NtCurrentPeb()->Ldr->InInitializationOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InInitializationOrderModuleList)->DllBase)
#define CURRENT_PROCESS_HEAP (NtCurrentPeb()->ProcessHeap)

#pragma endregion Current runtime information

#pragma region Limitations

#define MAX_CLASSNAME_CCH       256
#define MAX_CIDENTIFIERNAME_CCH 247
#define MAX_ATOM_CCH            255
#define MAX_REG_KEYNAME_CCH     255
#define MAX_REG_VALUENAME_CCH   16383
#define POINTER_CCH             (sizeof(PVOID) * 2 + 1)
#define HEX_RGB_CCH             8 // #RRGGBB

#pragma endregion Limitations

#pragma region Alignments

#define PAGE_SIZE 0x1000
#define CODE_ALIGNMENT 0x10
#define STRING_ALIGNMENT 0x4

#pragma endregion Alignments

#pragma region String

#define ASCII_CASE_MASK 0b100000
#define UNICODE_EOL ((DWORD)0x000A000D)
#define ANSI_EOL ((WORD)0x0A0D)

#pragma endregion String

#pragma region Any-size array

#define ANYSIZE_STRUCT_SIZE(structure, field, size) UFIELD_OFFSET(structure, field[size])

#define DEFINE_ANYSIZE_STRUCT(varname, structure, type, size) struct {\
    structure Base;\
    type Extra[(size) - 1];\
} varname

#pragma endregion Any-size array

#pragma region Funtion

// WIE_LINKDEFAULTLIB: link default libs
#if defined(WIE_LINKDEFAULTLIB)
#if _DEBUG
#if _DLL
#pragma comment(lib, "msvcrtd.lib")
#pragma comment(lib, "vcruntimed.lib")
#pragma comment(lib, "ucrtd.lib")
#ifdef __cplusplus
#pragma comment(lib, "msvcprtd.lib")
#endif
#else
#pragma comment(lib, "libcmtd.lib")
#pragma comment(lib, "libvcruntimed.lib")
#pragma comment(lib, "libucrtd.lib")
#ifdef __cplusplus
#pragma comment(lib, "libcpmtd.lib")
#endif
#endif
#else
#if _DLL
#pragma comment(lib, "msvcrt.lib")
#pragma comment(lib, "vcruntime.lib")
#pragma comment(lib, "ucrt.lib")
#ifdef __cplusplus
#pragma comment(lib, "msvcprt.lib")
#endif
#else
#pragma comment(lib, "libcmt.lib")
#pragma comment(lib, "libvcruntime.lib")
#pragma comment(lib, "libucrt.lib")
#ifdef __cplusplus
#pragma comment(lib, "libcpmt.lib")
#endif
#endif
#endif
#endif

#pragma endregion Funtion
