#include "jwt_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, data, input_length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    *output_length = bufferPtr->length;
    char *encoded_data = (char *)malloc(*output_length + 1);
    memcpy(encoded_data, bufferPtr->data, *output_length);
    encoded_data[*output_length] = '\0';

    return encoded_data;
}

char *create_jwt(const char *payload, const char *secret_key)
{
    // Header
    const char *header = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";

    // Encode the header and payload
    size_t header_length = strlen(header);
    size_t payload_length = strlen(payload);
    size_t encoded_header_length, encoded_payload_length;
    char *encoded_header = base64_encode((const unsigned char *)header, header_length, &encoded_header_length);
    char *encoded_payload = base64_encode((const unsigned char *)payload, payload_length, &encoded_payload_length);

    // Prepare the signing input
    size_t signing_input_length = encoded_header_length + encoded_payload_length + 1;
    char *signing_input = (char *)malloc(signing_input_length + 1);
    snprintf(signing_input, signing_input_length + 1, "%s.%s", encoded_header, encoded_payload);

    // Sign the token
    unsigned int signature_length;
    unsigned char *signature = HMAC(EVP_sha256(), secret_key, strlen(secret_key), (unsigned char *)signing_input, signing_input_length, NULL, &signature_length);

    // Base64-encode the signature
    size_t encoded_signature_length;
    char *encoded_signature = base64_encode(signature, signature_length, &encoded_signature_length);

    // Concatenate the JWT token
    size_t jwt_length = encoded_header_length + encoded_payload_length + encoded_signature_length + 3;
    char *jwt_token = (char *)malloc(jwt_length + 1);
    snprintf(jwt_token, jwt_length + 1, "%s.%s.%s", encoded_header, encoded_payload, encoded_signature);

    // Clean up resources
    free(encoded_header);
    free(encoded_payload);
    free(signing_input);
    // free(signature);

    return jwt_token;
}

// int main()
// {
//     const char *payload = "{\"sub\":\"1234567890\",\"name\":\"John Doe\"}";
//     const char *secret_key = "your_secret_key";

//     char *jwt_token = create_jwt(payload, secret_key);
//     printf("JWT token: %s\n", jwt_token);

//     free(jwt_token);

//     return 0;
// }
