#ifndef ENV_PARSER_H
#define ENV_PARSER_H

typedef struct
{
    char *key;
    char *value;
} KeyValuePair;

typedef struct
{
    KeyValuePair *pairs;
    int num_entries;
} EnvData;

char *get_env_value(const char *filename, const char *targetKey);

#endif
