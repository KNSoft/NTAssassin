#pragma once

/* Depend on Winexports */
#define OEMRESOURCE
#define STRICT_TYPED_ITEMIDS

#include <Wintexports/Wintexports.h>
#include <Wintexports/WIE_CommCtrl.h>

#pragma comment(lib, "WIE_WinAPI.lib")

#if defined(_VC_NODEFAULTLIB)
#define _NO_CRT_STDIO_INLINE
#pragma comment(lib, "WIE_CRT.lib")
#endif

#include "NTAssassin_Type.h"

/* NTA_DLL: Build lib (0) or dll (1) */ 
#if !defined(NTA_DLL)
#define NTA_DLL 1
#endif

#if NTA_DLL
#ifdef _NTASSASSIN_
#define NTA_API DECLSPEC_IMPORT
#else
#define NTA_API DECLSPEC_EXPORT
#endif
#else
#define NTA_API
#endif

#pragma comment (lib, "ntdll.lib")
