/*
 * Defines the cryptographic profiles: the set of functions associated to each cryptographic profiles.
 * These profiles are defined as struct of pointers. These immutable struct are extern and const, so that a profile
 * could not be modified before execution time.
 *
 *  Created on: Sep 9, 2016
 *      Author: PAB
 */

#include <stdlib.h>
#include <string.h>

#include "crypto_profiles.h"
#include "mbedtls/aes.h"
#include "mbedtls/md.h"


/* Security policy "Basic256Sha256", as of 09/09/2016:
 * SymmetricSignatureAlgorithm – Hmac_Sha256
 * SymmetricEncryptionAlgorithm – Aes256_CBC
 * AsymmetricSignatureAlgorithm – Rsa_Sha256
 * AsymmetricKeyWrapAlgorithm – KwRsaOaep
 * AsymmetricEncryptionAlgorithm – Rsa_Oaep
 * KeyDerivationAlgorithm – PSHA256
 * DerivedSignatureKeyLength – 256
 * MinAsymmetricKeyLength – 2048
 * MaxAsymmetricKeyLength – 4096
 * CertificateSignatureAlgorithm – Sha256
 */


// TODO: it is called CryptoProvider, why would it be in a crypto_profiles_*.c ?
static StatusCode CryptoProvider_SymmEncrypt_AES256(const CryptoProvider *pProvider,
                                                    const uint8_t *pInput,
                                                    uint32_t lenPlainText,
                                                    const KeyBuffer *pKey,
                                                    const uint8_t *pIV,
                                                    uint8_t *pOutput,
                                                    uint32_t lenOutput)
{
    mbedtls_aes_context aes; // Performance note: a context is initialized each time, as the _setkey operation initialize a new context.
    unsigned char iv_cpy[16]; // Performance note: IV is modified during the operation, so it should be copied first

    if(lenOutput < lenPlainText)
        return STATUS_INVALID_PARAMETERS;

    memcpy(iv_cpy, pIV, 16);

    if(mbedtls_aes_setkey_enc(&aes, (unsigned char *)pKey, 256) != 0)
        return STATUS_INVALID_PARAMETERS;
    if(mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, lenPlainText, iv_cpy, (unsigned char *)pInput, (unsigned char *)pOutput) != 0)
        return STATUS_INVALID_PARAMETERS;

    return STATUS_OK;
}


static StatusCode CryptoProvider_SymmDecrypt_AES256(const CryptoProvider *pProvider,
                                       const uint8_t *pInput,
                                       uint32_t lenCipherText,
                                       const KeyBuffer *pKey,
                                       const uint8_t *pIV,
                                       uint8_t *pOutput,
                                       uint32_t lenOutput)
{
    mbedtls_aes_context aes; // Performance note: a context is initialized each time, as the _setkey operation initialize a new context.
    unsigned char iv_cpy[16]; // Performance note: IV is modified during the operation, so it should be copied first

    if(lenOutput < lenCipherText)
        return STATUS_INVALID_PARAMETERS;

    memcpy(iv_cpy, pIV, 16);

    if(mbedtls_aes_setkey_dec(&aes, (unsigned char *)pKey, 256) != 0)
        return STATUS_INVALID_PARAMETERS;
    if(mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, lenCipherText, iv_cpy, (unsigned char *)pInput, (unsigned char *)pOutput) != 0)
        return STATUS_INVALID_PARAMETERS;

    return STATUS_OK;
}


static StatusCode CryptoProvider_SymmSign_HMAC_SHA256(const CryptoProvider *pProvider,
                                                      const uint8_t *pInput,
                                                      uint32_t lenInput,
                                                      const uint8_t *pKey,
                                                      uint8_t *pOutput)
{
    const mbedtls_md_info_t *pinfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    uint32_t lenKey;

    if(UA_NULL == pProvider || UA_NULL == pProvider->pProfile || UA_NULL == pInput || UA_NULL == pKey || UA_NULL == pOutput)
        return STATUS_INVALID_PARAMETERS;

    if(CryptoProvider_Symmetric_GetKeyLength_Low(pProvider, &lenKey) != STATUS_OK)
        return STATUS_NOK;

    if(mbedtls_md_hmac(pinfo, pKey, lenKey, pInput, lenInput, pOutput) != 0)
        return STATUS_NOK;

    return STATUS_OK;
}


static StatusCode CryptoProvider_SymmVerify_HMAC_SHA256(const CryptoProvider *pProvider,
                                                        const uint8_t *pInput,
                                                        uint32_t lenInput,
                                                        const uint8_t *pKey,
                                                        const uint8_t *pSignature)
{
    const mbedtls_md_info_t *pinfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    uint32_t lenKey, lenSig;
    uint8_t *pCalcSig;
    StatusCode status = STATUS_OK;

    if(UA_NULL == pProvider || UA_NULL == pProvider->pProfile || UA_NULL == pInput || UA_NULL == pKey || UA_NULL == pSignature)
        return STATUS_INVALID_PARAMETERS;

    if(CryptoProvider_Symmetric_GetKeyLength_Low(pProvider, &lenKey) != STATUS_OK)
        return STATUS_NOK;

    if(CryptoProvider_SymmetricSignature_GetLength_Low(pProvider, &lenSig) != STATUS_OK)
        return STATUS_NOK;

    pCalcSig = malloc(lenSig);
    if(UA_NULL == pCalcSig)
        return STATUS_NOK;

    status = mbedtls_md_hmac(pinfo, pKey, lenKey, pInput, lenInput, pCalcSig) != 0 ? STATUS_NOK : STATUS_OK;
    if(STATUS_OK == status)
        status = memcmp(pSignature, pCalcSig, lenSig) != 0 ? STATUS_NOK : STATUS_OK;

    free(pCalcSig);

    return status;
}


const CryptoProfile g_cpBasic256Sha256 = {
        .SecurityPolicyID = SecurityPolicy_Basic256Sha256_ID,
        .DerivedSignatureKeyBitLength = 256,
        .MinAsymmetricKeyBitLength = 2048,
        .MaxAsymmetricKeyBitLength = 4096,
        .pFnSymmEncrypt = &CryptoProvider_SymmEncrypt_AES256,
        .pFnSymmDecrypt = &CryptoProvider_SymmDecrypt_AES256,
        .pFnSymmSign = &CryptoProvider_SymmSign_HMAC_SHA256,
        .pFnSymmVerif = &CryptoProvider_SymmVerify_HMAC_SHA256,
};


#ifdef __cplusplus
}
#endif
