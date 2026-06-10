CC = gcc
CFLAGS = -g -fsanitize=address,undefined -MMD -MP
LDFLAGS =

TARGET = huffArchiver

SRC_DIR = src
BUILD_DIR = build

TEST_DIR = test/modular
TEST_BUILD_DIR = $(BUILD_DIR)/tests

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
HEADERS = $(wildcard $(SRC_DIR)/*.h)

#
# Main program
#

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

#
# Modular tests
#

TEST_SOURCES := $(wildcard $(TEST_DIR)/*.c)

TEST_EXECUTABLES := \
	$(patsubst $(TEST_DIR)/%.c,$(TEST_BUILD_DIR)/%,$(TEST_SOURCES))

$(TEST_BUILD_DIR):
	mkdir -p $(TEST_BUILD_DIR)

#
# Building each test
#

PROJECT_TEST_SOURCES := \
	$(filter-out $(SRC_DIR)/main.c,$(SOURCES))

$(TEST_BUILD_DIR)/%: $(TEST_DIR)/%.c $(PROJECT_TEST_SOURCES) $(HEADERS) | $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) $< $(PROJECT_TEST_SOURCES) -o $@

test: $(TEST_EXECUTABLES)
	@for t in $(TEST_EXECUTABLES); do \
		echo "Running $$t"; \
		$$t; \
	done

#
# Cleanup
#

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

#
# Dependency files
#

-include $(OBJECTS:.o=.d)

.PHONY: all clean