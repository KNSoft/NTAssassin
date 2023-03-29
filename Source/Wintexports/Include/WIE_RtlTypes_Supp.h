#pragma once

#include "WIE_ntdef.h"

typedef struct _RTLP_CURDIR_REF {
    LONG RefCount;
    HANDLE Handle;
} RTLP_CURDIR_REF, *PRTLP_CURDIR_REF;

typedef struct {
    UNICODE_STRING RelativeName;
    HANDLE ContainingDirectory;
    PRTLP_CURDIR_REF CurDirRef;
} RTL_RELATIVE_NAME_U, *PRTL_RELATIVE_NAME_U;

typedef struct _RTL_DRIVE_LETTER_CURDIR {
    USHORT Flags;
    USHORT Length;
    UINT TimeStamp;
    STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

typedef struct _RTL_DRIVE_LETTER_CURDIR64 {
    USHORT Flags;
    USHORT Length;
    UINT TimeStamp;
    STRING64 DosPath;
} RTL_DRIVE_LETTER_CURDIR64, *PRTL_DRIVE_LETTER_CURDIR64;

typedef struct _RTL_DRIVE_LETTER_CURDIR32 {
    USHORT Flags;
    USHORT Length;
    UINT TimeStamp;
    STRING32 DosPath;
} RTL_DRIVE_LETTER_CURDIR32, *PRTL_DRIVE_LETTER_CURDIR32;

typedef struct _RTL_BALANCED_NODE64 {
    union {
        struct _RTL_BALANCED_NODE64* POINTER_64 Children[2];
        struct {
            struct _RTL_BALANCED_NODE64* POINTER_64 Left;
            struct _RTL_BALANCED_NODE64* POINTER_64 Right;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    #define RTL_BALANCED_NODE_RESERVED_PARENT_MASK 3

    union {
        UCHAR Red : 1;
        UCHAR Balance : 2;
        ULONGLONG ParentValue;
    } DUMMYUNIONNAME2;
} RTL_BALANCED_NODE64, *PRTL_BALANCED_NODE64;

typedef struct _RTL_BALANCED_NODE32 {
    union {
        struct _RTL_BALANCED_NODE32* POINTER_32 Children[2];
        struct {
            struct _RTL_BALANCED_NODE32* POINTER_32 Left;
            struct _RTL_BALANCED_NODE32* POINTER_32 Right;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    #define RTL_BALANCED_NODE_RESERVED_PARENT_MASK 3

    union {
        UCHAR Red : 1;
        UCHAR Balance : 2;
        ULONG ParentValue;
    } DUMMYUNIONNAME2;
} RTL_BALANCED_NODE32, *PRTL_BALANCED_NODE32;

typedef USHORT RTL_ATOM, *PRTL_ATOM;
