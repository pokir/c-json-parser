#include "../src/json_parser.h"

#include <stdio.h>

int main() {
  FILE* file = fopen("resource/small_file.json", "r");
  if (file == NULL) {
    printf("Could not open the JSON file");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  uint8_t source[length];
  fread(source, 1, length, file);
  fclose(file);

  JSON* json = new_JSON();
  if (!parse_json(json, source)) printf("Could not parse the JSON file");

  free_json(json);
}
