#include "Include\NTACrypt.h"

#pragma comment (lib, "Crypt32.lib")

BOOL NTAPI Crypt_EnumBlobCertificates(_In_reads_bytes_(BlobSize) PBYTE BlobData, _In_ ULONG BlobSize, DWORD ExpectedContentTypeFlags, _In_ CRYPT_CERTENUMPROC CertEnumProc, LPARAM Param)
{
    CRYPT_DATA_BLOB Blob = { BlobSize, BlobData };
    HCERTSTORE CertStore;
    PCCERT_CONTEXT CertContext;

    CertStore = CertOpenStore(CERT_STORE_PROV_PKCS7, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, (HCRYPTPROV_LEGACY)NULL, 0, &Blob);
    if (!CertStore)
    {
        return FALSE;
    }

    CertContext = NULL;
    while ((CertContext = CertEnumCertificatesInStore(CertStore, CertContext)) != NULL)
    {
        if (!CertEnumProc(CertContext, Param))
        {
            CertFreeCertificateContext(CertContext);
            break;
        }
    };

    CertCloseStore(CertStore, 0);
    return TRUE;
}
