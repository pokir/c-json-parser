#include "../src/json_parser.h"

#include <stdio.h>
#include <string.h>

bool string_test() {
  JSON* json = new_JSON();

  bool pass = parse_json(json, (uint8_t*)"\"hello\\tworl\\b\\f\\n\\rd\\u0050\"")
    && json->type == STRING
    && strcmp((char*)get_JSON_string(json), "hello\tworl\b\f\n\rdP") == 0
    && json->number_value == 0
    && json->boolean_value == false
    && json->previous == NULL
    && json->next == NULL
    && json->child == NULL;

  free_json(json);

  return pass;
}

bool number_test() {
  JSON* json = new_JSON();

  bool pass = parse_json(json, (uint8_t*)"-1.935e-2")
    && json->type == NUMBER
    && json->string_value == NULL
    && json->number_value == -0.01935
    && json->boolean_value == false
    && json->previous == NULL
    && json->next == NULL
    && json->child == NULL;

  free_json(json);

  return pass;
}

bool array_test() {
  JSON* json = new_JSON();

  bool pass = parse_json(json, (uint8_t*)"[-123, 456, \"three\", null, true, false]")

    && json->type == ARRAY
    && json->string_value == NULL
    && json->number_value == 0
    && json->boolean_value == false
    && json->previous == NULL
    && json->next == NULL

    && json->child != NULL
    && json->child->type == NUMBER
    && json->child->string_value == NULL
    && json->child->number_value == -123
    && json->child->boolean_value == false
    && json->child->previous == NULL
    && json->child->child == NULL

    && json->child->next != NULL
    && json->child->next->type == NUMBER
    && json->child->next->string_value == NULL
    && json->child->next->number_value == 456
    && json->child->next->boolean_value == false
    && json->child->next->previous == json->child
    && json->child->next->child == NULL

    && json->child->next->next != NULL
    && json->child->next->next->type == STRING
    && strcmp((char*)get_JSON_string(json->child->next->next), "three") == 0
    && json->child->next->next->number_value == 0
    && json->child->next->next->boolean_value == false
    && json->child->next->next->previous == json->child->next
    && json->child->next->next->child == NULL

    && json->child->next->next->next != NULL
    && json->child->next->next->next->type == NULL_VALUE
    && json->child->next->next->next->string_value == NULL
    && json->child->next->next->next->number_value == 0
    && json->child->next->next->next->boolean_value == false
    && json->child->next->next->next->previous == json->child->next->next
    && json->child->next->next->next->child == NULL

    && json->child->next->next->next->next != NULL
    && json->child->next->next->next->next->type == BOOLEAN
    && json->child->next->next->next->next->string_value == NULL
    && json->child->next->next->next->next->number_value == 0
    && json->child->next->next->next->next->boolean_value == true
    && json->child->next->next->next->next->previous == json->child->next->next->next
    && json->child->next->next->next->next->child == NULL

    && json->child->next->next->next->next->next != NULL
    && json->child->next->next->next->next->next->type == BOOLEAN
    && json->child->next->next->next->next->next->string_value == NULL
    && json->child->next->next->next->next->next->number_value == 0
    && json->child->next->next->next->next->next->boolean_value == false
    && json->child->next->next->next->next->next->previous == json->child->next->next->next->next
    && json->child->next->next->next->next->next->child == NULL;

  free_json(json);

  return pass;
}

bool object_test() {
  JSON* json = new_JSON();

  bool pass = parse_json(json, (uint8_t*)"{\"one\": \"two\", \"three\": true}")

    && json->type == OBJECT
    && json->string_value == NULL
    && json->number_value == 0
    && json->boolean_value == false
    && json->previous == NULL
    && json->next == NULL

    && json->child != NULL
    && json->child->type == STRING
    && strcmp((char*)get_JSON_string(json->child), "one") == 0
    && json->child->number_value == 0
    && json->child->boolean_value == false
    && json->child->previous == NULL

    && json->child->child != NULL
    && json->child->child->type == STRING
    && strcmp((char*)get_JSON_string(json->child->child), "two") == 0
    && json->child->child->number_value == 0
    && json->child->child->boolean_value == false
    && json->child->child->previous == NULL
    && json->child->child->next == NULL
    && json->child->child->child == NULL

    && json->child->next != NULL
    && json->child->next->type == STRING
    && strcmp((char*)get_JSON_string(json->child->next), "three") == 0
    && json->child->next->number_value == 0
    && json->child->next->boolean_value == false
    && json->child->next->previous == json->child
    && json->child->next->next == NULL

    && json->child->next->child != NULL
    && json->child->next->child->type == BOOLEAN
    && json->child->next->child->string_value == NULL
    && json->child->next->child->number_value == 0
    && json->child->next->child->boolean_value == true
    && json->child->next->child->previous == NULL
    && json->child->next->child->next == NULL
    && json->child->next->child->child == NULL;

  free_json(json);

  return pass;
}

bool whitespace_test() {
  JSON* json = new_JSON();

  bool pass = parse_json(json, (uint8_t*)"\n\t{\n  \"one\"\n \t\n : \r\n\"two\" \n\r\n, \r\t\t\t\t\"three\"\n\r   : true \t}\n\t \n")

    && json->type == OBJECT
    && json->string_value == NULL
    && json->number_value == 0
    && json->boolean_value == false
    && json->previous == NULL
    && json->next == NULL

    && json->child != NULL
    && json->child->type == STRING
    && strcmp((char*)get_JSON_string(json->child), "one") == 0
    && json->child->number_value == 0
    && json->child->boolean_value == false
    && json->child->previous == NULL

    && json->child->child != NULL
    && json->child->child->type == STRING
    && strcmp((char*)get_JSON_string(json->child->child), "two") == 0
    && json->child->child->number_value == 0
    && json->child->child->boolean_value == false
    && json->child->child->previous == NULL
    && json->child->child->next == NULL
    && json->child->child->child == NULL

    && json->child->next != NULL
    && json->child->next->type == STRING
    && strcmp((char*)get_JSON_string(json->child->next), "three") == 0
    && json->child->next->number_value == 0
    && json->child->next->boolean_value == false
    && json->child->next->previous == json->child
    && json->child->next->next == NULL

    && json->child->next->child != NULL
    && json->child->next->child->type == BOOLEAN
    && json->child->next->child->string_value == NULL
    && json->child->next->child->number_value == 0
    && json->child->next->child->boolean_value == true
    && json->child->next->child->previous == NULL
    && json->child->next->child->next == NULL
    && json->child->next->child->child == NULL;

  free_json(json);

  return pass;
}

int main() {
  if (!string_test()) printf("String test failed\n");
  if (!number_test()) printf("Number test failed\n");
  if (!array_test()) printf("Array test failed\n");
  if (!object_test()) printf("Object test failed\n");
  if (!whitespace_test()) printf("White space test failed\n");
}
