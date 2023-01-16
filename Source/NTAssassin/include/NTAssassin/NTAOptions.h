#pragma once

// NTA_DLL: build lib or dll
// 0-lib 1-dll
#if !defined(NTA_DLL)
#define NTA_DLL 1
#endif

#if NTA_DLL
#ifdef NTA_EXPORTS
#define NTA_API DECLSPEC_EXPORT
#define SLIMCRT_USED
#pragma comment(lib, LIB_PATH_WITH_CONFIG("SlimCRT.lib"))
#pragma comment(lib, LIB_PATH_WITH_CONFIG("SlimCRT_Kernel32_Lib.lib"))
#else
#define NTA_API DECLSPEC_IMPORT
#endif
#else
#define NTA_API
#endif

// NTA_LINKDEFAULTLIB: link default libs
#if defined(NTA_LINKDEFAULTLIB)
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
