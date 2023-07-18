#include "logger.h"
#include <stdio.h>
#include <time.h>

void logMessage(const char *method, const char *path)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm);

    printf("\r\033[0;31m[%s] Received request:\033[0m \033[0;32m%s\033[0m, \033[0;33m%s\033[0m\n", timestamp, method, path);
}

void logResponse(int http_status)
{
    const char *http_status_color = (http_status == HTTP_STATUS_OK) ? "\033[0;32m" : "\033[0;31m";

    printf("\r%sResponse Sent: %s%d%s\n", http_status_color, http_status_color, http_status, http_status_color);
}