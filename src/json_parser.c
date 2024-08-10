#include "json_parser.h"

#include <string.h>
#include <stdlib.h>

JSON* new_JSON() {
  return calloc(1, sizeof(JSON));
}

// returns the rest of the source
char* skip_whitespace(char* source) {
  while (*source == ' ' || *source == '\t' || *source == '\n' || *source == '\r')
    ++source;

  return source;
}

// returns the rest of the source, or NULL if there was an error
// puts the parsed character into parsed_character
char* parse_string_character(char* parsed_character, char* source) {
  if (*source == '\\') {
    ++source;

    if (*source == '\n' || *source == '\0') return NULL;

    if (*source == 'u') {
      ++source;
      if (strlen(source) < 4) return NULL;

      int ascii_code = 0;

      for (int i = 0; i < 4; ++i) {
        ascii_code *= 16;

        if (source[i] >= '0' && source[i] <= '9') ascii_code += source[i] - '0';
        else if (source[i] >= 'a' && source[i] <= 'f') ascii_code += source[i] - 'a' + 10;
        else if (source[i] >= 'A' && source[i] <= 'F') ascii_code += source[i] - 'A' + 10;
        else return NULL;
      }

      *parsed_character = ascii_code;

      source += 4;
    } else {
      if (*source == 'b') *parsed_character = '\b';
      else if (*source == 'f') *parsed_character = '\f';
      else if (*source == 'n') *parsed_character = '\n';
      else if (*source == 'r') *parsed_character = '\r';
      else if (*source == 't') *parsed_character = '\t';
      else *parsed_character = *source;

      ++source;
    }
  } else {
    *parsed_character = *source;
    ++source;
  }

  return source;
}

// returns the rest of the source, or NULL if there was an error
char* parse_string(JSON* output_json, char* source) {
  if (*source != '"') return NULL;
  ++source;

  // string buffer with worst case size
  char string_buffer[strlen(source) + 1];
  int string_buffer_length = 0;

  while (true) {
    if (*source == '\n' || *source == '\0') {
      return NULL;
    } else if (*source == '"') {
      // end the string
      output_json->type = STRING;
      output_json->string_value = malloc((string_buffer_length + 1) * sizeof(char));
      strncpy(output_json->string_value, string_buffer, string_buffer_length);
      ++source;
      break;
    } else {
      // parse the character into the string buffer
      source = parse_string_character(string_buffer + string_buffer_length, source);
      ++string_buffer_length;

      if (source == NULL) return NULL;
    }
  }

  return source;
}

// returns the rest of the source, or NULL if there was an error
char* parse_boolean(JSON* output_json, char* source) {
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
char* parse_null(JSON* output_json, char* source) {
  if (strlen(source) < 4 || !(source[0] == 'n' && source[1] == 'u' && source[2] == 'l' && source[3] == 'l'))
    return NULL;

  source += 4;

  output_json->type = NULL_VALUE;

  return source;
}

// returns the rest of the source, or NULL if there was an error
char* parse_number(JSON* output_json, char* source) {
  if (*source != '-' && (*source < '0' || *source > '9')) return NULL;

  char* number_buffer = source;

  if (*source == '-') ++source;

  if (*source == '0') {
    ++source;

    // no leading zeros
    if (*source >= '0' && *source <= '9') return NULL;
  }

  bool reached_decimal_point = false;

  while ((*source >= '0' && *source <= '9') || (!reached_decimal_point && *source == '.')) {
    if (*source == '.') reached_decimal_point = true;
    ++source;
  }

  // TODO: exponent

  int number_buffer_length = source - number_buffer;
  char number_buffer_copy[(number_buffer_length + 1) * sizeof(char)];
  strncpy(number_buffer_copy, number_buffer, number_buffer_length);

  output_json->type = NUMBER;
  output_json->number_value = atof(number_buffer_copy);

  return source;
}

// returns the rest of the source, or NULL if there was an error
char* parse_value(JSON* output_json, char* source) {
  char* old_source = source;

  source = parse_string(output_json, old_source);
  if (source == NULL) source = parse_number(output_json, old_source);
  if (source == NULL) source = parse_boolean(output_json, old_source);
  if (source == NULL) source = parse_null(output_json, old_source);

  if (source == NULL) return NULL;

  return source;
}

// returns the rest of the source, or NULL if there was an error
char* parse_object_entry(JSON* output_json, char* source) {
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
char* parse_array(JSON* output_json, char* source) {
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
char* parse_object(JSON* output_json, char* source) {
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
char* parse_any(JSON* output_json, char* source) {
  char* old_source = source;

  source = parse_value(output_json, old_source);
  if (source == NULL) source = parse_object(output_json, old_source);
  if (source == NULL) source = parse_array(output_json, old_source);

  if (source == NULL) return NULL;

  return source;
}

// returns false if there was an error, true otherwise
bool parse_json(JSON* output_json, char* source) {
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
