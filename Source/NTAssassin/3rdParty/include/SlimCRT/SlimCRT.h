#pragma once

#include <stdarg.h>
#include <sal.h>

_Success_(return >= 0)
_Check_return_
int __CRTDECL vswprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
    _In_                                          size_t         const _BufferCount,
    _In_z_ _Printf_format_string_                 wchar_t const* const _Format,
    va_list              _ArgList
);

_Success_(return >= 0)
_Check_return_
int __CRTDECL vsprintf_s(
    _Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
    _In_                                          size_t      const _BufferCount,
    _In_z_ _Printf_format_string_                 char const* const _Format,
    va_list           _ArgList
);

_Success_(return >= 0)
_Check_return_
int __CRTDECL _vsnprintf(
    _Out_writes_opt_(_BufferCount) _Post_maybez_ char*      const _Buffer,
    _In_                                        size_t      const _BufferCount,
    _In_z_ _Printf_format_string_               char const* const _Format,
    va_list           _ArgList
);
