/*
 * Licensed to Systerel under one or more contributor license
 * agreements. See the NOTICE file distributed with this work
 * for additional information regarding copyright ownership.
 * Systerel licenses this file to you under the Apache
 * License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/** \file pki_stack.c
 *
 * The minimal PKI implementation provided by the stack. It is lib-specific.
 *
 * This is not the role of the stack to provide a full-blown configurable PKI.
 * The stack provides only a minimal, always safe validating PKI.
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "sopc_crypto_provider.h"
#include "sopc_key_manager.h"
#include "sopc_logger.h"
#include "sopc_macros.h"
#include "sopc_mem_alloc.h"
#include "sopc_pki.h"
#include "sopc_pki_stack.h"

#include "key_manager_lib.h"

#include "mbedtls/oid.h"
#include "mbedtls/x509.h"

/**
 * The minimal profile supported by the PKIProviderStack. It requires cacert signed with
 *  at least SHA-256, with an RSA key of at least 2048 bits.
 */
static const mbedtls_x509_crt_profile mbedtls_x509_crt_profile_minimal = {
    /* Hashes from SHA-256 and above */
    .allowed_mds = MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA256) | MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA384) |
                   MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA512),
    .allowed_pks = 0xFFFFFFF,     /* Any PK alg */
    .allowed_curves = 0xFFFFFFFF, /* Any curve  */
    .rsa_min_bitlen = 2048,
};

static uint32_t PKIProviderStack_GetCertificateValidationError(uint32_t failure_reasons)
{
    if ((failure_reasons & MBEDTLS_X509_BADCERT_EXPIRED) != 0)
    {
        return SOPC_CertificateValidationError_TimeInvalid;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_REVOKED) != 0)
    {
        return SOPC_CertificateValidationError_Revoked;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0)
    {
        return SOPC_CertificateValidationError_HostNameInvalid;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0)
    {
        return SOPC_CertificateValidationError_Untrusted;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCRL_NOT_TRUSTED) != 0)
    {
        return SOPC_CertificateValidationError_RevocationUnknown;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCRL_EXPIRED) != 0)
    {
        return SOPC_CertificateValidationError_RevocationUnknown;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_MISSING) != 0)
    {
        return SOPC_CertificateValidationError_Invalid;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_SKIP_VERIFY) != 0)
    {
        return SOPC_CertificateValidationError_UseNotAllowed;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_OTHER) != 0)
    {
        return SOPC_CertificateValidationError_Untrusted;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_FUTURE) != 0)
    {
        return SOPC_CertificateValidationError_TimeInvalid;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCRL_FUTURE) != 0)
    {
        return SOPC_CertificateValidationError_RevocationUnknown;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_KEY_USAGE) != 0)
    {
        return SOPC_CertificateValidationError_Invalid;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_EXT_KEY_USAGE) != 0)
    {
        return SOPC_CertificateValidationError_Invalid;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_NS_CERT_TYPE) != 0)
    {
        return SOPC_CertificateValidationError_Invalid;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_BAD_MD) != 0)
    {
        return SOPC_CertificateValidationError_Invalid;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_BAD_PK) != 0)
    {
        return SOPC_CertificateValidationError_Invalid;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCERT_BAD_KEY) != 0)
    {
        return SOPC_CertificateValidationError_Invalid;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCRL_BAD_MD) != 0)
    {
        return SOPC_CertificateValidationError_RevocationUnknown;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCRL_BAD_PK) != 0)
    {
        return SOPC_CertificateValidationError_RevocationUnknown;
    }
    else if ((failure_reasons & MBEDTLS_X509_BADCRL_BAD_KEY) != 0)
    {
        return SOPC_CertificateValidationError_RevocationUnknown;
    }

    return SOPC_CertificateValidationError_Unkown;
}

static int verify_cert(void* trust_li, mbedtls_x509_crt* crt, int certificate_depth, uint32_t* flags)
{
    /* When we have an issued certificate that is trusted, but we don't trust its parents,
     * we have verified the chain certificates, but we should still mark the certificate as trusted.
     * "NOT_TRUSTED" should be the sole problem.
     */
    if (NULL != trust_li && 0 == certificate_depth && MBEDTLS_X509_BADCERT_NOT_TRUSTED == *flags)
    {
        /* Verify if crt is in trust_li */
        /* x509_crt_check_ee_locally_trusted() does this but only for self-signed end-entity certificates (!) */
        for (mbedtls_x509_crt* cur = (mbedtls_x509_crt*) trust_li; NULL != cur; cur = cur->next)
        {
            if (crt->raw.len == cur->raw.len && 0 == memcmp(crt->raw.p, cur->raw.p, crt->raw.len))
            {
                *flags = 0;
                break;
            }
        }
    }

    /* Only fatal errors whould be returned here, as this error code will be forwarded to the caller of
     * mbedtls_x509_crt_verify_with_profile, and the verification stopped.
     * Errors may be MBEDTLS_ERR_X509_FATAL_ERROR, or application specific */
    return 0;
}

