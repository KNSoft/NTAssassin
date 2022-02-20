#pragma once

#include "NTAssassin.h"

typedef PVOID PGDIP_IMAGE;

typedef enum _GDIP_ENCODER_PARAM {
    TIFF_ParamValue_CompressionUnused,
    TIFF_ParamValue_CompressionLZW,
    TIFF_ParamValue_CompressionCCITT3,
    TIFF_ParamValue_CompressionCCITT4,
    TIFF_ParamValue_CompressionRle,
    TIFF_ParamValue_CompressionNone,
    TIFF_ParamValue_CompressionMax
} GDIP_ENCODER_PARAM, *PGDIP_ENCODER_PARAM;

NTA_API PCWSTR NTAPI GDIP_GetStatusText(INT Status);
NTA_API BOOL NTAPI GDIP_Startup(PULONG_PTR Token);
NTA_API PGDIP_IMAGE NTAPI GDIP_LoadImageFromFile(PCWSTR FileName);
NTA_API BOOL NTAPI GDIP_SaveImageToBMPFile(PGDIP_IMAGE Image, PCWSTR FileName);
NTA_API BOOL NTAPI GDIP_SaveImageToGIFFile(PGDIP_IMAGE Image, PCWSTR FileName);
NTA_API BOOL NTAPI GDIP_SaveImageToPNGFile(PGDIP_IMAGE Image, PCWSTR FileName);
NTA_API BOOL NTAPI GDIP_SaveImageToJPEGFile(PGDIP_IMAGE Image, PCWSTR FileName, INT Quality);
NTA_API BOOL NTAPI GDIP_SaveImageToTIFFFile(PGDIP_IMAGE Image, PCWSTR FileName, GDIP_ENCODER_PARAM Compression, INT ColorDepth);