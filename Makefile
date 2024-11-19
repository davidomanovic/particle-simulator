# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude -Ilibs/glfw/include

# Platform-specific libraries
ifeq ($(OS),Windows_NT)
    LDFLAGS = -Llibs/glfw/src -lglfw3 -lopengl32 -lgdi32
else
    LDFLAGS = -lglfw -lGL -lX11 -lpthread -lm
endif

# Directories
SRC_DIR = src
SCRIPT_DIR = scripts
OBJ_DIR = bin/obj
BIN_DIR = bin

# Source and Object Files
CORE_SRC = $(wildcard $(SRC_DIR)/*.c)
CORE_OBJ = $(CORE_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Scripts (entry points)
SCRIPTS = simulate_impulse
SCRIPT_BINS = $(SCRIPTS:%=$(BIN_DIR)/%)

# Default Target
all: $(SCRIPT_BINS)

# Compile core object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build each script binary
$(BIN_DIR)/%: $(SCRIPT_DIR)/%.c $(CORE_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(CORE_OBJ) $(LDFLAGS)

# Check target for running tests
check: all
	@echo "Running tests..."
	@$(BIN_DIR)/simulate_impulse --test || echo "Tests failed"

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
