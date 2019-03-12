CC = gcc

TARGET_EXEC ?= main

BUILD_DIR ?= build
SRC_DIRS ?= src

CFLAGS = -Wall
LFLAGS = -lm

SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJNAMES := $(SRCS:%.c=%.o)
OBJS := $(OBJNAMES:%=$(BUILD_DIR)/%)

run: $(TARGET_EXEC)
	./$(TARGET_EXEC) script

$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
