#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "http_parser.h"

void handle_get_route(int client_sock, const char *request)
{
    // Parse the HTTP request
    HttpRequest http_request;
    parse_http_request(request, &http_request);
    printf("Request Headers:\n%s\n", http_request.headers);

    printf("Request body:\n%s\n", http_request.body);

    // Parse the JSON body
    // Assuming the body contains a valid JSON string
    // You can use your preferred JSON parsing library here
    const char *json_body = http_request.body;

    // Return the JSON body in the response
    const char *response = json_body;
    int response_length = strlen(response);

    char *http_response = (char *)malloc(MAX_REQUEST_SIZE);
    snprintf(http_response, MAX_REQUEST_SIZE, RESPONSE_TEMPLATE, response_length, response);

    // Make sure to include the necessary header file
    // #include <sys/socket.h>
    send(client_sock, http_response, strlen(http_response), 0);
    free(http_response);
}
int main()
{
    // Create a new server instance
    Server server;
    server_init(&server, 8080);

    // Create an IOC container
    IOCContainer *container = create_ioc_container();

    // Register custom routes
    add_route(container, "/users", POST, handle_get_route);
    // Start the server
    server_start(&server, container);

    // Cleanup the IOC container
    free(container->routes);
    free(container);

    return 0;
}
