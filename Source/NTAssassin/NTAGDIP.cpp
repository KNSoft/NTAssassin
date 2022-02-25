#include "include\NTAssassin\NTAssassin.h"
#include <gdiplus.h>

#pragma warning(disable: 26812)

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

typedef BOOL(CALLBACK* GDIP_IMGCODECENUMPROC)(Gdiplus::ImageCodecInfo ImageCodecInfo, LPARAM Param);

BOOL NTAPI GDIP_Startup(PULONG_PTR Token) {
    GdiplusStartupInput gdiplusStartupInput;
    return GdiplusStartup(Token, &gdiplusStartupInput, NULL) == Ok;
}

BOOL NTAPI GDIP_EnumImageEncoders(GDIP_IMGCODECENUMPROC ImgEncEnumProc, LPARAM Param) {
    BOOL bRet = FALSE;
    UINT uNum, uSize;
    if (GdipGetImageEncodersSize(&uNum, &uSize) == Ok && uNum && uSize) {
        Gdiplus::ImageCodecInfo* pImgEncInfo = (Gdiplus::ImageCodecInfo*)(Mem_Alloc(uSize));
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
        Gdiplus::ImageCodecInfo* pImgEncInfo = (Gdiplus::ImageCodecInfo*)(Mem_Alloc(uSize));
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
    return GdipLoadImageFromFile(FileName, &pImage) == Ok ? (PGDIP_IMAGE)pImage : NULL;
}

PGDIP_IMAGE NTAPI GDIP_LoadImageFromBitmap(HBITMAP Bitmap) {
    GpBitmap *pBitmap;
    return GdipCreateBitmapFromHBITMAP(Bitmap, NULL, &pBitmap) == Ok ? (GpImage*)pBitmap : NULL;
}

BOOL NTAPI GDIP_DisposeImage(PGDIP_IMAGE Image) {
    return GdipDisposeImage((Gdiplus::GpImage*)Image) == Ok;
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

BOOL NTAPI GDIP_SaveImageToFileEx(PGDIP_IMAGE Image, PCWSTR FileName, REFCLSID FormatID, GDIPCONST Gdiplus::EncoderParameters* EncParams) {
    BOOL bRet = FALSE;
    CLSID Encoder;
    GDIP_IMGCODECENUMPARAM Param = { FormatID, &Encoder, FALSE };
    if (GDIP_EnumImageEncoders(GDIP_FindCodecEnumProc, reinterpret_cast<LPARAM>(&Param)) && Param.Found) {
        bRet = GdipSaveImageToFile((Gdiplus::GpImage*)Image, FileName, const_cast<GDIPCONST LPCLSID>(&Encoder), EncParams) == Ok;
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

BOOL NTAPI GDIP_SaveImageToTIFFFile(PGDIP_IMAGE Image, PCWSTR FileName, GDIP_TIFFENCODER_PARAMVALUE Compression, INT ColorDepth) {
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