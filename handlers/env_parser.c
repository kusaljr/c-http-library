// env_parser.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env_parser.h"

char *get_env_value(const char *filename, const char *targetKey)
{
    // Open the .env file
    FILE *envFile = fopen(filename, "r");
    if (envFile == NULL)
    {
        printf("Error opening .env file.\n");
        return NULL;
    }

    // Parse the .env file line by line
    char line[100];
    while (fgets(line, sizeof(line), envFile))
    {
        // Split the line into key and value
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        // Check if the key matches the target key
        if (strcmp(key, targetKey) == 0)
        {
            // Close the .env file
            fclose(envFile);
            return strdup(value);
        }
    }

    // Close the .env file
    fclose(envFile);

    // Return NULL if the target key was not found
    return NULL;
}
