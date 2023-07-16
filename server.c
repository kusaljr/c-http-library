#include "http_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server.h"

void handle_request(int client_sock, char *request, IOCContainer *container)
{
    // Determine the HTTP method
    char *request_copy = strdup(request);
    char *method = strtok(request_copy, " ");

    // Determine the URL path with query parameters
    char *path_with_query = strtok(NULL, " ");
    char *path = strtok(path_with_query, "?");

    if (path == NULL)
    {
        // Invalid request, no URL path
        free(request_copy);
        const char *response = "<html><body><h1>Invalid Request</h1></body></html>";
        int response_length = strlen(response);

        char *http_response = (char *)malloc(MAX_REQUEST_SIZE);
        snprintf(http_response, MAX_REQUEST_SIZE, RESPONSE_TEMPLATE, response_length, response);

        send(client_sock, http_response, strlen(http_response), 0);
        free(http_response);
        return;
    }

    // Find a matching route
    int i;
    int route_found = 0;
    for (i = 0; i < container->num_routes; i++)
    {
        if (strcmp(path, container->routes[i].path) == 0)
        {
            HttpMethod method_type = container->routes[i].method;

            // Check if the method matches
            if ((strcmp(method, "GET") == 0 && method_type == GET) ||
                (strcmp(method, "POST") == 0 && method_type == POST) ||
                (strcmp(method, "PUT") == 0 && method_type == PUT) ||
                (strcmp(method, "PATCH") == 0 && method_type == PATCH) ||
                (strcmp(method, "DELETE") == 0 && method_type == DELETE))
            {
                // Execute middleware if provided
                if (container->routes[i].middleware != NULL)
                {
                    HttpRequest *http_request = (HttpRequest *)malloc(sizeof(HttpRequest));
                    parse_http_request(request, http_request);
                    container->routes[i].middleware(client_sock, *http_request, container->routes[i].handler);
                    free(http_request);
                }
                else
                {
                    // No middleware, directly call the handler
                    HttpRequest *http_request = (HttpRequest *)malloc(sizeof(HttpRequest));
                    parse_http_request(request, http_request);
                    container->routes[i].handler(client_sock, *http_request);
                    free(http_request);
                }

                route_found = 1;
                break;
            }
        }
    }

    if (!route_found)
    {
        // Route not found
        const char *response = "<html><body><h1>Route Not Found</h1></body></html>";
        int response_length = strlen(response);

        char *http_response = (char *)malloc(MAX_REQUEST_SIZE);
        snprintf(http_response, MAX_REQUEST_SIZE, RESPONSE_TEMPLATE, response_length, response);

        send(client_sock, http_response, strlen(http_response), 0);
        free(http_response);
    }

    free(request_copy);
}

void server_init(Server *server, int port)
{
    server->socket_fd = -1;
    server->port = port;
}

void server_start(Server *server, IOCContainer *container)
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create a socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server->port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_sock, 10) == -1)
    {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Listening on port %d...\n", server->port);

    while (1)
    {
        // Accept a client connection
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock == -1)
        {
            perror("Failed to accept client connection");
            exit(EXIT_FAILURE);
        }

        // Read the request from the client
        char request[MAX_REQUEST_SIZE];
        ssize_t request_size = recv(client_sock, request, MAX_REQUEST_SIZE - 1, 0);
        if (request_size == -1)
        {
            perror("Failed to read request from client");
            exit(EXIT_FAILURE);
        }

        // Null-terminate the request string
        request[request_size] = '\0';

        // Handle the request and send the response
        handle_request(client_sock, request, container);

        // Close the client connection
        close(client_sock);
    }

    // Close the server socket
    close(server_sock);
}

void add_route(IOCContainer *container, const char *path, HttpMethod method, void (*middleware)(int client_sock, HttpRequest http_request, void (*next)(int client_sock, HttpRequest http_request)), void (*handler)(int client_sock, HttpRequest http_request))
{
    if (container->num_routes >= 10)
    {
        printf("Route table full. Cannot add more routes.\n");
        return;
    }

    container->routes[container->num_routes].path = path;
    container->routes[container->num_routes].method = method;

    if (middleware != NULL)
    {
        container->routes[container->num_routes].middleware = middleware;
        container->routes[container->num_routes].handler = handler;
    }
    else
    {
        container->routes[container->num_routes].middleware = NULL;
        container->routes[container->num_routes].handler = handler;
    }

    container->num_routes++;
}

IOCContainer *create_ioc_container()
{
    IOCContainer *container = (IOCContainer *)malloc(sizeof(IOCContainer));
    container->routes = (Route *)malloc(sizeof(Route) * 10); // Maximum 10 routes, adjust as needed
    container->num_routes = 0;

    return container;
}