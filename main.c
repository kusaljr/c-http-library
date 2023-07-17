#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server/server.h"
#include "http/http_parser.h"
#include "db/pg_connector.h"
#include <unistd.h>
#include "handlers/auth_handler.h"
#include "jwt/jwt_helper.h"
#include "handlers/env_parser.h"
PostgreSQLConnector connector;
const char *secret_key = "ASd12d12@9c03j#(*H(C";
void handle_hello_route(int client_sock, HttpRequest http_request)
{
    const char *response = "{\"message\": \"Hello World\"}";
    int response_length = strlen(response);

    char *http_response = (char *)malloc(MAX_REQUEST_SIZE);
    snprintf(http_response, MAX_REQUEST_SIZE, RESPONSE_TEMPLATE_JSON_OK, response_length, response);

    send(client_sock, http_response, strlen(http_response), 0);
    free(http_response);
}

void handle_users_middleware(int client_sock, HttpRequest http_request, void (*next)(int client_sock, HttpRequest http_request))
{

    // Perform middleware operations before calling the next function
    printf("Middleware: Handling users route\n");
    // for (int i = 0; i < http_request.num_query; i++)
    // {
    //     printf("%s: %s\n", http_request.query[i].key, http_request.query[i].value);
    // }

    handle_authorization(client_sock, http_request);

    // Call the next function in the middleware chain or the final handler
    next(client_sock, http_request);
}

void handle_auth_route(int client_sock, HttpRequest http_request)
{
    const char *payload = "{\"sub\":\"1234567890\",\"name\":\"John Doe\"}";

    char *jwt_token = create_jwt(payload, secret_key);

    // Construct the response JSON string
    const char *response_template = "{\"token\": \"%s\"}";
    int response_length = strlen(response_template) + strlen(jwt_token) - 2;
    char *response = (char *)malloc(response_length + 1);
    snprintf(response, response_length + 1, response_template, jwt_token);

    // Construct the HTTP response
    char *http_response = (char *)malloc(MAX_REQUEST_SIZE);
    snprintf(http_response, MAX_REQUEST_SIZE, RESPONSE_TEMPLATE_JSON_OK, response_length, response);

    // Send the response to the client
    send(client_sock, http_response, strlen(http_response), 0);

    // Clean up resources
    free(http_response);
}

int main()
{
    char *port = get_env_value(".env", "PORT");
    if (port == NULL)
    {
        printf("API_KEY not found in .env file\n");
        return 0;
    }

    // Create a new server instance
    Server server;
    server_init(&server, atoi(port));

    const char *host = "localhost";
    const char *db_port = "5432";
    const char *dbname = "crms";
    const char *user = "postgres";
    const char *password = "learnifydbpwd";

    // Connect to PostgreSQL
    if (!pgConnect(&connector, host, db_port, dbname, user, password))
    {
        fprintf(stderr, "Failed to connect to PostgreSQL\n");
        return 0;
    }
    printf("Successfully connected to the PostgreSQL database\n");

    // Create an IOC container
    IOCContainer *container = create_ioc_container();

    // Register custom routes
    add_route(container, "/hello", GET, handle_users_middleware, handle_hello_route);
    add_route(container, "/auth", GET, NULL, handle_auth_route);

    // Start the server
    server_start(&server, container);

    // Cleanup the IOC container
    free(container->routes);
    free(container);
    free(port);

    return 0;
}
