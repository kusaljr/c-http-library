#ifndef POSTGRESQLCONNECTOR_H
#define POSTGRESQLCONNECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#define MAX_RESPONSE_SIZE 4096

// Struct to hold the connection information
typedef struct
{
    PGconn *conn;
    PGresult *res;
} PostgreSQLConnector;

typedef struct
{
    int client_sock;
    PostgreSQLConnector *connector;
} RequestData;

// Function to connect to PostgreSQL
int pgConnect(PostgreSQLConnector *connector, const char *host, const char *port, const char *dbname,
              const char *user, const char *password);

// Function to disconnect from PostgreSQL
void pgDisconnect(PostgreSQLConnector *connector);

// Function to execute a query
PGresult *pgQuery(PostgreSQLConnector *connector, const char *query);

PGconn *pgGetConnection(PostgreSQLConnector *connector);
char *pgQueryResult(PostgreSQLConnector *connector, const char *query);
char *execute_query(const char *query, PostgreSQLConnector *connector);

#endif /* POSTGRESQLCONNECTOR_H */
