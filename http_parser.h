#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <stddef.h>
#define MAX_NUM_HEADERS 10
#define MAX_NUM_PARAMETERS 10
typedef struct
{
    char *authorization;
    char *host;
    // Add more header fields as needed
} HttpHeaders;

typedef struct
{
    HttpHeaders headers;
    int num_headers;
    int num_params;
    char *body;

    struct
    {
        char key[256];
        char value[256];
    } params[MAX_NUM_PARAMETERS];
} HttpRequest;

void parse_http_request(const char *request, HttpRequest *http_request);

#endif /* HTTP_PARSER_H */
