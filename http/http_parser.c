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
        else if (strncmp(header_line, "Host: ", 6) == 0)
        {
            // Handle Host header
            // Extract the value after the prefix
            char *host_value = header_line + 6;

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
    const char *query_start = strchr(request, '?');
    if (query_start != NULL)
    {
        query_start++; // Move past the '?' character

        // Find the end position of the query parameters
        const char *query_end = strchr(query_start, ' ');
        if (query_end != NULL)
        {
            size_t query_length = query_end - query_start;
            char query_string[256];
            strncpy(query_string, query_start, query_length);
            query_string[query_length] = '\0';

            // Replace '+' with space ' ' (URL decoding)
            for (size_t i = 0; i < query_length; i++)
            {
                if (query_string[i] == '+')
                {
                    query_string[i] = ' ';
                }
            }
            char *params = strdup(query_string); // Create a duplicate string to avoid modifying the original
            char *token;
            char *param;

            int param_count = 0;

            token = strtok(params, "&");
            while (token != NULL && param_count < MAX_NUM_PARAMETERS)
            {
                param = strchr(token, '=');
                if (param != NULL)
                {
                    *param = '\0'; // Null-terminate the parameter name
                    param++;       // Move to the start of the parameter value

                    // Copy parameter name and value to the request structure
                    strncpy(http_request->query[param_count].key, token, sizeof(http_request->query[param_count].key) - 1);
                    strncpy(http_request->query[param_count].value, param, sizeof(http_request->query[param_count].value) - 1);

                    param_count++;
                }

                token = strtok(NULL, "&");
            }

            http_request->num_query = param_count;

            free(params);
        }
    }
}