/* Returns 0 if all key usages and extended key usages are ok */
static SOPC_ReturnStatus check_key_usages(const mbedtls_x509_crt* crt)
{
    unsigned int usages = MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_NON_REPUDIATION |
                          MBEDTLS_X509_KU_KEY_ENCIPHERMENT | MBEDTLS_X509_KU_DATA_ENCIPHERMENT;
    int err = mbedtls_x509_crt_check_key_usage(crt, usages);

    if (0 == err)
    {
        /* If the ext usage is neither server auth nor client auth, it shall be rejected */
        /* TODO: check whether the crt is for a server or a client, and only check the corresponding ext usage */
        bool missSer = mbedtls_x509_crt_check_extended_key_usage(crt, MBEDTLS_OID_SERVER_AUTH,
                                                                 MBEDTLS_OID_SIZE(MBEDTLS_OID_SERVER_AUTH));
        bool missCli = mbedtls_x509_crt_check_extended_key_usage(crt, MBEDTLS_OID_CLIENT_AUTH,
                                                                 MBEDTLS_OID_SIZE(MBEDTLS_OID_CLIENT_AUTH));
        if (missSer && missCli)
        {
            err = 1;
        }
    }

    if (0 == err)
    {
        return SOPC_STATUS_OK;
    }
    else
    {
        return SOPC_STATUS_NOK;
    }
}

static SOPC_ReturnStatus PKIProviderStack_ValidateCertificate(const SOPC_PKIProvider* pPKI,
                                                              const SOPC_CertificateList* pToValidate,
                                                              uint32_t* error)
{
    if (NULL == pPKI || NULL == pToValidate || NULL == error)
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }
    *error = SOPC_CertificateValidationError_Unkown;

    /* Gathers certificates from pki structure */
    SOPC_CertificateList* cert_ca = (SOPC_CertificateList*) (pPKI->pUserTrustedIssuersList);
    SOPC_CRLList* cert_crl = (SOPC_CRLList*) (pPKI->pUserCertRevocList);
    if (NULL == cert_ca || NULL == cert_crl)
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }
    /* Assumes that mbedtls does not modify the certificates */
    SOPC_GCC_DIAGNOSTIC_IGNORE_CAST_CONST
    mbedtls_x509_crt* mbed_ca = (mbedtls_x509_crt*) (&cert_ca->crt);
    mbedtls_x509_crt* mbed_chall = (mbedtls_x509_crt*) (&pToValidate->crt);
    mbedtls_x509_crl* mbed_crl = (mbedtls_x509_crl*) (&cert_crl->crl);
    SOPC_GCC_DIAGNOSTIC_RESTORE

    /* Check certificate usages and certificate chain */
    SOPC_ReturnStatus status = check_key_usages(mbed_chall);
    if (SOPC_STATUS_OK != status)
    {
        *error = SOPC_CertificateValidationError_UseNotAllowed;
    }

    if (SOPC_STATUS_OK == status)
    {
        uint32_t failure_reasons = 0;
        if (mbedtls_x509_crt_verify_with_profile(mbed_chall, mbed_ca, mbed_crl, &mbedtls_x509_crt_profile_minimal,
                                                 NULL /* You can specify an expected Common Name here */,
                                                 &failure_reasons, verify_cert, mbed_ca) != 0)
        {
            *error = PKIProviderStack_GetCertificateValidationError(failure_reasons);
            status = SOPC_STATUS_NOK;
        }
    }

    return status;
}

static void PKIProviderStack_Free(SOPC_PKIProvider* pPKI)
{
    if (pPKI == NULL)
    {
        return;
    }

    /* Deleting the untrusted list will also clear the trusted list, as they are linked.
     * mbedtls zeroizes the elements upon clear, so we can call Certificate_Free on issuers.
     */
    SOPC_KeyManager_Certificate_Free(pPKI->pUserUntrustedIssuersList);
    SOPC_KeyManager_Certificate_Free(pPKI->pUserTrustedIssuersList);
    SOPC_KeyManager_Certificate_Free(pPKI->pUserIssuedCertsList);
    SOPC_KeyManager_CRL_Free(pPKI->pUserCertRevocList);
    SOPC_Free(pPKI);
}

