#pragma once

/* Depend on Winexports */

#define WIN32_LEAN_AND_MEAN
#define OEMRESOURCE
#define STRICT_TYPED_ITEMIDS

#ifndef _WINTEXPORTS_
#include "Wintexports.h"
#endif

#include <suppress.h>
#include "WIE_CommCtrl.h"

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
#pragma comment(lib, VS_CONFIGURATION"\\WIE_CRT.lib")
#endif
#else
#define NTA_API
#endif

#pragma comment (lib, "ntdll.lib")
