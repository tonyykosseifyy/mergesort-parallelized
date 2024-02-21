# Compiler and Linker
CC := gcc

# The Target Binary Program
TARGET := bin/program

# Directories
SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include

# Flags, Libraries and Includes
CFLAGS := -Wall -I$(INC_DIR)
LDFLAGS :=
LIBS :=

# Source and Object Files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Default make
all: $(TARGET)

# Link the target binary
$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS) $(LIBS)

# Compile the source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the object file directory
$(OBJ_DIR):
	mkdir -p $@

# Clean the build
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Non-file targets
.PHONY: all clean
