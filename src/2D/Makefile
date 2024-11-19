# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I../../include/2D -I../../libs/glfw/include

# Platform-specific libraries
ifeq ($(OS),Windows_NT)
    LDFLAGS = -L../../libs/glfw/src -lglfw3 -lopengl32 -lgdi32
else
    LDFLAGS = -lglfw -lGL -lX11 -lpthread -lm
endif

# Source and Object Files
SRC_DIR = .
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/particle.c $(SRC_DIR)/movement.c $(SRC_DIR)/simulate_impulse.c $(SRC_DIR)/draw_particle.c
OBJ_DIR = ../../bin/2D_simulator/obj
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
TARGET = ../../bin/2D_simulator/verlet_simulator

# Build Rules
all: $(OBJ_DIR) ../../bin/2D_simulator $(TARGET)

# Create the object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Create the binary directory
../../bin/2D_simulator:
	mkdir -p ../../bin/2D_simulator

# Build the target executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Placeholder for tests
check:
	@echo "No tests defined. Skipping."

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) ../../bin/2D_simulator/verlet_simulator
