SOURCE_DIR=src
TEST_DIR=test
TARGET_DIR=target

CC=gcc
CFLAGS=-Wall

TEST_SOURCE_FILES=$(wildcard $(TEST_DIR)/*.c)
TEST_OUTPUT_FILES=$(patsubst $(TEST_DIR)/%.c, $(TARGET_DIR)/%, $(TEST_SOURCE_FILES))

.PHONY: all test always clean

test: $(TEST_OUTPUT_FILES) $(TARGET_DIR)/resource

$(TARGET_DIR)/%: $(TEST_DIR)/%.c $(SOURCE_DIR)/json_parser.c always
	$(CC) $(CFLAGS) -o $@ $< $(SOURCE_DIR)/json_parser.c

$(TARGET_DIR)/resource: $(TEST_DIR)/resource
	cp -R $(TEST_DIR)/resource $(TARGET_DIR)

always:
	mkdir -p $(TARGET_DIR)

clean:
	rm -rf $(TARGET_DIR)
