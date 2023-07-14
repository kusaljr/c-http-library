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

        // Append the header line to the existing headers
        size_t headers_length = strlen(http_request->headers);
        size_t new_headers_length = headers_length + line_length + 2; // +2 for '\n' and '\0'
        http_request->headers = (char *)realloc(http_request->headers, new_headers_length);
        strcat(http_request->headers, header_line);
        strcat(http_request->headers, "\n");

        // Move to the next header line
        *request = end_of_line + 1;
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
    http_request->headers = malloc(1); // Allocate memory for an empty string
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
