#ifndef JWT_HELPER_H
#define JWT_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length);
char *create_jwt(const char *payload, const char *secret_key);

#endif /* JWT_HELPER_H */
