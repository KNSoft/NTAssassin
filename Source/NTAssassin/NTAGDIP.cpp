#include "include\NTAssassin\NTAssassin.h"
#include <comdef.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

PCWSTR pszStatusText[] = {
    L"The method call was successful.",
    L"The method call was successful.",
    L"There was an error on the method call, which is identified as something other than those defined by the other elements of this enumeration.",
    L"One of the arguments passed to the method was not valid.",
    L"The operating system is out of memory and could not allocate memory to process the method call.",
    L"One of the arguments specified in the API call is already in use in another thread.",
    L"A buffer specified as an argument in the API call is not large enough to hold the data to be received.",
    L"The method is not implemented.",
    L"The method generated a Win32 error.",
    L"The object is in an invalid state to satisfy the API call.",
    L"The method was aborted.",
    L"The specified image file or metafile cannot be found.",
    L"The method performed an arithmetic operation that produced a numeric overflow.",
    L"A write operation is not allowed on the specified file.",
    L"The specified image file format is not known.",
    L"The specified font family cannot be found. Either the font family name is incorrect or the font family is not installed.",
    L"The specified style is not available for the specified font family.",
    L"The font retrieved from an HDC or LOGFONT is not a TrueType font and cannot be used with GDI+.",
    L"The version of GDI+ that is installed on the system is incompatible with the version with which the application was compiled.",
    L"The GDI+API is not in an initialized state.",
    L"The specified property does not exist in the image.",
    L"The specified property is not supported by the format of the image and, therefore, cannot be set.",
    L"The color profile required to save an image in CMYK format was not found."
};

PCWSTR NTAPI GDIP_GetStatusText(INT Status) {
    return Status >= 0 && Status < ARRAYSIZE(pszStatusText) ? pszStatusText[Status] : NULL;
}

BOOL NTAPI GDIP_Startup(PULONG_PTR Token) {
    GdiplusStartupInput gdiplusStartupInput;
    return GdiplusStartup(Token, &gdiplusStartupInput, NULL) == Ok;
}

BOOL NTAPI GDIP_EnumImageEncoders(GDIP_IMGCODECENUMPROC ImgEncEnumProc, LPARAM Param) {
    BOOL bRet = FALSE;
    UINT uNum, uSize;
    if (GdipGetImageEncodersSize(&uNum, &uSize) == Ok && uNum && uSize) {
        Gdiplus::ImageCodecInfo* pImgEncInfo = (Gdiplus::ImageCodecInfo*)Mem_Alloc(uSize);
        if (pImgEncInfo) {
            if (GdipGetImageEncoders(uNum, uSize, pImgEncInfo) == Ok) {
                for (UINT i = 0; i < uNum; i++) {
                    if (!ImgEncEnumProc((PGDIP_IMAGECODECINFO)&pImgEncInfo[i], Param))
                        break;
                }
                bRet = TRUE;
            }
            Mem_Free(pImgEncInfo);
        }
    }
    return bRet;
}

BOOL NTAPI GDIP_EnumImageDecoders(GDIP_IMGCODECENUMPROC ImgDecEnumProc, LPARAM Param) {
    BOOL bRet = FALSE;
    UINT uNum, uSize;
    if (GdipGetImageDecodersSize(&uNum, &uSize) == Ok && uNum && uSize) {
        Gdiplus::ImageCodecInfo* pImgEncInfo = (Gdiplus::ImageCodecInfo*)Mem_Alloc(uSize);
        if (pImgEncInfo) {
            if (GdipGetImageDecoders(uNum, uSize, pImgEncInfo) == Ok) {
                for (UINT i = 0; i < uNum; i++) {
                    if (!ImgDecEnumProc((PGDIP_IMAGECODECINFO)&pImgEncInfo[i], Param))
                        break;
                }
                bRet = TRUE;
            }
            Mem_Free(pImgEncInfo);
        }
    }
    return bRet;
}

BOOL NTAPI GDIP_LoadImageFromBitmap(HBITMAP Bitmap, PVOID* Image) {
    return FALSE;
}

PVOID NTAPI GDIP_LoadImageFromFile(PCWSTR FileName) {
    GpImage *pImage;
    return GdipLoadImageFromFile(FileName, &pImage) == Ok ? static_cast<PVOID>(pImage) : NULL;
}