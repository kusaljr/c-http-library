#ifndef JSON_PARSER_H
#define JSON_PARSER_H

typedef enum {
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_TRUE,
    JSON_FALSE,
    JSON_NULL
} JsonType;

typedef struct {
    JsonType type;
    union {
        struct {
            char* key;
            void* value;
        } object;
        void** array;
        char* string;
        double number;
    };
    size_t array_size;
} JsonValue;

void parse_json(const char* json);

#endif  /* JSON_PARSER_H */
