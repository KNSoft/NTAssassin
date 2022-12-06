#pragma once

// NTA_DLL
// 0-lib 1-dll
#if !defined(NTA_DLL)
#define NTA_DLL 0
#endif

#if NTA_DLL
#ifdef NTA_EXPORTS
#define NTA_API DECLSPEC_EXPORT
#if defined(_M_IX86)
#if defined(_DEBUG)
#pragma comment (lib, "x86\\Debug\\SlimCRT.lib")
#else
#pragma comment (lib, "x86\\Release\\SlimCRT.lib")
#endif
#elif defined(_M_X64)
#if defined(_DEBUG)
#pragma comment (lib, "x64\\Debug\\SlimCRT.lib")
#else
#pragma comment (lib, "x64\\Release\\SlimCRT.lib")
#endif
#endif
#else
#define NTA_API DECLSPEC_IMPORT
#endif
#else
#define NTA_API
#endif

// NTA_CUSTOMENTRY / NTA_NOENTRY
#ifdef NTA_CUSTOMENTRY
#pragma comment(linker, "/ENTRY:" NTA_CUSTOMENTRY)
#endif

#if !defined(NTA_EXPORTS) && (defined(NTA_CUSTOMENTRY) || defined(NTA_NOENTRY))
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
