#pragma once

#ifndef _WIE_WINDOWS_
#define _WIE_WINDOWS_

/* Append ntstatus.h */

#include "WIE_Winnt_Supp.h"

#define WIN32_NO_STATUS
#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <intrin.h>

/* Append ntdef.h and Kernel-Mode headers */

#include "WIE_Winnt_Supp.h"
#include "WIE_Arch_Supp.h"
#include "WIE_ntdef.h"
#include "WIE_RtlTypes_Supp.h"

#include "WIE_KeTypes_Supp.h"
#include "WIE_ExTypes_Supp.h"
#include "WIE_wdm.h"
#include "WIE_ntddk.h"
#include "WIE_ntifs.h"
#include "WIE_fltKernel.h"

#include "WIE_Internal_Supp.h"
#include "WIE_WinUser.h"

/* Append base API */

#include "WIE_Ntdll_API.h"
#include "WIE_Kernel32_API.h"
#include "WIE_User32_API.h"

#endif
