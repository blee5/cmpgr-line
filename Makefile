CC = gcc

TARGET_EXEC ?= main

BUILD_DIR ?= build
SRC_DIR ?= src

CFLAGS := -Wall
LFLAGS := -lm

SRCS := $(shell find $(SRC_DIR) -name "*.c")
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

run: $(TARGET_EXEC)
	./$(TARGET_EXEC) script

$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET_EXEC)
