#include "json_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_json(const char *json);

static void skip_whitespace(const char **json)
{
    while (**json && (**json == ' ' || **json == '\t' || **json == '\n' || **json == '\r'))
    {
        (*json)++;
    }
}

static void parse_value(const char **json, JsonValue *value);

static void parse_object(const char **json, JsonValue *value)
{
    skip_whitespace(json);

    if (**json != '{')
    {
        fprintf(stderr, "Expected '{' at position %ld\n", *json - json);
        exit(EXIT_FAILURE);
    }

    (*json)++;
    skip_whitespace(json);

    // Create an object structure
    value->type = JSON_OBJECT;
    value->object.key = NULL;
    value->object.value = NULL;

    // Check if the object is empty
    if (**json == '}')
    {
        (*json)++;
        return;
    }

    // Parse object properties
    while (1)
    {
        // Parse property key
        skip_whitespace(json);
        if (**json != '"')
        {
            fprintf(stderr, "Expected '\"' at position %ld\n", *json - json);
            exit(EXIT_FAILURE);
        }

        // Skip the opening quote
        (*json)++;

        // Find the closing quote of the key
        const char *key_start = *json;
        while (**json && **json != '"')
        {
            (*json)++;
        }

        if (**json != '"')
        {
            fprintf(stderr, "Expected '\"' at position %ld\n", *json - json);
            exit(EXIT_FAILURE);
        }

        // Calculate the length of the key
        size_t key_length = *json - key_start;
        (*json)++; // Skip the closing quote

        // Allocate memory for the key and copy it
        value->object.key = (char *)malloc(key_length + 1);
        strncpy(value->object.key, key_start, key_length);
        value->object.key[key_length] = '\0';

        // Parse the property value
        skip_whitespace(json);
        if (**json != ':')
        {
            fprintf(stderr, "Expected ':' at position %ld\n", *json - json);
            exit(EXIT_FAILURE);
        }
        (*json)++;
        skip_whitespace(json);

        JsonValue *property_value = (JsonValue *)malloc(sizeof(JsonValue));
        parse_value(json, property_value);

        // Add the key-value pair to the object
        value->object.value = property_value;

        // Move to the next property or exit the loop
        skip_whitespace(json);
        if (**json == '}')
        {
            (*json)++;
            break;
        }

        if (**json != ',')
        {
            fprintf(stderr, "Expected ',' or '}' at position %ld\n", *json - json);
            exit(EXIT_FAILURE);
        }
        (*json)++;
    }
}

static void parse_array(const char **json, JsonValue *value)
{
    skip_whitespace(json);

    if (**json != '[')
    {
        fprintf(stderr, "Expected '[' at position %ld\n", *json - json);
        exit(EXIT_FAILURE);
    }

    (*json)++;
    skip_whitespace(json);

    // Create an array structure
    value->type = JSON_ARRAY;
    value->array = NULL;

    // Check if the array is empty
    if (**json == ']')
    {
        (*json)++;
        return;
    }

    // Parse array elements
    while (1)
    {
        // Parse the element value
        JsonValue *element_value = (JsonValue *)malloc(sizeof(JsonValue));
        parse_value(json, element_value);

        // Append the element value to the array
        value->array = realloc(value->array, sizeof(JsonValue *) * (size_t)(value->array + 1));
        value->array[value->array_size] = element_value;
        value->array_size++;

        // Move to the next element or exit the loop
        skip_whitespace(json);
        if (**json == ']')
        {
            (*json)++;
            break;
        }

        if (**json != ',')
        {
            fprintf(stderr, "Expected ',' or ']' at position %ld\n", *json - json);
            exit(EXIT_FAILURE);
        }
        (*json)++;
    }
}

static void parse_string(const char **json, JsonValue *value)
{
    skip_whitespace(json);

    if (**json != '"')
    {
        fprintf(stderr, "Expected '\"' at position %ld\n", *json - json);
        exit(EXIT_FAILURE);
    }

    // Skip the opening quote
    (*json)++;

    // Find the closing quote
    const char *string_start = *json;
    while (**json && **json != '"')
    {
        (*json)++;
    }

    if (**json != '"')
    {
        fprintf(stderr, "Expected '\"' at position %ld\n", *json - json);
        exit(EXIT_FAILURE);
    }

    // Calculate the length of the string
    size_t string_length = *json - string_start;
    (*json)++; // Skip the closing quote

    // Allocate memory for the string and copy it
    value->type = JSON_STRING;
    value->string = (char *)malloc(string_length + 1);
    strncpy(value->string, string_start, string_length);
    value->string[string_length] = '\0';
}

static void parse_number(const char **json, JsonValue *value)
{
    skip_whitespace(json);

    const char *number_start = *json;
    int is_float = 0;

    if (**json == '-')
    {
        (*json)++;
    }

    while (**json && (**json >= '0' && **json <= '9'))
    {
        (*json)++;
    }

    if (**json == '.')
    {
        is_float = 1;
        (*json)++;

        while (**json && (**json >= '0' && **json <= '9'))
        {
            (*json)++;
        }
    }

    if (is_float)
    {
        value->type = JSON_NUMBER;
        value->number = atof(number_start);
    }
    else
    {
        value->type = JSON_NUMBER;
        value->number = atoi(number_start);
    }
}

static void parse_literal(const char **json, JsonValue *value)
{
    skip_whitespace(json);

    if (strncmp(*json, "true", 4) == 0)
    {
        value->type = JSON_TRUE;
        *json += 4;
    }
    else if (strncmp(*json, "false", 5) == 0)
    {
        value->type = JSON_FALSE;
        *json += 5;
    }
    else if (strncmp(*json, "null", 4) == 0)
    {
        value->type = JSON_NULL;
        *json += 4;
    }
    else
    {
        fprintf(stderr, "Unexpected literal at position %ld\n", *json - json);
        exit(EXIT_FAILURE);
    }
}

static void parse_value(const char **json, JsonValue *value)
{
    skip_whitespace(json);

    switch (**json)
    {
    case '{':
        parse_object(json, value);
        break;
    case '[':
        parse_array(json, value);
        break;
    case '"':
        parse_string(json, value);
        break;
    case '-':
    case '0' ... '9':
        parse_number(json, value);
        break;
    case 't':
    case 'f':
    case 'n':
        parse_literal(json, value);
        break;
    default:
        fprintf(stderr, "Unexpected character at position %ld\n", *json - json);
        exit(EXIT_FAILURE);
    }
}

void parse_json(const char *json)
{
    JsonValue root;
    parse_value(&json, &root);

    // Access the parsed JSON data
    switch (root.type)
    {
    case JSON_OBJECT:
        // Access the object members using root.object.key and root.object.value
        break;
    case JSON_ARRAY:
        // Access the array elements using root.array[i]
        for (int i = 0; i < root.array_size; i++)
        {
            // Access each element using root.array[i]
        }
        break;
    case JSON_STRING:
        // Access the string value using root.string
        break;
    case JSON_NUMBER:
        // Access the numeric value using root.number
        break;
    case JSON_TRUE:
        // The value is true
        break;
    case JSON_FALSE:
        // The value is false
        break;
    case JSON_NULL:
        // The value is null
        break;
    }
}