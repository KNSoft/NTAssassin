﻿#pragma once

#include "NTAssassin.h"

typedef enum _IMAGE_DATA_FORMAT {
    ImageDataFormatUnknow,       // 0xFFRRGGBB
    ImageDataFormatRGB32,       // 0xFFRRGGBB
    ImageDataFormatARGB32,      // 0xAARRGGBB
    ImageDataFormatBGR32,       // 0xFFBBGGRR
    ImageDataFormatABGR32,      // 0xAABBGGRR
    ImageDataFormatRGB24,       // 0xRRGGBB
    ImageDataFormatBGR24,       // 0xBBGGRR
    ImageDataFormatGray8,       // 0xXX
} IMAGE_DATA_FORMAT, * PIMAGE_DATA_FORMAT;

typedef struct _IMAGE_DATA {
    UINT                uRows;
    UINT                uCols;
    IMAGE_DATA_FORMAT   Format;
    PVOID               lpData;
} IMAGE_DATA, * PIMAGE_DATA;

typedef enum _IMAGE_FILE_FORMAT {
    ImageFileFormatUnknow,
    ImageFileFormatBMP,
    ImageFileFormatEMF,
    ImageFileFormatWMF,
    ImageFileFormatJPEG,
    ImageFileFormatPNG,
    ImageFileFormatGIF,
    ImageFileFormatTIFF,
    ImageFileFormatEXIF,
    ImageFileFormatIcon,
    ImageFileFormatHEIF,
    ImageFileFormatWEBP
} IMAGE_FILE_FORMAT, * PIMAGE_FILE_FORMAT;