# REST API Framework in C

## Installation

Install Postgres connector

```bash
sudo apt-get install libpq-dev
```

Compile Project

```bash
make myprogram
```

Run Project

```bash
./myprogram
```

Now you can make HTTP Request

```json
GET http://localhost:8080/hello

{
    message: "Hello World"
}
```

## Documentation

````

### Types

Http Method types

```typescript
typedef enum
{
    GET,
    POST,
    PUT,
    PATCH,
    DELETE
} HttpMethod;
````

Add Route

```typescript
add_route(
  IOContainer,
  { route_name },
  HTTP_METHOD,
  MIDDLEWARE_FUNCTION,
  ROUTE_FUNCTION
);
```

HTTP Request

```typescript
typedef struct
{
    HttpHeaders headers;
    int num_headers;
    int num_params;
    char *body;

    struct
    {
        char key[256];
        char value[256];
    } params[MAX_NUM_PARAMETERS];
} HttpRequest;
```

## Examples

### Simple Route

```c
// Create a global container to work with
IOCContainer *container = create_ioc_container();

// Register routes
add_route(container, "/hello", GET, NULL, handle_hello_middleware);

// Start the server
server_start(&server, container);
```

Middleware and Route function

```typescript
void handle_hello_route(int client_sock, HttpRequest http_request)
{
    const char *response = "{\"message\": \"Hello World\"}";
    int response_length = strlen(response);

    char *http_response = (char *)malloc(MAX_REQUEST_SIZE);
    snprintf(http_response, MAX_REQUEST_SIZE, RESPONSE_TEMPLATE_JSON_OK, response_length, response);

    send(client_sock, http_response, strlen(http_response), 0);
    free(http_response);
}

void handle_hello_middleware(int client_sock, HttpRequest http_request, void (*next)(int client_sock, HttpRequest http_request))
{

    // Perform middleware operations before calling the next function
    printf("Middleware: Handling users route\n");

    // Call the next function in the middleware chain or the final handler
    next(client_sock, http_request);
}
```

---

You can pass `NULL` in place of middleware function too

## Making Database Call

First of all Configure database credential in main function

```c
const char *host = "localhost";
const char *port = "5432";
const char *dbname = "database_name";
const char *user = "postgres";
const char *password = "database_password";
```

```typescript
void handle_database_route(int client_sock, HttpRequest http_request)
{
    const char *query = "SELECT * FROM lead";
    char *response = execute_query(query, &connector);

    int response_length = strlen(response);

    char *http_response = (char *)malloc(MAX_REQUEST_SIZE);
    snprintf(http_response, MAX_REQUEST_SIZE, RESPONSE_TEMPLATE, response_length, response);

    send(client_sock, http_response, strlen(http_response), 0);
    free(http_response);
}

```
