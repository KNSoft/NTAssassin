#include "include\NTAssassin\NTAssassin.h"
#include <comdef.h>
#include <gdiplus.h>

#pragma warning(disable: 26812)

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

typedef BOOL(CALLBACK* GDIP_IMGCODECENUMPROC)(Gdiplus::ImageCodecInfo ImageCodecInfo, LPARAM Param);

PCWSTR pszStatusText[] = {
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
        Gdiplus::ImageCodecInfo* pImgEncInfo = static_cast<Gdiplus::ImageCodecInfo *>(Mem_Alloc(uSize));
        if (pImgEncInfo) {
            if (GdipGetImageEncoders(uNum, uSize, pImgEncInfo) == Ok) {
                for (UINT i = 0; i < uNum; i++) {
                    if (!ImgEncEnumProc(pImgEncInfo[i], Param))
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
        Gdiplus::ImageCodecInfo* pImgEncInfo = static_cast<Gdiplus::ImageCodecInfo *>(Mem_Alloc(uSize));
        if (pImgEncInfo) {
            if (GdipGetImageDecoders(uNum, uSize, pImgEncInfo) == Ok) {
                for (UINT i = 0; i < uNum; i++) {
                    if (!ImgDecEnumProc(pImgEncInfo[i], Param))
                        break;
                }
                bRet = TRUE;
            }
            Mem_Free(pImgEncInfo);
        }
    }
    return bRet;
}

PGDIP_IMAGE NTAPI GDIP_LoadImageFromFile(PCWSTR FileName) {
    GpImage *pImage;
    return GdipLoadImageFromFile(FileName, &pImage) == Ok ? static_cast<PGDIP_IMAGE>(pImage) : NULL;
}

typedef struct _GDIP_IMGCODECENUMPARAM {
    REFCLSID    FormatCLSID;
    LPCLSID     OutCLSID;
    BOOL        Found;
} GDIP_IMGCODECENUMPARAM, * PGDIP_IMGCODECENUMPARAM;

BOOL GDIP_FindCodecEnumProc(Gdiplus::ImageCodecInfo ImageCodecInfo, LPARAM Param) {
    PGDIP_IMGCODECENUMPARAM pParam = reinterpret_cast<PGDIP_IMGCODECENUMPARAM>(Param);
    BOOL bRet = TRUE;
    if (Sys_EqualGUID(ImageCodecInfo.FormatID, pParam->FormatCLSID)) {
        Sys_CopyGUID(pParam->OutCLSID, ImageCodecInfo.Clsid);
        pParam->Found = TRUE;
        bRet = FALSE;
    }
    return bRet;
}

BOOL NTAPI GDIP_SaveImageToFileEx(PGDIP_IMAGE Image, PCWSTR FileName, REFCLSID FormatID, Gdiplus::EncoderParameters* EncParams) {
    BOOL bRet = FALSE;
    CLSID Encoder;
    GDIP_IMGCODECENUMPARAM Param = { FormatID, &Encoder, FALSE };
    if (GDIP_EnumImageEncoders(GDIP_FindCodecEnumProc, reinterpret_cast<LPARAM>(&Param)) && Param.Found) {
        bRet = GdipSaveImageToFile(static_cast<Gdiplus::GpImage*>(Image), FileName, &Encoder, EncParams) == Ok;
    }
    return bRet;
}

BOOL NTAPI GDIP_SaveImageToBMPFile(PGDIP_IMAGE Image, PCWSTR FileName) {
    return GDIP_SaveImageToFileEx(Image, FileName, Gdiplus::ImageFormatBMP, NULL);
}

BOOL NTAPI GDIP_SaveImageToGIFFile(PGDIP_IMAGE Image, PCWSTR FileName) {
    return GDIP_SaveImageToFileEx(Image, FileName, Gdiplus::ImageFormatGIF, NULL);
}

BOOL NTAPI GDIP_SaveImageToPNGFile(PGDIP_IMAGE Image, PCWSTR FileName) {
    return GDIP_SaveImageToFileEx(Image, FileName, Gdiplus::ImageFormatPNG, NULL);
}

BOOL NTAPI GDIP_SaveImageToJPEGFile(PGDIP_IMAGE Image, PCWSTR FileName, INT Quality) {
    UINT ubQuality = 0;
    if (Quality >= 0 && Quality <= 100) {
        ubQuality++;
    }
    Gdiplus::EncoderParameters *EncParams;
    if (ubQuality) {
        EncParams = (Gdiplus::EncoderParameters*)Mem_Alloc(sizeof(Gdiplus::EncoderParameters));
        if (EncParams) {
            EncParams->Count = 1;
            EncParams->Parameter[0].Guid = Gdiplus::EncoderQuality;
            EncParams->Parameter[0].Type = EncoderParameterValueTypeLong;
            EncParams->Parameter[0].NumberOfValues = 1;
            EncParams->Parameter[0].Value = &Quality;
        } else {
            return FALSE;
        }
    } else {
        EncParams = NULL;
    }
    BOOL bRet = GDIP_SaveImageToFileEx(Image, FileName, Gdiplus::ImageFormatJPEG, EncParams);
    if (EncParams) {
        Mem_Free(EncParams);
    }
    return bRet;
}

BOOL NTAPI GDIP_SaveImageToTIFFFile(PGDIP_IMAGE Image, PCWSTR FileName, GDIP_ENCODER_PARAM Compression, INT ColorDepth) {
    UINT ubCompression = 0, ubColorDepth = 0, uIndex = 0, uCount;
    if (Compression > TIFF_ParamValue_CompressionUnused && Compression < TIFF_ParamValue_CompressionMax) {
        ubCompression++;
    }
    if (ColorDepth == 1 || ColorDepth == 4 || ColorDepth == 8 || ColorDepth == 24 || ColorDepth == 32) {
        ubColorDepth++;
    }
    uCount = ubCompression + ubColorDepth;
    Gdiplus::EncoderParameters *EncParams;
    if (uCount) {
        EncParams = (Gdiplus::EncoderParameters*)Mem_Alloc(sizeof(Gdiplus::EncoderParameters) + (uCount - 1) * sizeof(Gdiplus::EncoderParameter));
        if (EncParams) {
            EncParams->Count = uCount;
            if (ubCompression) {
                ULONG ulCompression;
                if (Compression == TIFF_ParamValue_CompressionLZW) {
                    ulCompression = EncoderValueCompressionLZW;
                } else if (Compression == TIFF_ParamValue_CompressionCCITT3) {
                    ulCompression = EncoderValueCompressionCCITT3;
                } else if (Compression == TIFF_ParamValue_CompressionCCITT4) {
                    ulCompression = EncoderValueCompressionCCITT4;
                } else if (Compression == TIFF_ParamValue_CompressionRle) {
                    ulCompression = EncoderValueCompressionRle;
                } else if (Compression == TIFF_ParamValue_CompressionNone) {
                    ulCompression = EncoderValueCompressionNone;
                }
                EncParams->Parameter[uIndex].Guid = Gdiplus::EncoderCompression;
                EncParams->Parameter[uIndex].Type = EncoderParameterValueTypeLong;
                EncParams->Parameter[uIndex].NumberOfValues = 1;
                EncParams->Parameter[uIndex].Value = &ulCompression;
                uIndex++;
            }
            if (ubColorDepth) {
                EncParams->Parameter[uIndex].Guid = Gdiplus::EncoderColorDepth;
                EncParams->Parameter[uIndex].Type = EncoderParameterValueTypeLong;
                EncParams->Parameter[uIndex].NumberOfValues = 1;
                EncParams->Parameter[uIndex].Value = &ColorDepth;
            }
        } else {
            return FALSE;
        }
    } else {
        EncParams = NULL;
    }
    BOOL bRet = GDIP_SaveImageToFileEx(Image, FileName, Gdiplus::ImageFormatTIFF, EncParams);
    if (EncParams) {
        Mem_Free(EncParams);
    }
    return bRet;
}

#pragma warning(default: 26812)