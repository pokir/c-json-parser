#include "../src/json_parser.h"

#include <stdio.h>
#include <string.h>

bool string_test() {
  JSON* json = new_JSON();

  bool pass = parse_json(json, "\"hello\"")
    && json->type == STRING
    && strcmp(json->string_value, "hello") == 0
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

  bool pass = parse_json(json, "1.935")
    && json->type == NUMBER
    && json->string_value == NULL
    && json->number_value == 1.935
    && json->boolean_value == false
    && json->previous == NULL
    && json->next == NULL
    && json->child == NULL;

  free_json(json);

  return pass;
}

bool array_test() {
  JSON* json = new_JSON();

  bool pass = parse_json(json, "[123, 456, \"three\", null, true, false]")

    && json->type == ARRAY
    && json->string_value == NULL
    && json->number_value == 0
    && json->boolean_value == false
    && json->previous == NULL
    && json->next == NULL

    && json->child != NULL
    && json->child->type == NUMBER
    && json->child->string_value == NULL
    && json->child->number_value == 123
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
    && strcmp(json->child->next->next->string_value, "three") == 0
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

  bool pass = parse_json(json, "{\"one\": \"two\"}")

    && json->type == OBJECT
    && json->string_value == NULL
    && json->number_value == 0
    && json->boolean_value == false
    && json->previous == NULL
    && json->next == NULL

    && json->child != NULL
    && json->child->type == OBJECT_KEY
    && strcmp(json->child->string_value, "one") == 0
    && json->child->number_value == 0
    && json->child->boolean_value == false
    && json->child->previous == NULL
    && json->child->next == NULL

    && json->child->child != NULL
    && json->child->child->type == STRING
    && strcmp(json->child->child->string_value, "two") == 0
    && json->child->child->number_value == 0
    && json->child->child->boolean_value == false
    && json->child->child->previous == NULL
    && json->child->child->next == NULL
    && json->child->child->child == NULL;

  free_json(json);

  return pass;
}

bool whitespace_test() {
  JSON* json = new_JSON();

  bool pass = parse_json(json, "\n\t{\n  \"one\"\n \t\n : \r\n\"two\" \n\r\n}\n\t \n")

    && json->type == OBJECT
    && json->string_value == NULL
    && json->number_value == 0
    && json->boolean_value == false
    && json->previous == NULL
    && json->next == NULL

    && json->child != NULL
    && json->child->type == OBJECT_KEY
    && strcmp(json->child->string_value, "one") == 0
    && json->child->number_value == 0
    && json->child->boolean_value == false
    && json->child->previous == NULL
    && json->child->next == NULL

    && json->child->child != NULL
    && json->child->child->type == STRING
    && strcmp(json->child->child->string_value, "two") == 0
    && json->child->child->number_value == 0
    && json->child->child->boolean_value == false
    && json->child->child->previous == NULL
    && json->child->child->next == NULL
    && json->child->child->child == NULL;

  free_json(json);

  return pass;

}

int main() {
  if (!string_test()) printf("String test failed");
  if (!number_test()) printf("Number test failed");
  if (!array_test()) printf("Array test failed");
  if (!object_test()) printf("Object test failed");
  if (!whitespace_test()) printf("Object test failed");
}