static SOPC_PKIProvider* create_pkistack(SOPC_CertificateList* issuers,
                                         SOPC_CertificateList* issued,
                                         SOPC_CertificateList* untrusted,
                                         SOPC_CRLList* crl,
                                         void* pUserData)
{
    SOPC_PKIProvider* pki = SOPC_Malloc(sizeof(SOPC_PKIProvider));

    if (NULL != pki)
    {
        /* The pki function pointer shall be const after this init */
        SOPC_GCC_DIAGNOSTIC_IGNORE_CAST_CONST
        *(SOPC_PKIProvider_Free_Func*) (&pki->pFnFree) = &PKIProviderStack_Free;
        *(SOPC_FnValidateCertificate*) (&pki->pFnValidateCertificate) = &PKIProviderStack_ValidateCertificate;
        SOPC_GCC_DIAGNOSTIC_RESTORE

        pki->pUserTrustedIssuersList = issuers;
        pki->pUserIssuedCertsList = issued;
        pki->pUserUntrustedIssuersList = untrusted;
        pki->pUserCertRevocList = crl;
        pki->pUserData = pUserData;
    }

    return pki;
}

SOPC_ReturnStatus SOPC_PKIProviderStack_Create(SOPC_SerializedCertificate* pCertAuth,
                                               SOPC_CRLList* pRevocationList,
                                               SOPC_PKIProvider** ppPKI)
{
    if (NULL == pCertAuth || NULL == ppPKI)
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }

    SOPC_CertificateList* caCert = NULL;
    SOPC_PKIProvider* pki = NULL;
    SOPC_ReturnStatus status = SOPC_KeyManager_SerializedCertificate_Deserialize(pCertAuth, &caCert);

    /* Check the CRL-CA association before creating the PKI */
    if (SOPC_STATUS_OK == status)
    {
        bool match = false;
        status = SOPC_KeyManager_CertificateList_MatchCRLList(caCert, pRevocationList, &match);
        if (SOPC_STATUS_OK == status && !match)
        {
            /* mbedtls does not verify that each CA has a CRL, so we must do it ourselves
             * We must fail here, otherwise we can't report misconfigurations to the users */
            status = SOPC_STATUS_NOK;
            printf(
                "> PKI creation error: Not all certificate authorities have a single certificate revocation list! "
                "Certificates issued by these CAs will be refused.\n");
        }
    }

    if (SOPC_STATUS_OK == status)
    {
        pki = create_pkistack(caCert, NULL, NULL, pRevocationList, NULL);
        if (NULL == pki)
        {
            status = SOPC_STATUS_OUT_OF_MEMORY;
        }
    }

    if (SOPC_STATUS_OK == status)
    {
        *ppPKI = pki;
    }
    /* Clear partial alloc */
    else
    {
        SOPC_KeyManager_Certificate_Free(caCert);
        SOPC_Free(pki);
    }

    return status;
}

