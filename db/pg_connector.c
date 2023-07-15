#include "pg_connector.h"
#include <string.h>

int pgConnect(PostgreSQLConnector *connector, const char *host, const char *port, const char *dbname,
              const char *user, const char *password)
{
    // Build the connection string
    char conninfo[256];
    snprintf(conninfo, sizeof(conninfo), "host=%s port=%s dbname=%s user=%s password=%s", host, port, dbname, user, password);

    // Establish the connection
    connector->conn = PQconnectdb(conninfo);

    // Check if the connection succeeded
    if (PQstatus(connector->conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(connector->conn));
        return 0;
    }

    return 1;
}

PGconn *pgGetConnection(PostgreSQLConnector *connector)
{
    return connector->conn;
}

void pgDisconnect(PostgreSQLConnector *connector)
{
    // Clear the result if there is one
    if (connector->res != NULL)
    {
        PQclear(connector->res);
        connector->res = NULL;
    }

    // Close the connection
    PQfinish(connector->conn);
    connector->conn = NULL;
}

PGresult *pgQuery(PostgreSQLConnector *connector, const char *query)
{
    // Clear any previous result
    if (connector->res != NULL)
    {
        PQclear(connector->res);
    }

    // Execute the query
    connector->res = PQexec(connector->conn, query);

    // Check if the query execution succeeded
    if (PQresultStatus(connector->res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Query execution failed: %s", PQerrorMessage(connector->conn));
        return NULL;
    }

    return connector->res;
}

// Function to format a string as JSON key-value pair
char *formatKeyValue(const char *key, const char *value)
{
    size_t keyLength = strlen(key);
    size_t valueLength = strlen(value);

    // Allocate memory for the formatted JSON key-value pair
    size_t formattedSize = keyLength + valueLength + 8; // 8 for quotes, colon, space, and null terminator
    char *formattedPair = (char *)malloc(formattedSize);

    if (value[0] == '\"' && value[valueLength - 1] == '\"')
    {
        // If value is already inside double quotation marks, remove them
        snprintf(formattedPair, formattedSize, "\"%s\": %s", key, value);
    }
    else
    {
        // Value is not inside double quotation marks, keep it intact
        snprintf(formattedPair, formattedSize, "\"%s\": \"%s\"", key, value);
    }

    return formattedPair;
}
char *execute_query(const char *query, PostgreSQLConnector *connector)
{
    // Get the PostgreSQL connection
    PGconn *connection = pgGetConnection(connector);

    // Execute the query
    PGresult *result = PQexec(connection, query);

    if (PQresultStatus(result) == PGRES_TUPLES_OK)
    {
        // Process the query result
        int rowCount = PQntuples(result);
        int colCount = PQnfields(result);

        // Allocate memory for the JSON response
        size_t responseSize = (rowCount * colCount * 100) + 2; // Estimate the response size
        char *jsonResponse = (char *)malloc(responseSize);
        jsonResponse[0] = '\0'; // Initialize the response as an empty string

        strcat(jsonResponse, "[");

        // Iterate over each row
        for (int row = 0; row < rowCount; row++)
        {
            if (row > 0)
            {
                strcat(jsonResponse, ",");
            }

            strcat(jsonResponse, "{");

            // Iterate over each column
            for (int col = 0; col < colCount; col++)
            {
                const char *columnName = PQfname(result, col);
                const char *columnValue = PQgetvalue(result, row, col);

                char *formattedPair = formatKeyValue(columnName, columnValue);

                strcat(jsonResponse, formattedPair);
                free(formattedPair);

                if (col < colCount - 1)
                {
                    strcat(jsonResponse, ",");
                }
            }

            strcat(jsonResponse, "}");
        }

        strcat(jsonResponse, "]");

        // Free the result
        PQclear(result);

        // Close the database connection
        // PQfinish(connection);

        return jsonResponse;
    }
    else
    {
        fprintf(stderr, "Failed to execute query: %s\n", PQerrorMessage(connection));

        // Free the result
        PQclear(result);

        // Close the database connection
        // PQfinish(connection);

        // Return an error response
        const char *error_response = "{\"error\":\"Failed to execute query\"}";
        char *response = (char *)malloc(strlen(error_response) + 1);
        strcpy(response, error_response);

        return response;
    }
}