#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "http_parser.h"
#include "db/pg_connector.h"

PostgreSQLConnector connector;

void handle_leads_route(int client_sock, HttpRequest http_request)
{
    const char *query = "SELECT * FROM lead";
    char *response = execute_query(query, &connector);

    int response_length = strlen(response);

    char *http_response = (char *)malloc(MAX_REQUEST_SIZE);
    snprintf(http_response, MAX_REQUEST_SIZE, RESPONSE_TEMPLATE, response_length, response);

    // Make sure to include the necessary header file
    // #include <sys/socket.h>
    send(client_sock, http_response, strlen(http_response), 0);
    free(http_response);
}

void handle_users_middleware(int client_sock, HttpRequest http_request, void (*next)(int client_sock, HttpRequest http_request))
{
    // Perform middleware operations before calling the next function
    printf("Middleware: Handling users route, Header: %s\n", http_request.headers);

    // Call the next function in the middleware chain or the final handler
    next(client_sock, http_request);
}

int main()
{
    // Create a new server instance
    Server server;
    server_init(&server, 8080);

    const char *host = "localhost";
    const char *port = "5432";
    const char *dbname = "crms";
    const char *user = "postgres";
    const char *password = "learnifydbpwd";

    // Connect to PostgreSQL
    if (!pgConnect(&connector, host, port, dbname, user, password))
    {
        fprintf(stderr, "Failed to connect to PostgreSQL\n");
        return 0;
    }
    printf("Successfully connected to the PostgreSQL database\n");

    // Create an IOC container
    IOCContainer *container = create_ioc_container();

    // Register custom routes
    add_route(container, "/leads", GET, handle_users_middleware, handle_leads_route);

    // Start the server
    server_start(&server, container);

    // Cleanup the IOC container
    free(container->routes);
    free(container);

    return 0;
}
