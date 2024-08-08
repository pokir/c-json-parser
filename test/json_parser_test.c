#include "../src/json_parser.h"

#include <stdio.h>

int main() {
  // TODO: split tests

  JSON* json = new_JSON();

  if (parse_value(json, "\"hello\"", NO_CONTEXT) == NULL) {
    printf("JSON error");
    return 1;
  }

  printf("%u\n", json->type);
  printf("%s\n", json->string_value);

  free_json(json);

  // ---

  json = new_JSON();

  if (parse_value(json, "1.999999999.999991", NO_CONTEXT) == NULL) {
    printf("JSON error");
    return 1;
  }

  printf("%u\n", json->type);
  printf("%f\n", json->number_value);

  free_json(json);

  // ---

  json = new_JSON();

  if (parse_array(json, "[123, 456]") == NULL) {
    printf("JSON error");
    return 1;
  }

  printf("%u\n", json->type);
  printf("%f\n", json->child->number_value);

  free_json(json);

  // ---

  json = new_JSON();

  if (parse_object(json, "{\"one\": \"two\"}") == NULL) {
    printf("JSON error");
    return 1;
  }

  printf("%u\n", json->type);
  printf("%s\n", json->child->child->string_value);

  free_json(json);

  // ---

  json = new_JSON();

  if (parse_json(json, "\n\t{\n\"one\"\n\t\n : \n\"two\"\n\n}\n\n") == false) {
    printf("JSON error");
    return 1;
  }

  printf("%u\n", json->type);
  printf("%s\n", json->child->child->string_value);

  free_json(json);
}
