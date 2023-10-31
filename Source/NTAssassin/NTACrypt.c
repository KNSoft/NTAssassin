#include "Include\NTACrypt.h"

#pragma comment (lib, "Crypt32.lib")

BOOL NTAPI Crypt_EnumBlobCertificates(_In_reads_(BlobSize) PBYTE BlobData, _In_ ULONG BlobSize, DWORD ExpectedContentTypeFlags, _In_ CRYPT_CERTENUMPROC CertEnumProc, LPARAM Param)
{
    CRYPT_UINT_BLOB blob = { BlobSize, BlobData };
    HCERTSTORE hCertStore;
    if (CryptQueryObject(CERT_QUERY_OBJECT_BLOB, &blob, ExpectedContentTypeFlags, CERT_QUERY_FORMAT_FLAG_BINARY, 0, NULL, NULL, NULL, &hCertStore, NULL, NULL))
    {
        PCCERT_CONTEXT pContext = NULL;
        while ((pContext = CertEnumCertificatesInStore(hCertStore, pContext)) != NULL)
        {
            if (!CertEnumProc(pContext, Param))
            {
                CertFreeCertificateContext(pContext);
                break;
            }
        };
        CertCloseStore(hCertStore, 0);
        return TRUE;
    } else
    {
        return FALSE;
    }
}
