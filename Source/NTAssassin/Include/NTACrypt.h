#pragma once

#include "NTAssassin_Base.h"

#include <wincrypt.h>

typedef BOOL(CALLBACK* CRYPT_CERTENUMPROC)(_In_ PCCERT_CONTEXT CertContext, LPARAM Param);

NTA_API BOOL NTAPI Crypt_EnumBlobCertificates(_In_reads_bytes_(BlobSize) PBYTE BlobData, _In_ ULONG BlobSize, DWORD ExpectedContentTypeFlags, _In_ CRYPT_CERTENUMPROC CertEnumProc, LPARAM Param);
