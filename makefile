CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -Wshadow -pedantic

DEP_FILE = $(OBJ_DIR)/$*.d
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_FILE)

SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

SRCS = $(shell find $(SRC_DIR) -name *.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS)) #$(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) other alternative
DEPS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.d, $(SRCS))


EXE = malloc_perso

all: $(EXE)


$(EXE):	$(OBJS)
	$(CC) $(CFLAGS) $^ -o $(EXE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(DEPFLAGS) $< -o $@

clean:
	@rm -rf $(BUILD_DIR)

re: clean all

.PHONY: all clean re

$(DEPS):
include $(DEPS)
