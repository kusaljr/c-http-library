#include <unistd.h>
#include <string.h>
#include "auth_handler.h"

void handle_authorization(int client_sock, HttpRequest http_request)
{
    // Check if authorization header is present
    if (http_request.headers.authorization == NULL)
    {
        // Return a response with "Forbidden" message
        const char *response = "HTTP/1.1 403 Forbidden\r\n"
                               "Content-Length: 9\r\n"
                               "Content-Type: text/plain\r\n"
                               "\r\n"
                               "Forbidden";
        write(client_sock, response, strlen(response));
        close(client_sock);
        return;
    }

    // Authorization header is present, continue with next middleware or final handler
    // ...
    // Your code here
}
