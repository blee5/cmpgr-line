CC = gcc

TARGET_EXEC ?= mdl

BUILD_DIR ?= build
SRC_DIR ?= src
PAR_DIR ?= yacc

CFLAGS := -Wall -Wextra -O3 -Wno-unused-result -Wno-unused-variable -g
LFLAGS := -lm

SRCS := $(shell find $(SRC_DIR) -name "*.c")
PARS := $(shell find $(PAR_DIR) -name "*.c")
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

run: $(TARGET_EXEC)
	./$(TARGET_EXEC) test.mdl

$(PAR_DIR)/lex.yy.c: $(PAR_DIR)/mdl.l $(PAR_DIR)/mdl.tab.h
	flex -o $@ $(PAR_DIR)/mdl.l

$(PAR_DIR)/mdl.tab.c: $(PAR_DIR)/mdl.y
	bison -o $@ $(PAR_DIR)/mdl.y

$(PAR_DIR)/mdl.tab.h: $(PAR_DIR)/mdl.y
	bison --defines=$@ -o $(PAR_DIR)/mdl.tab.c $(PAR_DIR)/mdl.y

$(TARGET_EXEC): $(PAR_DIR)/mdl.tab.h $(PAR_DIR)/mdl.tab.c $(PAR_DIR)/lex.yy.c $(OBJS)
	$(CC) $^ -o $@ $(LFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS) $(LFLAGS) -I $(PAR_DIR)

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET_EXEC)
	@rm -f $(PAR_DIR)/mdl.tab.c $(PAR_DIR)/mdl.tab.h $(PAR_DIR)/lex.yy.c

