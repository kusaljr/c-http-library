#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

// Define custom route handlers
void handle_get_route(int client_sock, const char *request)
{
    const char *response = "<html><body><h1>POST Route BABAY</h1><p>Welcome to the GET route!</p></body></html>";
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
