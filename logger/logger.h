#ifndef LOGGER_H
#define LOGGER_H
#include "../server/server.h"

void logMessage(const char *method, const char *path);
void logResponse(int http_status);

#endif /* LOGGER_H */
