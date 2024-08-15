#include "json_parser.h"

#include <string.h>

// allocates memory (initialized to zero) for a new JSON
JSON* new_JSON() {
  return calloc(1, sizeof(JSON));
}

// returns the rest of the source
uint8_t* skip_whitespace(uint8_t* source) {
  while (*source == ' ' || *source == '\t' || *source == '\n' || *source == '\r')
    ++source;

  return source;
}

// returns the rest of the source, or NULL if there was an error
// puts the parsed character into output_code_point
uint8_t* parse_utf8_character(uint32_t* output_code_point, uint8_t* source) {
  if (!*source) return NULL; // empty string

  int num_bytes = 1;
  int first_byte_num_high_order_bits = 1;

  if (source[0] >> 7 != 0) {
    if (source[0] >> 4 == 0b1111) num_bytes = 4;
    else if (source[0] >> 5 == 0b111) num_bytes = 3;
    else if (source[0] >> 6 == 0b11) num_bytes = 2;

    first_byte_num_high_order_bits = num_bytes + 1;
  }

  *output_code_point = 0;

  for (int i = 0; i < num_bytes; ++i) {
    if (!*source) return NULL; // not enough bytes to read from
    int num_high_order_bits = i == 0 ? first_byte_num_high_order_bits : 2;
    *output_code_point <<= 8 - num_high_order_bits;
    *output_code_point |= ((*source << num_high_order_bits) & 0b11111111) >> num_high_order_bits;
    ++source;
  }

  return source;
}

// returns the rest of the source, or NULL if there was an error
// puts the parsed character into parsed_character
uint8_t* parse_string_character(uint32_t* parsed_code_point, uint8_t* source) {
  if (*source == '\\') {
    ++source;

    if (*source == '\n' || *source == '\0') return NULL;

    if (*source == 'u') {
      ++source;
      if (strlen(source) < 4) return NULL;

      int code_point = 0;

      for (int i = 0; i < 4; ++i) {
        code_point *= 16;

        if (source[i] >= '0' && source[i] <= '9') code_point += source[i] - '0';
        else if (source[i] >= 'a' && source[i] <= 'f') code_point += source[i] - 'a' + 10;
        else if (source[i] >= 'A' && source[i] <= 'F') code_point += source[i] - 'A' + 10;
        else return NULL;
      }

      *parsed_code_point = code_point;

      // TODO: handle consecutive \u

      source += 4;
    } else {
      if (*source == '"') *parsed_code_point = '"';
      else if (*source == '\\') *parsed_code_point = '\\';
      else if (*source == '/') *parsed_code_point = '/';
      else if (*source == 'b') *parsed_code_point = '\b';
      else if (*source == 'f') *parsed_code_point = '\f';
      else if (*source == 'n') *parsed_code_point = '\n';
      else if (*source == 'r') *parsed_code_point = '\r';
      else if (*source == 't') *parsed_code_point = '\t';
      else return NULL;

      ++source;
    }
  } else {
    source = parse_utf8_character(parsed_code_point, source);
  }

  return source;
}

// returns the rest of the source, or NULL if there was an error
uint8_t* parse_string(JSON* output_json, uint8_t* source) {
  if (*source != '"') return NULL;
  ++source;

  // buffer with worst case size
  uint32_t code_points[strlen(source)];
  int code_points_length = 0;

  while (true) {
    if (*source == '\n' || *source == '\0') {
      return NULL;
    } else if (*source == '"') {
      // end the string
      output_json->type = STRING;

      output_json->string_value = malloc((code_points_length + 1) * sizeof(uint32_t));
      memcpy(output_json->string_value, code_points, code_points_length * sizeof(uint32_t));
      output_json->string_value[code_points_length] = '\0';

      ++source;
      break;
    } else {
      // parse the character into the string buffer
      source = parse_string_character(code_points + code_points_length, source);
      ++code_points_length;

      if (source == NULL) return NULL;
    }
  }

  return source;
}

