SOURCE_DIR=src
TEST_DIR=test
BUILD_DIR=build

CC=gcc
CFLAGS=-Wall

TEST_SOURCE_FILES=$(wildcard $(TEST_DIR)/*.c)
TEST_OUTPUT_FILES=$(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%, $(TEST_SOURCE_FILES))

.PHONY: all test always clean

test: $(TEST_OUTPUT_FILES) $(BUILD_DIR)/resource

$(BUILD_DIR)/%: $(TEST_DIR)/%.c $(SOURCE_DIR)/json_parser.c always
	$(CC) $(CFLAGS) -o $@ $< $(SOURCE_DIR)/json_parser.c

$(BUILD_DIR)/resource: $(TEST_DIR)/resource
	cp -R $(TEST_DIR)/resource $(BUILD_DIR)

always:
	mkdir -p build

clean:
	rm -rf build
