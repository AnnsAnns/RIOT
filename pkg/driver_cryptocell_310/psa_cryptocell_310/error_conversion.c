/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @file
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 driver APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa_error.h"

psa_status_t CRYS_to_psa_error(CRYSError_t error)
{
    switch (error) {
    case CRYS_OK:
        return PSA_SUCCESS;
    case CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR:
    case CRYS_HASH_IS_NOT_SUPPORTED:
        return PSA_ERROR_NOT_SUPPORTED;
    case CRYS_HASH_USER_CONTEXT_CORRUPTED_ERROR:
        return PSA_ERROR_CORRUPTION_DETECTED;
    case CRYS_ECDSA_VERIFY_INCONSISTENT_VERIFY_ERROR:
    case CRYS_ECEDW_SIGN_VERIFY_FAILED_ERROR:
        return PSA_ERROR_INVALID_SIGNATURE;
    case CRYS_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR:
    case CRYS_ECDSA_SIGN_USER_PRIV_KEY_VALIDATION_TAG_ERROR:
    case CRYS_ECDSA_VERIFY_USER_CONTEXT_VALIDATION_TAG_ERROR:
    case CRYS_ECDSA_VERIFY_SIGNER_PUBL_KEY_VALIDATION_TAG_ERROR:
    case CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_IN_PTR_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_PTR_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR:
    case CRYS_ECDSA_VERIFY_INVALID_DOMAIN_ID_ERROR:
    case CRYS_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR:
    case CRYS_ECDSA_VERIFY_INVALID_SIGNER_PUBL_KEY_PTR_ERROR:
    case CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR:
    case CRYS_HASH_DATA_IN_POINTER_INVALID_ERROR:
    case CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_SIZE_ERROR:
    case CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_PTR_ERROR:
    case CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_EPHEMERAL_KEY_PTR_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_RND_CONTEXT_PTR_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_RND_FUNCTION_PTR_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_USER_PRIV_KEY_PTR_ERROR:
    case CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_PTR_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_PTR_ERROR:
    case CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR:
    case CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR:
    case CRYS_ECC_ILLEGAL_HASH_MODE_ERROR:
    case CRYS_ECPKI_ILLEGAL_DOMAIN_ID_ERROR:
    case CRYS_ECPKI_DOMAIN_PTR_ERROR:
    case CRYS_ECPKI_RND_CONTEXT_PTR_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_COMPRESSION_MODE_ERROR:
    case CRYS_ECPKI_BUILD_KEY_ILLEGAL_DOMAIN_ID_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_IN_PTR_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_SIZE_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_DATA_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_IN_PTR_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_SIZE_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_CHECK_MODE_ERROR:
    case CRYS_ECPKI_BUILD_KEY_INVALID_TEMP_BUFF_PTR_ERROR:
    case CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR:
    case CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_COMPRESSION_MODE_ERROR:
    case CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_EXTERN_PUBL_KEY_PTR_ERROR:
    case CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_PTR_ERROR:
    case CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_ERROR:
    case CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_DOMAIN_ID_ERROR:
    case CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_VALIDATION_TAG_ERROR:
    case CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_DATA_ERROR:
    case CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR:
    case CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_EXTERN_PRIV_KEY_PTR_ERROR:
    case CRYS_ECPKI_EXPORT_PRIV_KEY_ILLEGAL_VALIDATION_TAG_ERROR:
    case CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_PRIV_KEY_SIZE_PTR_ERROR:
    case CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_PRIV_KEY_SIZE_ERROR:
    case CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_PRIV_KEY_DATA_ERROR:
    case CRYS_ECPKI_BUILD_DOMAIN_ID_IS_NOT_VALID_ERROR:
    case CRYS_ECPKI_BUILD_DOMAIN_DOMAIN_PTR_ERROR:
    case CRYS_ECPKI_BUILD_DOMAIN_EC_PARAMETR_PTR_ERROR:
    case CRYS_ECPKI_BUILD_DOMAIN_EC_PARAMETR_SIZE_ERROR:
    case CRYS_ECPKI_BUILD_DOMAIN_COFACTOR_PARAMS_ERROR:
    case CRYS_ECPKI_BUILD_DOMAIN_SECURITY_STRENGTH_ERROR:
    case CRYS_ECPKI_BUILD_SCA_RESIST_ILLEGAL_MODE_ERROR:
    case CRYS_ECDH_SVDP_DH_INVALID_PARTNER_PUBL_KEY_PTR_ERROR:
    case CRYS_ECDH_SVDP_DH_PARTNER_PUBL_KEY_VALID_TAG_ERROR:
    case CRYS_ECDH_SVDP_DH_INVALID_USER_PRIV_KEY_PTR_ERROR:
    case CRYS_ECDH_SVDP_DH_USER_PRIV_KEY_VALID_TAG_ERROR:
    case CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_PTR_ERROR:
    case CRYS_ECDH_SVDP_DH_INVALID_TEMP_DATA_PTR_ERROR:
    case CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_PTR_ERROR:
    case CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR:
    case CRYS_ECDH_SVDP_DH_ILLEGAL_DOMAIN_ID_ERROR:
    case CRYS_ECPKI_GEN_KEY_INVALID_PRIVATE_KEY_PTR_ERROR:
    case CRYS_ECPKI_GEN_KEY_INVALID_PUBLIC_KEY_PTR_ERROR:
    case CRYS_ECPKI_GEN_KEY_INVALID_TEMP_DATA_PTR_ERROR:
    case CRYS_ECPKI_INVALID_RND_FUNC_PTR_ERROR:
    case CRYS_ECPKI_INVALID_RND_CTX_PTR_ERROR:
    case CRYS_ECPKI_INVALID_DOMAIN_ID_ERROR:
    case CRYS_ECPKI_INVALID_PRIV_KEY_TAG_ERROR:
    case CRYS_ECPKI_INVALID_PUBL_KEY_TAG_ERROR:
    case CRYS_ECPKI_INVALID_DATA_IN_PASSED_STRUCT_ERROR:
    case CRYS_HASH_DATA_SIZE_ILLEGAL:
    case CRYS_HASH_INVALID_RESULT_BUFFER_POINTER_ERROR:
    case CRYS_HASH_ILLEGAL_PARAMS_ERROR:
    case CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR:
    case CRYS_HASH_LAST_BLOCK_ALREADY_PROCESSED_ERROR:
    case CRYS_HASH_CTX_SIZES_ERROR:
    case CRYS_ECEDW_INVALID_INPUT_POINTER_ERROR:
    case CRYS_ECEDW_INVALID_INPUT_SIZE_ERROR:
    case CRYS_ECEDW_INVALID_SCALAR_SIZE_ERROR:
    case CRYS_ECEDW_INVALID_SCALAR_DATA_ERROR:
    case CRYS_ECEDW_RND_CONTEXT_PTR_INVALID_ERROR:
    case CRYS_ECEDW_RND_GEN_VECTOR_FUNC_ERROR:
    case CRYS_ECMONT_INVALID_INPUT_POINTER_ERROR:
    case CRYS_ECMONT_INVALID_INPUT_SIZE_ERROR:
    case CRYS_ECMONT_INVALID_DOMAIN_ID_ERROR:
    case CRYS_ECEDW_PKI_ERROR:
    case CRYS_ECMONT_PKI_ERROR:
    case CRYS_ECMONT_IS_NOT_SUPPORTED:
    case CRYS_ECEDW_IS_NOT_SUPPORTED:
        return PSA_ERROR_INVALID_ARGUMENT;
    default:
        return PSA_ERROR_GENERIC_ERROR;
    }
}