SOPC_ReturnStatus SOPC_PKIProviderStack_CreateFromPaths(char** lPathTrustedIssuers,
                                                        char** lPathIssuedCerts,
                                                        char** lPathUntrustedIssuers,
                                                        char** lPathCRL,
                                                        SOPC_PKIProvider** ppPKI)
{
    if (NULL == lPathTrustedIssuers || NULL == lPathIssuedCerts || NULL == lPathUntrustedIssuers || NULL == lPathCRL ||
        NULL == ppPKI)
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }

    /* Creates a chain that contain all issuers (trusted and untrusted)
     * which will be used to verify the issued certificates.
     * Then creates the trusted issuers list, which is used to verify the other certificates.
     * In practise, as certificates are linked lists in mbedtls,
     * we create a single list, and chain them: untrusted -> trusted.
     */
    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    SOPC_CertificateList* untrusted = NULL;
    char* cur = *lPathUntrustedIssuers;
    while (NULL != cur && SOPC_STATUS_OK == status)
    {
        status = SOPC_KeyManager_Certificate_CreateOrAddFromFile(cur, &untrusted);
        ++lPathUntrustedIssuers;
        cur = *lPathUntrustedIssuers;
    }

    SOPC_CertificateList* issued = NULL;
    cur = *lPathIssuedCerts;
    while (NULL != cur && SOPC_STATUS_OK == status)
    {
        status = SOPC_KeyManager_Certificate_CreateOrAddFromFile(cur, &issued);
        ++lPathIssuedCerts;
        cur = *lPathIssuedCerts;
    }

    SOPC_CertificateList* trusted = NULL;
    cur = *lPathTrustedIssuers;
    while (NULL != cur && SOPC_STATUS_OK == status)
    {
        status = SOPC_KeyManager_Certificate_CreateOrAddFromFile(cur, &trusted);
        ++lPathTrustedIssuers;
        cur = *lPathTrustedIssuers;
    }

    SOPC_CRLList* crl = NULL;
    cur = *lPathCRL;
    while (NULL != cur && SOPC_STATUS_OK == status)
    {
        status = SOPC_KeyManager_CRL_CreateOrAddFromFile(cur, &crl);
        ++lPathCRL;
        cur = *lPathCRL;
    }

    /* Link the untrusted list with the trusted list */
    if (SOPC_STATUS_OK == status)
    {
        if (NULL != untrusted && NULL != trusted)
        {
            mbedtls_x509_crt* crt = &untrusted->crt;
            /* crt should not be NULL, as either untrusted is NULL or at least one cert was created */
            assert(NULL != crt);
            while (NULL != crt->next)
            {
                crt = crt->next;
            }
            /* crt is now the last certificate of the chain, link it with trusted */
            crt->next = &trusted->crt;
        }
        else if (NULL != trusted)
        {
            /* When there are no untrusted, we must create the structure */
            /* TODO: avoid the duplication of the first element of trusted */
            status =
                SOPC_KeyManager_Certificate_CreateOrAddFromDER(trusted->crt.raw.p, (uint32_t) trusted->crt.raw.len, &untrusted);
            if (SOPC_STATUS_OK == status)
            {
                untrusted->crt.next = &trusted->crt;
            }
        }
        else
        {
            assert(false && "TODO: handle the case where NULL trusted and NULL untrusted");
        }
    }

    /* Check the CRL-CA association before creating the PKI */
    if (SOPC_STATUS_OK == status)
    {
        bool match = false;
        status = SOPC_KeyManager_CertificateList_MatchCRLList(untrusted, crl, &match);
        if (SOPC_STATUS_OK == status && !match)
        {
            /* mbedtls does not verify that each CA has a CRL, so we must do it ourselves.
             * We must fail here, otherwise we can't report misconfigurations to the users */
            status = SOPC_STATUS_NOK;
            printf(
                "> PKI creation error: Not all certificate authorities have a single certificate revocation list! "
                "Certificates issued by these CAs will be refused.\n");
        }
    }

    /* TODO: Check that all issued certificates have a CA in untrusted
     * This is already checked when receiving an issued certificate,
     * but it would be useful to report this misconfiguration.
     */

    /* Simpler case: check and warn that there is at least an issued certificate or an issuer */
    if (SOPC_STATUS_OK == status)
    {
        bool bIssuedXorUntrusted = (NULL == untrusted) ^ (NULL == issued);
        if (bIssuedXorUntrusted)
        {
            printf(
                "> PKI creation warning: issued certificates are given but no untrusted CA are given, or untrusted "
                "certificates are given but no issued certificates are given. Issued certificates without untrusted CA "
                "will never be accepted while creating a secure connection.\n");
        }
    }

    SOPC_PKIProvider* pki = NULL;
    if (SOPC_STATUS_OK == status)
    {
        pki = create_pkistack(trusted, issued, untrusted, crl, NULL);
        if (NULL == pki)
        {
            status = SOPC_STATUS_OUT_OF_MEMORY;
        }
    }

    if (SOPC_STATUS_OK == status)
    {
        *ppPKI = pki;
    }
    /* Clear partial alloc */
    else
    {
        /* Deleting the untrusted list will also clear the trusted list, as they are linked.
         * mbedtls zeroizes the elements upon clear, so we can call Certificate_Free on issuers.
         */
        SOPC_KeyManager_Certificate_Free(untrusted);
        SOPC_KeyManager_Certificate_Free(trusted);
        SOPC_KeyManager_Certificate_Free(issued);
        SOPC_KeyManager_CRL_Free(crl);
        SOPC_Free(pki);
    }

    return status;
}
