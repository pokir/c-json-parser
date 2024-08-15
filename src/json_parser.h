#pragma once

#include <stdbool.h>
#include <stdlib.h>

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

  uint32_t* string_value; // for string and object entry (array of code points)
  double number_value; // for number
  bool boolean_value; // for boolean
} JSON;

JSON* new_JSON();

uint8_t* skip_whitespace(uint8_t* source);
uint8_t* parse_utf8_character(uint32_t* output_code_point, uint8_t* source);
uint8_t* parse_unicode_escape_sequence(uint32_t* parsed_code_point, uint8_t* source);
uint8_t* parse_string_character(uint32_t* parsed_code_point, uint8_t* source);
uint8_t* parse_string(JSON* output_json, uint8_t* source);
uint8_t* parse_boolean(JSON* output_json, uint8_t* source);
uint8_t* parse_null(JSON* output_json, uint8_t* source);
uint8_t* parse_number(JSON* output_json, uint8_t* source);
uint8_t* parse_value(JSON* output_json, uint8_t* source);
uint8_t* parse_object_entry(JSON* output_json, uint8_t* source);
uint8_t* parse_array(JSON* output_json, uint8_t* source);
uint8_t* parse_object(JSON* output_json, uint8_t* source);
uint8_t* parse_any(JSON* output_json, uint8_t* source);

bool parse_json(JSON* output_json, uint8_t* source);

void free_json(JSON* json);

int utf8_num_bytes(uint32_t code_point);
void code_point_to_utf8(uint8_t* output, int* output_num_bytes, uint32_t code_point);

uint8_t* get_JSON_string(JSON* json);
