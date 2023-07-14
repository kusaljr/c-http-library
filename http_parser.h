#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <stddef.h>

typedef struct
{
    char *headers;
    int num_headers;
    char *body;
} HttpRequest;

void parse_http_request(const char *request, HttpRequest *http_request);

#endif /* HTTP_PARSER_H */
