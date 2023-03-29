/*
 * _WIE_NO_HASH_SUPP_       Not includes hash support
 * _WIE_NO_EXTENDED_SUPP_   Not includes extended functions support by Wintexports
 * _WIE_NO_CRT_SUPP_        Not includes CRT support by Wintexports even if _VC_NODEFAULTLIB defined
 */

#pragma once

#ifndef _WINTEXPORTS_
#define _WINTEXPORTS_

#include "WIE_Windows.h"

#if !defined(_WIE_NO_HASH_SUPP_)
#include "WIE_Hash_Supp.h"
#endif

#if !defined(_WIE_NO_EXTENDED_SUPP_)
#include "WIE_Extended.h"
#endif

#endif
