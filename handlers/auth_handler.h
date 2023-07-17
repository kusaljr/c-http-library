#ifndef AUTHORIZATION_HANDLER_H
#define AUTHORIZATION_HANDLER_H

#include "../http/http_parser.h"

void handle_authorization(int client_sock, HttpRequest http_request);

#endif /* AUTHORIZATION_HANDLER_H */
