#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // Include the necessary header for 'send'

#define MAX_REQUEST_SIZE 2048
#define RESPONSE_TEMPLATE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s"

typedef enum
{
    GET,
    POST,
    PUT,
    PATCH,
    DELETE
} HttpMethod;

typedef struct
{
    int socket_fd;
    int port;
} Server;

typedef struct
{
    const char *path;
    HttpMethod method;
    void (*handler)(int client_sock, const char *request);
} Route;

typedef struct
{
    Route *routes;
    int num_routes;
} IOCContainer;

void server_init(Server *server, int port);
void server_start(Server *server, IOCContainer *container);
void handle_request(int client_sock, char *request, IOCContainer *container);
void add_route(IOCContainer *container, const char *path, HttpMethod method, void (*handler)(int client_sock, const char *request));
IOCContainer *create_ioc_container(); // Add the function declaration for 'create_ioc_container'

#endif
