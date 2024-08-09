#pragma once

#include <stdbool.h>

enum JSON_type {
  STRING, // for strings and object keys
  NUMBER,
  BOOLEAN,
  ARRAY,
  OBJECT,
  NULL_VALUE
};

typedef struct JSON {
  enum JSON_type type;

  struct JSON* next; // points to the next element (in array or object)
  struct JSON* previous; // points to the previous element (in array or object)
  struct JSON* child; // for object and array (points to first element), and for object keys (points to the value)

  char* string_value; // for string and object entry
  double number_value; // for number
  bool boolean_value; // for boolean
} JSON;

JSON* new_JSON();

char* skip_whitespace(char* source);
char* parse_string(JSON* output_json, char* source);
char* parse_boolean(JSON* output_json, char* source);
char* parse_null(JSON* output_json, char* source);
char* parse_number(JSON* output_json, char* source);
char* parse_value(JSON* output_json, char* source);
char* parse_object_entry(JSON* output_json, char* source);
char* parse_array(JSON* output_json, char* source);
char* parse_object(JSON* output_json, char* source);
char* parse_any(JSON* output_json, char* source);

bool parse_json(JSON* output_json, char* source);

void free_json(JSON* json);
