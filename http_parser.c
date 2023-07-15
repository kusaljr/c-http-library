#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_parser.h"

static void parse_headers(const char **request, HttpRequest *http_request);

static void parse_headers(const char **request, HttpRequest *http_request)
{
    // Skip the first line (request line)
    const char *end_of_line = strchr(*request, '\n');
    if (end_of_line == NULL)
    {
        fprintf(stderr, "Invalid request format: end of request line not found\n");
        exit(EXIT_FAILURE);
    }
    *request = end_of_line + 1;

    // Parse the headers until an empty line is encountered
    while (**request != '\0' && strncmp(*request, "\r\n", 2) != 0)
    {
        // Find the end of the current header line
        end_of_line = strchr(*request, '\n');
        if (end_of_line == NULL)
        {
            fprintf(stderr, "Invalid request format: end of header line not found\n");
            exit(EXIT_FAILURE);
        }

        // Extract the header line
        size_t line_length = end_of_line - *request;
        char *header_line = (char *)malloc(line_length + 1);
        strncpy(header_line, *request, line_length);
        header_line[line_length] = '\0';

        // Move to the next header line
        *request = end_of_line + 1;

        // Check if the header line contains a known header type
        const char *authorization_prefix = "Authorization: ";
        if (strncmp(header_line, authorization_prefix, strlen(authorization_prefix)) == 0)
        {
            // Handle Authorization header
            // Extract the value after the prefix
            char *authorization_value = header_line + strlen(authorization_prefix);

            // Store the value in the HttpRequest struct
            http_request->headers.authorization = strdup(authorization_value);
        }

        const char *host_prefix = "Host: ";
        if (strncmp(header_line, host_prefix, strlen(host_prefix)) == 0)
        {
            // Handle Host header
            // Extract the value after the prefix
            char *host_value = header_line + strlen(host_prefix);

            // Store the value in the HttpRequest struct
            http_request->headers.host = strdup(host_value);
        }

        free(header_line);
    }

    // Move to the start of the request body
    if (**request == '\r' && *(*request + 1) == '\n')
    {
        *request += 2;
    }
}

void parse_http_request(const char *request, HttpRequest *http_request)
{
    // Initialize the HttpRequest struct
    http_request->headers.authorization = NULL;
    http_request->headers.host = NULL;
    // Initialize other header fields as needed
    http_request->num_headers = 0;
    http_request->body = NULL;
    // Find the start of the request body
    const char *body_start = strstr(request, "\r\n\r\n");
    if (body_start == NULL)
    {
        body_start = strstr(request, "\n\n");
        if (body_start == NULL)
        {
            fprintf(stderr, "Invalid request format: no headers/body separator found\n");
            exit(EXIT_FAILURE);
        }
        body_start += 2;
    }
    else
    {
        body_start += 4;
    }

    // Calculate the size of the headers
    size_t headers_length = body_start - request;

    // Extract the headers
    char *headers = (char *)malloc(headers_length + 1);
    strncpy(headers, request, headers_length);
    headers[headers_length] = '\0';

    // Parse the headers
    const char *request_ptr = headers;
    parse_headers(&request_ptr, http_request);

    // Extract the request body
    size_t body_length = strlen(body_start);
    http_request->body = (char *)malloc(body_length + 1);
    strcpy(http_request->body, body_start);

    free(headers);
}