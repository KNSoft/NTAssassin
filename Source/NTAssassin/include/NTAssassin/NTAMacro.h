﻿#pragma once

#define DECLSPEC_EXPORT __declspec(dllexport)
#define EXPORT_STD_FUNC #pragma comment(linker, "/EXPORT:"__FUNCTION__"="__FUNCDNAME__)

// VS Project macros

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

#define LIB_PATH_WITH_CONFIG(LibName) VS_PLATFORMTARGET"\\"VS_CONFIGURATION"\\"LibName

#if _WIN64
#define IS_WIN64 TRUE
#else
#define IS_WIN64 FALSE
#endif

// Uses an alter value instead if the value is NULL
#define IF_NULL(val, alt_val) ((val) ? (val) : (alt_val))
// Uses an alter value instead if the value is not NULL
#define IF_NOTNULL(val, alt_val) ((val) ? (alt_val) : (val))

// Applies different expression depends on 32-bit or 64-bit architecture
#define IF_ARCH32OR64(expr64, expr32) (_WIN64 ? (expr64) : (expr32))

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
// Sets or removes a flag from a combination value
#define COMBINE_FLAGS(val, uflag, bEnable) ((bEnable) ? ((val) | (uflag)) : ((val) & ~(uflag)))

#define BYTE_ALIGN(val, ali) (((val) + (ali) - 1) & (~((ali) - 1)))
#define IS_BYTE_ALIGNED(val, ali) (!((val) & ((ali) - 1)))

// Gets is the value is within the valid range of an atom
#define IS_ATOM(val) (((ULONG_PTR)(val) & 0xFFFF) > 0 && ((ULONG_PTR)(val) & 0xFFFF) < MAXINTATOM)

// Moves pointer
#define MOVE_PTR(address, offset, type) ((type *)((PBYTE)(address) + (LONG_PTR)(offset)))

// Defines structure with variable length field
#define DEFINE_ANYSIZE_STRUCT(var, structure, type, size) struct {\
    structure Base;\
    type Extra[(size) - 1];\
} var
