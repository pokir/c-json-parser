#include "../src/json_parser.h"

#include <stdio.h>

bool parse_utf8_character_test() {
  // 0xxx xxxx (ASCII characters)
  for (uint8_t i = 1; i <= 0b1111111; ++i) {
    uint8_t source[] = " ";
    source[0] = i;

    uint32_t code_point;
    uint8_t* new_source = parse_utf8_character(&code_point, source);

    bool pass = new_source != NULL
      && *new_source == '\0'
      && code_point == i;

    if (!pass) return false;
  }

  // all characters (including ASCII characters)
  for (uint32_t i = 1; i <= 0b111111111111111111111; ++i) {
    int num_bytes = utf8_num_bytes(i);

    uint8_t source[num_bytes + 1];
    code_point_to_utf8(source, NULL, i);
    source[num_bytes] = '\0';

    uint32_t code_point;
    uint8_t* new_source = parse_utf8_character(&code_point, source);

    bool pass = new_source != NULL
      && *new_source == '\0'
      && code_point == i;

    if (!pass) return false;
  }

  return true;
}

bool parse_unicode_escape_sequence_test() {
  {
    uint8_t* source = (uint8_t*)"\\uCCC4";

    uint32_t code_point;
    uint8_t* new_source = parse_unicode_escape_sequence(&code_point, source);

    if (!(
          new_source != NULL
          && *new_source == '\0'
          && code_point == 0xCCC4
         )) return false;
  }

  {
    uint8_t* source = (uint8_t*)"\\uD801\\uDC37";

    uint32_t code_point;
    uint8_t* new_source = parse_unicode_escape_sequence(&code_point, source);

    if (!(
          new_source != NULL
          && *new_source == '\0'
          && code_point == 0x10437
         )) return false;
  }

  return true;
}

int main() {
  if (!parse_utf8_character_test()) printf("parse_utf8_character test failed\n");
  if (!parse_unicode_escape_sequence_test()) printf("parse_unicode_escape_sequence test failed\n");
}
