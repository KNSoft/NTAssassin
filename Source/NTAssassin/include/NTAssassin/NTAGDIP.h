#pragma once

#include "NTAssassin.h"

typedef struct _GDIP_IMAGECODECINFO {
    CLSID Clsid;
    GUID  FormatID;
    const WCHAR* CodecName;
    const WCHAR* DllName;
    const WCHAR* FormatDescription;
    const WCHAR* FilenameExtension;
    const WCHAR* MimeType;
    DWORD Flags;
    DWORD Version;
    DWORD SigCount;
    DWORD SigSize;
    const BYTE* SigPattern;
    const BYTE* SigMask;
} GDIP_IMAGECODECINFO, * PGDIP_IMAGECODECINFO;

typedef BOOL(CALLBACK* GDIP_IMGCODECENUMPROC)(PGDIP_IMAGECODECINFO ImageCodecInfo, LPARAM Param);

NTA_API PCWSTR NTAPI GDIP_GetStatusText(INT Status);
NTA_API BOOL NTAPI GDIP_Startup(PULONG_PTR Token);
NTA_API BOOL NTAPI GDIP_EnumImageEncoders(GDIP_IMGCODECENUMPROC ImgEncEnumProc, LPARAM Param);
NTA_API BOOL NTAPI GDIP_EnumImageDecoders(GDIP_IMGCODECENUMPROC ImgDecEnumProc, LPARAM Param);