psa_status_t SaSi_to_psa_error(SaSiStatus error)
{
    switch (error) {
    case SASI_AES_INVALID_USER_CONTEXT_POINTER_ERROR:
    case SASI_AES_INVALID_IV_OR_TWEAK_PTR_ERROR:
    case SASI_AES_ILLEGAL_OPERATION_MODE_ERROR:
    case SASI_AES_ILLEGAL_KEY_SIZE_ERROR:
    case SASI_AES_INVALID_KEY_POINTER_ERROR:
    case SASI_AES_INVALID_ENCRYPT_MODE_ERROR:
    case SASI_AES_DATA_IN_POINTER_INVALID_ERROR:
    case SASI_AES_DATA_OUT_POINTER_INVALID_ERROR:
    case SASI_AES_DATA_IN_SIZE_ILLEGAL:
    case SASI_AES_DATA_OUT_DATA_IN_OVERLAP_ERROR:
    case SASI_AES_DATA_OUT_SIZE_POINTER_INVALID_ERROR:
    case SASI_AES_CTX_SIZES_ERROR:
    case SASI_AES_ILLEGAL_PARAMS_ERROR:
    case SASI_AES_CTR_ILLEGAL_BLOCK_OFFSET_ERROR:
    case SASI_AES_CTR_ILLEGAL_COUNTER_ERROR:
        return PSA_ERROR_INVALID_ARGUMENT;
    case SASI_AES_DATA_IN_BUFFER_SIZE_ERROR:
    case SASI_AES_DATA_OUT_BUFFER_SIZE_ERROR:
        return PSA_ERROR_BUFFER_TOO_SMALL;
    case SASI_AES_ILLEGAL_PADDING_TYPE_ERROR:
    case SASI_AES_INCORRECT_PADDING_ERROR:
        return PSA_ERROR_INVALID_PADDING;
    case SASI_AES_CORRUPTED_OUTPUT_ERROR:
    case SASI_AES_USER_CONTEXT_CORRUPTED_ERROR:
        return PSA_ERROR_CORRUPTION_DETECTED;
    case SASI_AES_DECRYPTION_NOT_ALLOWED_ON_THIS_MODE:
    case SASI_AES_ADDITIONAL_BLOCK_NOT_PERMITTED_ERROR:
        return PSA_ERROR_NOT_PERMITTED;
    case SASI_AES_KEY_TYPE_NOT_SUPPORTED_ERROR:
    case SASI_AES_IS_NOT_SUPPORTED:
        return PSA_ERROR_NOT_SUPPORTED;
    case SASI_OUT_OF_RESOURCE_ERROR:
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    default:
        return PSA_ERROR_GENERIC_ERROR;
    }
}