// returns the rest of the source, or NULL if there was an error
uint8_t* parse_boolean(JSON* output_json, uint8_t* source) {
  if (*source == 't') {
    if (strlen(source) < 4 || !(source[1] == 'r' && source[2] == 'u' && source[3] == 'e'))
      return NULL;

    source += 4;

    output_json->type = BOOLEAN;
    output_json->boolean_value = true;
  } else if (*source == 'f') {
    if (strlen(source) < 5 || !(source[1] == 'a' && source[2] == 'l' && source[3] == 's' && source[4] == 'e'))
      return NULL;

    source += 5;

    output_json->type = BOOLEAN;
    output_json->boolean_value = false;
  } else {
    return NULL;
  }

  return source;
}

// returns the rest of the source, or NULL if there was an error
uint8_t* parse_null(JSON* output_json, uint8_t* source) {
  if (strlen(source) < 4 || !(source[0] == 'n' && source[1] == 'u' && source[2] == 'l' && source[3] == 'l'))
    return NULL;

  source += 4;

  output_json->type = NULL_VALUE;

  return source;
}

// returns the rest of the source, or NULL if there was an error
uint8_t* parse_number(JSON* output_json, uint8_t* source) {
  if (*source != '-' && (*source < '0' || *source > '9')) return NULL;

  double number = 0;

  int sign = 1;
  if (*source == '-') {
    sign = -1;
    ++source;
  }

  if (*source == '0') {
    ++source;

    // no leading zeros
    if (*source >= '0' && *source <= '9') return NULL;
  }

  bool reached_decimal_point = false;
  int shift_left_amount = 0; // -(number of digits after the decimal point) + power

  while (
      (*source >= '0' && *source <= '9')
      || (!reached_decimal_point && *source == '.')
  ) {
    if (*source == '.') reached_decimal_point = true;
    else {
      number *= 10;
      if (reached_decimal_point) --shift_left_amount;
      number += *source - '0';
    }

    ++source;
  }

  // exponent part
  if (*source == 'e' || *source == 'E') {
    ++source;

    int exponent = 0;

    int exponent_sign = 1;
    if (*source == '-') {
      exponent_sign = -1;
      ++source;
    } else if (*source == '+') ++source;

    while (*source >= '0' && *source <= '9') {
      exponent *= 10;
      exponent += *source - '0';
      ++source;
    }

    exponent *= exponent_sign;

    shift_left_amount += exponent;
  }

  if (shift_left_amount >= 0) while (shift_left_amount--) number *= 10;
  else while (shift_left_amount++) number /= 10;

  number *= sign;

  output_json->type = NUMBER;
  output_json->number_value = number;

  return source;
}

// returns the rest of the source, or NULL if there was an error
uint8_t* parse_value(JSON* output_json, uint8_t* source) {
  uint8_t* old_source = source;

  source = parse_string(output_json, old_source);
  if (source == NULL) source = parse_number(output_json, old_source);
  if (source == NULL) source = parse_boolean(output_json, old_source);
  if (source == NULL) source = parse_null(output_json, old_source);

  if (source == NULL) return NULL;

  return source;
}

// returns the rest of the source, or NULL if there was an error
uint8_t* parse_object_entry(JSON* output_json, uint8_t* source) {
  source = parse_string(output_json, source);
  if (source == NULL) return NULL;

  source = skip_whitespace(source);

  if (*source != ':') return NULL;
  ++source;

  source = skip_whitespace(source);

  JSON* value_json = new_JSON();

  source = parse_any(value_json, source);
  if (source == NULL) return NULL;

  output_json->child = value_json;

  return source;
}

// returns the rest of the source, or NULL if there was an error
uint8_t* parse_array(JSON* output_json, uint8_t* source) {
  if (*source != '[') return NULL;
  ++source;

  source = skip_whitespace(source);

  JSON *previous_json = new_JSON();
  source = parse_any(previous_json, source);

  if (source == NULL) return NULL;

  output_json->type = ARRAY;
  output_json->child = previous_json;

  while (true) {
    source = skip_whitespace(source);

    if (*source == ']') {
      ++source;
      break;
    }

    if (*source != ',') return NULL;
    ++source;

    source = skip_whitespace(source);

    JSON* next_json = new_JSON();
    source = parse_any(next_json, source);
    if (source == NULL) return NULL;

    previous_json->next = next_json;
    next_json->previous = previous_json;
    previous_json = next_json;
  }

  return source;
}

