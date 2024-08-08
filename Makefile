SOURCE_DIR=src
TEST_DIR=test
BUILD_DIR=build

CC=gcc
CFLAGS=-Wall

.PHONY: all test always clean

all: test

test: $(BUILD_DIR)/json_parser_test

$(BUILD_DIR)/json_parser_test: always $(TEST_DIR)/json_parser_test.c $(SOURCE_DIR)/json_parser.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/json_parser_test $(TEST_DIR)/json_parser_test.c $(SOURCE_DIR)/json_parser.c

always:
	mkdir -p build

clean:
	rm -rf build
