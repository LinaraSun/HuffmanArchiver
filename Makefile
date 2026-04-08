CC = gcc
CFLAGS = -g -fsanitize=address,undefined -MMD -MP
LDFLAGS =

TARGET = huffArchiver

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
HEADERS = $(wildcard $(SRC_DIR)/*.h)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(OBJECTS:.o=.d)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

.PHONY: all clean