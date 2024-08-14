#include "../src/json_parser.h"
#include <stdio.h>

int main() {
    // get a new JSON
    JSON* array_json = new_JSON();

    // parse a string into the JSON
    bool success = parse_json(array_json, (uint8_t*)"[{\"key1\": \"value1\", \"key2\": \"ko\"}, 12345, 0.6789, null, true, false]");

    if (!success) {
        printf("Could not parse the JSON");
        return 1;
    }

    // first element in the array
    JSON* object_json = array_json->child;

    // all other elements in the array
    JSON* first_number_json = object_json->next;
    JSON* second_number_json = first_number_json->next;
    JSON* null_json = second_number_json->next;
    JSON* true_json = null_json->next;
    JSON* false_json = true_json->next;

    // you can go backwards in arrays and objects as well
    JSON* true_json_again = false_json->previous;

    // first key in the object
    JSON* object_first_key_json = object_json->child;

    // value corresponding to the first key in the object
    JSON* object_first_value_json = object_first_key_json->child;

    // second key in the object
    JSON* object_second_key_json = object_first_key_json->next;

    // value corresponding to the second key in the object
    JSON* object_second_value_json = object_second_key_json->child;

    // check the type of a JSON
    if (first_number_json->type == NUMBER) {
        // get the number value (all numbers are stored as doubles)
        printf("%f\n", first_number_json->number_value);
    }

    if (object_first_key_json->type == STRING) {
        uint8_t* utf8_string = get_JSON_string(object_first_key_json);
        printf("%s\n", utf8_string);
        free(utf8_string); // important! free the string
    }

    if (null_json->type == NULL_VALUE) {
        // no value for null
        printf("null\n");
    }

    if (true_json->type == BOOLEAN) {
        printf("%s\n", true_json->boolean_value ? "true" : "false");
    }

    // important! free the JSON
    free_json(array_json);
}