const char *cryptocell310_status_to_humanly_readable(uint32_t status)
{
    switch(status) {
        case CRYS_ECDH_SVDP_DH_NOT_CONCENT_PUBL_AND_PRIV_DOMAIN_ID_ERROR:
            return "CRYS_ECDH_SVDP_DH_NOT_CONCENT_PUBL_AND_PRIV_DOMAIN_ID_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_IS_EPHEMER_KEY_INTERNAL_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_IS_EPHEMER_KEY_INTERNAL_ERROR";
        case CRYS_ECDSA_SIGN_SIGNING_ERROR:
            return "CRYS_ECDSA_SIGN_SIGNING_ERROR";
        case CRYS_ECDSA_VERIFY_INCONSISTENT_VERIFY_ERROR:
            return "CRYS_ECDSA_VERIFY_INCONSISTENT_VERIFY_ERROR";
        case CRYS_ECPKI_PKI_INTERNAL_ERROR:
            return "CRYS_ECPKI_PKI_INTERNAL_ERROR";
        case CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR:
            return "CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR";
        case CRYS_HASH_IS_NOT_SUPPORTED:
            return "CRYS_HASH_IS_NOT_SUPPORTED";
        case CRYS_HASH_USER_CONTEXT_CORRUPTED_ERROR:
            return "CRYS_HASH_USER_CONTEXT_CORRUPTED_ERROR";
        case CRYS_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR:
            return "CRYS_ECDSA_SIGN_USER_CONTEXT_VALIDATION_TAG_ERROR";
        case CRYS_ECDSA_SIGN_USER_PRIV_KEY_VALIDATION_TAG_ERROR:
            return "CRYS_ECDSA_SIGN_USER_PRIV_KEY_VALIDATION_TAG_ERROR";
        case CRYS_ECDSA_VERIFY_USER_CONTEXT_VALIDATION_TAG_ERROR:
            return "CRYS_ECDSA_VERIFY_USER_CONTEXT_VALIDATION_TAG_ERROR";
        case CRYS_ECDSA_VERIFY_SIGNER_PUBL_KEY_VALIDATION_TAG_ERROR:
            return "CRYS_ECDSA_VERIFY_SIGNER_PUBL_KEY_VALIDATION_TAG_ERROR";
        case CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_IN_PTR_ERROR:
            return "CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_IN_PTR_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_PTR_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_PTR_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_MESSAGE_DATA_IN_SIZE_ERROR";
        case CRYS_ECDSA_VERIFY_INVALID_DOMAIN_ID_ERROR:
            return "CRYS_ECDSA_VERIFY_INVALID_DOMAIN_ID_ERROR";
        case CRYS_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR:
            return "CRYS_ECDSA_VERIFY_INVALID_USER_CONTEXT_PTR_ERROR";
        case CRYS_ECDSA_VERIFY_INVALID_SIGNER_PUBL_KEY_PTR_ERROR:
            return "CRYS_ECDSA_VERIFY_INVALID_SIGNER_PUBL_KEY_PTR_ERROR";
        case CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR:
            return "CRYS_ECDSA_VERIFY_ILLEGAL_HASH_OP_MODE_ERROR";
        case CRYS_HASH_DATA_IN_POINTER_INVALID_ERROR:
            return "CRYS_HASH_DATA_IN_POINTER_INVALID_ERROR";
        case CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_SIZE_ERROR:
            return "CRYS_ECDSA_VERIFY_INVALID_SIGNATURE_SIZE_ERROR";
        case CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_PTR_ERROR:
            return "CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_PTR_ERROR";
        case CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR:
            return "CRYS_ECDSA_VERIFY_INVALID_MESSAGE_DATA_IN_SIZE_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_EPHEMERAL_KEY_PTR_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_EPHEMERAL_KEY_PTR_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_RND_CONTEXT_PTR_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_RND_CONTEXT_PTR_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_RND_FUNCTION_PTR_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_RND_FUNCTION_PTR_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_DOMAIN_ID_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_USER_CONTEXT_PTR_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_USER_PRIV_KEY_PTR_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_USER_PRIV_KEY_PTR_ERROR";
        case CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR:
            return "CRYS_ECDSA_SIGN_ILLEGAL_HASH_OP_MODE_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_PTR_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_PTR_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_PTR_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_PTR_ERROR";
        case CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR:
            return "CRYS_ECDSA_SIGN_INVALID_SIGNATURE_OUT_SIZE_ERROR";
        case CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR:
            return "CRYS_ECC_ILLEGAL_PARAMS_ACCORDING_TO_PRIV_ERROR";
        case CRYS_ECC_ILLEGAL_HASH_MODE_ERROR:
            return "CRYS_ECC_ILLEGAL_HASH_MODE_ERROR";
        case CRYS_ECPKI_ILLEGAL_DOMAIN_ID_ERROR:
            return "CRYS_ECPKI_ILLEGAL_DOMAIN_ID_ERROR";
        case CRYS_ECPKI_DOMAIN_PTR_ERROR:
            return "CRYS_ECPKI_DOMAIN_PTR_ERROR";
        case CRYS_ECPKI_RND_CONTEXT_PTR_ERROR:
            return "CRYS_ECPKI_RND_CONTEXT_PTR_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_COMPRESSION_MODE_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_COMPRESSION_MODE_ERROR";
        case CRYS_ECPKI_BUILD_KEY_ILLEGAL_DOMAIN_ID_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_ILLEGAL_DOMAIN_ID_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_IN_PTR_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_IN_PTR_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_SIZE_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_SIZE_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_DATA_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_PRIV_KEY_DATA_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_IN_PTR_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_IN_PTR_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_SIZE_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_SIZE_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_PUBL_KEY_DATA_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_CHECK_MODE_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_CHECK_MODE_ERROR";
        case CRYS_ECPKI_BUILD_KEY_INVALID_TEMP_BUFF_PTR_ERROR:
            return "CRYS_ECPKI_BUILD_KEY_INVALID_TEMP_BUFF_PTR_ERROR";
        case CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR:
            return "CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_USER_PUBL_KEY_PTR_ERROR";
        case CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_COMPRESSION_MODE_ERROR:
            return "CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_COMPRESSION_MODE_ERROR";
        case CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_EXTERN_PUBL_KEY_PTR_ERROR:
            return "CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_EXTERN_PUBL_KEY_PTR_ERROR";
        case CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_PTR_ERROR:
            return "CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_PTR_ERROR";
        case CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_ERROR:
            return "CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_SIZE_ERROR";
        case CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_DOMAIN_ID_ERROR:
            return "CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_DOMAIN_ID_ERROR";
        case CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_VALIDATION_TAG_ERROR:
            return "CRYS_ECPKI_EXPORT_PUBL_KEY_ILLEGAL_VALIDATION_TAG_ERROR";
        case CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_DATA_ERROR:
            return "CRYS_ECPKI_EXPORT_PUBL_KEY_INVALID_PUBL_KEY_DATA_ERROR";
        case CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR:
            return "CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_USER_PRIV_KEY_PTR_ERROR";
        case CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_EXTERN_PRIV_KEY_PTR_ERROR:
            return "CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_EXTERN_PRIV_KEY_PTR_ERROR";
        case CRYS_ECPKI_EXPORT_PRIV_KEY_ILLEGAL_VALIDATION_TAG_ERROR:
            return "CRYS_ECPKI_EXPORT_PRIV_KEY_ILLEGAL_VALIDATION_TAG_ERROR";
        case CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_PRIV_KEY_SIZE_PTR_ERROR:
            return "CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_PRIV_KEY_SIZE_PTR_ERROR";
        case CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_PRIV_KEY_SIZE_ERROR:
            return "CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_PRIV_KEY_SIZE_ERROR";
        case CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_PRIV_KEY_DATA_ERROR:
            return "CRYS_ECPKI_EXPORT_PRIV_KEY_INVALID_PRIV_KEY_DATA_ERROR";
        case CRYS_ECPKI_BUILD_DOMAIN_ID_IS_NOT_VALID_ERROR:
            return "CRYS_ECPKI_BUILD_DOMAIN_ID_IS_NOT_VALID_ERROR";
        case CRYS_ECPKI_BUILD_DOMAIN_DOMAIN_PTR_ERROR:
            return "CRYS_ECPKI_BUILD_DOMAIN_DOMAIN_PTR_ERROR";
        case CRYS_ECPKI_BUILD_DOMAIN_EC_PARAMETR_PTR_ERROR:
            return "CRYS_ECPKI_BUILD_DOMAIN_EC_PARAMETR_PTR_ERROR";
        case CRYS_ECPKI_BUILD_DOMAIN_EC_PARAMETR_SIZE_ERROR:
            return "CRYS_ECPKI_BUILD_DOMAIN_EC_PARAMETR_SIZE_ERROR";
        case CRYS_ECPKI_BUILD_DOMAIN_COFACTOR_PARAMS_ERROR:
            return "CRYS_ECPKI_BUILD_DOMAIN_COFACTOR_PARAMS_ERROR";
        case CRYS_ECPKI_BUILD_DOMAIN_SECURITY_STRENGTH_ERROR:
            return "CRYS_ECPKI_BUILD_DOMAIN_SECURITY_STRENGTH_ERROR";
        case CRYS_ECPKI_BUILD_SCA_RESIST_ILLEGAL_MODE_ERROR:
            return "CRYS_ECPKI_BUILD_SCA_RESIST_ILLEGAL_MODE_ERROR";
        case CRYS_ECDH_SVDP_DH_INVALID_PARTNER_PUBL_KEY_PTR_ERROR:
            return "CRYS_ECDH_SVDP_DH_INVALID_PARTNER_PUBL_KEY_PTR_ERROR";
        case CRYS_ECDH_SVDP_DH_PARTNER_PUBL_KEY_VALID_TAG_ERROR:
            return "CRYS_ECDH_SVDP_DH_PARTNER_PUBL_KEY_VALID_TAG_ERROR";
        case CRYS_ECDH_SVDP_DH_INVALID_USER_PRIV_KEY_PTR_ERROR:
            return "CRYS_ECDH_SVDP_DH_INVALID_USER_PRIV_KEY_PTR_ERROR";
        case CRYS_ECDH_SVDP_DH_USER_PRIV_KEY_VALID_TAG_ERROR:
            return "CRYS_ECDH_SVDP_DH_USER_PRIV_KEY_VALID_TAG_ERROR";
        case CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_PTR_ERROR:
            return "CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_PTR_ERROR";
        case CRYS_ECDH_SVDP_DH_INVALID_TEMP_DATA_PTR_ERROR:
            return "CRYS_ECDH_SVDP_DH_INVALID_TEMP_DATA_PTR_ERROR";
        case CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_PTR_ERROR:
            return "CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_PTR_ERROR";
        case CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR:
            return "CRYS_ECDH_SVDP_DH_INVALID_SHARED_SECRET_VALUE_SIZE_ERROR";
        case CRYS_ECDH_SVDP_DH_ILLEGAL_DOMAIN_ID_ERROR:
            return "CRYS_ECDH_SVDP_DH_ILLEGAL_DOMAIN_ID_ERROR";
        case CRYS_ECPKI_GEN_KEY_INVALID_PRIVATE_KEY_PTR_ERROR:
            return "CRYS_ECPKI_GEN_KEY_INVALID_PRIVATE_KEY_PTR_ERROR";
        case CRYS_ECPKI_GEN_KEY_INVALID_PUBLIC_KEY_PTR_ERROR:
            return "CRYS_ECPKI_GEN_KEY_INVALID_PUBLIC_KEY_PTR_ERROR";
        case CRYS_ECPKI_GEN_KEY_INVALID_TEMP_DATA_PTR_ERROR:
            return "CRYS_ECPKI_GEN_KEY_INVALID_TEMP_DATA_PTR_ERROR";
        case CRYS_ECPKI_INVALID_RND_FUNC_PTR_ERROR:
            return "CRYS_ECPKI_INVALID_RND_FUNC_PTR_ERROR";
        case CRYS_ECPKI_INVALID_RND_CTX_PTR_ERROR:
            return "CRYS_ECPKI_INVALID_RND_CTX_PTR_ERROR";
        case CRYS_ECPKI_INVALID_DOMAIN_ID_ERROR:
            return "CRYS_ECPKI_INVALID_DOMAIN_ID_ERROR";
        case CRYS_ECPKI_INVALID_PRIV_KEY_TAG_ERROR:
            return "CRYS_ECPKI_INVALID_PRIV_KEY_TAG_ERROR";
        case CRYS_ECPKI_INVALID_PUBL_KEY_TAG_ERROR:
            return "CRYS_ECPKI_INVALID_PUBL_KEY_TAG_ERROR";
        case CRYS_ECPKI_INVALID_DATA_IN_PASSED_STRUCT_ERROR:
            return "CRYS_ECPKI_INVALID_DATA_IN_PASSED_STRUCT_ERROR";
        case CRYS_HASH_DATA_SIZE_ILLEGAL:
            return "CRYS_HASH_DATA_SIZE_ILLEGAL";
        case CRYS_HASH_INVALID_RESULT_BUFFER_POINTER_ERROR:
            return "CRYS_HASH_INVALID_RESULT_BUFFER_POINTER_ERROR";
        case CRYS_HASH_ILLEGAL_PARAMS_ERROR:
            return "CRYS_HASH_ILLEGAL_PARAMS_ERROR";
        case CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR:
            return "CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR";
        case CRYS_HASH_LAST_BLOCK_ALREADY_PROCESSED_ERROR:
            return "CRYS_HASH_LAST_BLOCK_ALREADY_PROCESSED_ERROR";
        case CRYS_HASH_CTX_SIZES_ERROR:
            return "CRYS_HASH_CTX_SIZES_ERROR";
        case SASI_AES_INVALID_USER_CONTEXT_POINTER_ERROR:
            return "SASI_AES_INVALID_USER_CONTEXT_POINTER_ERROR";
        case SASI_AES_INVALID_IV_OR_TWEAK_PTR_ERROR:
            return "SASI_AES_INVALID_IV_OR_TWEAK_PTR_ERROR";
        case SASI_AES_ILLEGAL_OPERATION_MODE_ERROR:
            return "SASI_AES_ILLEGAL_OPERATION_MODE_ERROR";
        case SASI_AES_ILLEGAL_KEY_SIZE_ERROR:
            return "SASI_AES_ILLEGAL_KEY_SIZE_ERROR";
        case SASI_AES_INVALID_KEY_POINTER_ERROR:
            return "SASI_AES_INVALID_KEY_POINTER_ERROR";
        case SASI_AES_INVALID_ENCRYPT_MODE_ERROR:
            return "SASI_AES_INVALID_ENCRYPT_MODE_ERROR";
        case SASI_AES_DATA_IN_POINTER_INVALID_ERROR:
            return "SASI_AES_DATA_IN_POINTER_INVALID_ERROR";
        case SASI_AES_DATA_OUT_POINTER_INVALID_ERROR:
            return "SASI_AES_DATA_OUT_POINTER_INVALID_ERROR";
        case SASI_AES_DATA_IN_SIZE_ILLEGAL:
            return "SASI_AES_DATA_IN_SIZE_ILLEGAL";
        case SASI_AES_DATA_OUT_DATA_IN_OVERLAP_ERROR:
            return "SASI_AES_DATA_OUT_DATA_IN_OVERLAP_ERROR";
        case SASI_AES_DATA_OUT_SIZE_POINTER_INVALID_ERROR:
            return "SASI_AES_DATA_OUT_SIZE_POINTER_INVALID_ERROR";
        case SASI_AES_CTX_SIZES_ERROR:
            return "SASI_AES_CTX_SIZES_ERROR";
        case SASI_AES_ILLEGAL_PARAMS_ERROR:
            return "SASI_AES_ILLEGAL_PARAMS_ERROR";
        case SASI_AES_CTR_ILLEGAL_BLOCK_OFFSET_ERROR:
            return "SASI_AES_CTR_ILLEGAL_BLOCK_OFFSET_ERROR";
        case SASI_AES_CTR_ILLEGAL_COUNTER_ERROR:
            return "SASI_AES_CTR_ILLEGAL_COUNTER_ERROR";
        case SASI_AES_DATA_IN_BUFFER_SIZE_ERROR:
            return "SASI_AES_DATA_IN_BUFFER_SIZE_ERROR";
        case SASI_AES_DATA_OUT_BUFFER_SIZE_ERROR:
            return "SASI_AES_DATA_OUT_BUFFER_SIZE_ERROR";
        case SASI_AES_ILLEGAL_PADDING_TYPE_ERROR:
            return "SASI_AES_ILLEGAL_PADDING_TYPE_ERROR";
        case SASI_AES_INCORRECT_PADDING_ERROR:
            return "SASI_AES_INCORRECT_PADDING_ERROR";
        case SASI_AES_CORRUPTED_OUTPUT_ERROR:
            return "SASI_AES_CORRUPTED_OUTPUT_ERROR";
        case SASI_AES_USER_CONTEXT_CORRUPTED_ERROR:
            return "SASI_AES_USER_CONTEXT_CORRUPTED_ERROR";
        case SASI_AES_DECRYPTION_NOT_ALLOWED_ON_THIS_MODE:
            return "SASI_AES_DECRYPTION_NOT_ALLOWED_ON_THIS_MODE";
        case SASI_AES_ADDITIONAL_BLOCK_NOT_PERMITTED_ERROR:
            return "SASI_AES_ADDITIONAL_BLOCK_NOT_PERMITTED_ERROR";
        case SASI_AES_KEY_TYPE_NOT_SUPPORTED_ERROR:
            return "SASI_AES_KEY_TYPE_NOT_SUPPORTED_ERROR";
        case SASI_AES_IS_NOT_SUPPORTED:
            return "SASI_AES_IS_NOT_SUPPORTED";
        case SASI_OUT_OF_RESOURCE_ERROR:
            return "SASI_OUT_OF_RESOURCE_ERROR";
        case CRYS_ECEDW_INVALID_INPUT_POINTER_ERROR:
            return "CRYS_ECEDW_INVALID_INPUT_POINTER_ERROR";
        case CRYS_ECEDW_INVALID_INPUT_SIZE_ERROR:
            return "CRYS_ECEDW_INVALID_INPUT_SIZE_ERROR";
        case CRYS_ECEDW_INVALID_SCALAR_SIZE_ERROR:
            return "CRYS_ECEDW_INVALID_SCALAR_SIZE_ERROR";
        case CRYS_ECEDW_INVALID_SCALAR_DATA_ERROR:
            return "CRYS_ECEDW_INVALID_SCALAR_DATA_ERROR";
        case CRYS_ECEDW_RND_CONTEXT_PTR_INVALID_ERROR:
            return "CRYS_ECEDW_RND_CONTEXT_PTR_INVALID_ERROR";
        case CRYS_ECEDW_RND_GEN_VECTOR_FUNC_ERROR:
            return "CRYS_ECEDW_RND_GEN_VECTOR_FUNC_ERROR";
        case CRYS_ECEDW_SIGN_VERIFY_FAILED_ERROR:
            return "CRYS_ECEDW_SIGN_VERIFY_FAILED_ERROR";
        case CRYS_ECMONT_INVALID_INPUT_POINTER_ERROR:
            return "CRYS_ECMONT_INVALID_INPUT_POINTER_ERROR";
        case CRYS_ECMONT_INVALID_INPUT_SIZE_ERROR:
            return "CRYS_ECMONT_INVALID_INPUT_SIZE_ERROR";
        case CRYS_ECMONT_INVALID_DOMAIN_ID_ERROR:
            return "CRYS_ECMONT_INVALID_DOMAIN_ID_ERROR";
        case CRYS_ECEDW_PKI_ERROR:
            return "CRYS_ECEDW_PKI_ERROR";
        case CRYS_ECMONT_PKI_ERROR:
            return "CRYS_ECMONT_PKI_ERROR";
        case CRYS_ECMONT_IS_NOT_SUPPORTED:
            return "CRYS_ECMONT_IS_NOT_SUPPORTED";
        case CRYS_ECEDW_IS_NOT_SUPPORTED:
            return "CRYS_ECEDW_IS_NOT_SUPPORTED";
        default:
            return "Error value not recognized";
    }

}
