#include "jwt_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#define BIO_FLAGS_BASE64_NO_PADDING 0x100

char *base64url_encode(const unsigned char *data, size_t input_length, size_t *output_length)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // Set the BIO to use base64url encoding (no line breaks, no padding)
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL | BIO_FLAGS_BASE64_NO_PADDING);
    BIO_write(bio, data, input_length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    *output_length = bufferPtr->length;
    char *encoded_data = (char *)malloc(*output_length + 1);
    memcpy(encoded_data, bufferPtr->data, *output_length);
    encoded_data[*output_length] = '\0';

    // Convert base64 to base64url by replacing characters '+' with '-' and '/' with '_'
    for (size_t i = 0; i < *output_length; ++i)
    {
        if (encoded_data[i] == '+')
            encoded_data[i] = '-';
        else if (encoded_data[i] == '/')
            encoded_data[i] = '_';
    }

    // Remove padding characters '=' if present
    while (*output_length > 0 && encoded_data[*output_length - 1] == '=')
    {
        (*output_length)--;
        encoded_data[*output_length] = '\0';
    }

    return encoded_data;
}

char *sign_jwt(const char *payload, const char *secret_key)
{
    // Prepare the signing input
    const char *header = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";

    size_t header_length = strlen(header);
    size_t payload_length = strlen(payload);
    size_t signing_input_length = header_length + payload_length + 2; // +2 for dots ('.')
    char *signing_input = (char *)malloc(signing_input_length + 1);
    snprintf(signing_input, signing_input_length + 1, "%s.%s", header, payload);

    // Sign the token using HMAC with SHA-256
    unsigned int signature_length = EVP_MAX_MD_SIZE;
    unsigned char *signature = (unsigned char *)malloc(signature_length);
    HMAC(EVP_sha256(), secret_key, strlen(secret_key), (const unsigned char *)signing_input, signing_input_length, signature, &signature_length);

    // Base64url-encode the header, payload, and signature
    size_t encoded_header_length;
    char *encoded_header = base64url_encode((const unsigned char *)header, header_length, &encoded_header_length);

    size_t encoded_payload_length;
    char *encoded_payload = base64url_encode((const unsigned char *)payload, payload_length, &encoded_payload_length);

    size_t encoded_signature_length;
    char *encoded_signature = base64url_encode(signature, signature_length, &encoded_signature_length);

    // Concatenate the JWT token
    size_t jwt_length = encoded_header_length + encoded_payload_length + encoded_signature_length + 3; // +3 for dots ('.')
    char *jwt_token = (char *)malloc(jwt_length + 1);
    snprintf(jwt_token, jwt_length + 1, "%s.%s.%s", encoded_header, encoded_payload, encoded_signature);

    // Clean up resources
    free(signing_input);
    free(signature);
    free(encoded_header);
    free(encoded_payload);
    free(encoded_signature);

    return jwt_token;
}