// returns the rest of the source, or NULL if there was an error
uint8_t* parse_object(JSON* output_json, uint8_t* source) {
  if (*source != '{') return NULL;
  ++source;

  source = skip_whitespace(source);

  JSON *previous_json = new_JSON();
  source = parse_object_entry(previous_json, source);

  if (source == NULL) return NULL;

  output_json->type = OBJECT;
  output_json->child = previous_json;

  while (true) {
    source = skip_whitespace(source);

    if (*source == '}') {
      ++source;
      break;
    }

    if (*source != ',') return NULL;
    ++source;

    source = skip_whitespace(source);

    JSON* next_json = new_JSON();
    source = parse_object_entry(next_json, source);
    if (source == NULL) return NULL;

    previous_json->next = next_json;
    next_json->previous = previous_json;
    previous_json = next_json;
  }

  return source;
}

// returns the rest of the source, or NULL if there was an error
uint8_t* parse_any(JSON* output_json, uint8_t* source) {
  uint8_t* old_source = source;

  source = parse_value(output_json, old_source);
  if (source == NULL) source = parse_object(output_json, old_source);
  if (source == NULL) source = parse_array(output_json, old_source);

  if (source == NULL) return NULL;

  return source;
}

// returns false if there was an error, true otherwise
bool parse_json(JSON* output_json, uint8_t* source) {
  source = skip_whitespace(source);

  source = parse_any(output_json, source);
  if (source == NULL) return false;

  source = skip_whitespace(source);

  if (*source != '\0') return false;

  return true;
}

void free_json(JSON* json) {
  if (json == NULL) return;

  // free recursively
  free_json(json->next);
  free_json(json->child);

  // free the string value and the JSON struct
  free(json->string_value);
  free(json);
}

// -- String functions

// gets the number of bytes required to represent a code point in utf-8
int utf8_num_bytes(uint32_t code_point) {
  if (code_point > 0b1111111111111111) return 4;
  else if (code_point > 0b11111111111) return 3;
  else if (code_point > 0b1111111) return 2;
  return 1;
}

// output_buffer must have at least 4 bytes (worst case)
// outputs the utf8 character in output
// puts the number of bytes used in output_num_bytes (NULL to ignore)
void code_point_to_utf8(uint8_t* output, int* output_num_bytes, uint32_t code_point) {
  // 0xxx xxxx
  // 110x xxxx  10xx xxxx
  // 1110 xxxx  10xx xxxx  10xx xxxx
  // 1111 0xxx  10xx xxxx  10xx xxxx 10xx xxxx

  int num_bytes = utf8_num_bytes(code_point);

  for (int i = num_bytes - 1; i >= 0; --i) {
    int num_bits = 6;
    if (num_bytes == 1) num_bits = 7;
    else if (i == 0) num_bits = 7 - num_bytes;

    int shift_amount = 32 - num_bits;
    int bits = (code_point << shift_amount) >> shift_amount;
    code_point >>= num_bits;

    output[i] = (0b11111111 >> (num_bits + 1)) << (num_bits + 1) | bits;
  }

  if (output_num_bytes != NULL) *output_num_bytes = num_bytes;
}

// -- JSON utility functions

// gets the string of a JSON string in utf-8
// result must be freed using free()
uint8_t* get_JSON_string(JSON* json) {
  uint32_t* code_points = json->string_value;
  int utf8_string_length = 0;

  while (*code_points) {
    utf8_string_length += utf8_num_bytes(*code_points);
    ++code_points;
  }

  // go back to the start of the string
  code_points = json->string_value;

  uint8_t* utf8_string = malloc((utf8_string_length + 1) * sizeof(uint8_t));

  int byte_index = 0;
  while (*code_points) {
    int num_bytes = 0;
    code_point_to_utf8(utf8_string + byte_index, &num_bytes, *code_points);
    byte_index += num_bytes;
    ++code_points;
  }

  utf8_string[utf8_string_length] = '\0';

  return utf8_string;
}
