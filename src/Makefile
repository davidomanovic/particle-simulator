# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I../include -I../libs/glfw/include
LDFLAGS = -L../libs/glfw/src -lglfw3 -lopengl32 -lgdi32

# Source and Target
SRC = main.c particle.c movement.c simulate_impulse.c draw_particle.c
OBJ_DIR = ../bin/obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))
TARGET = ../bin/verlet_simulator

# Build Rules
all: $(OBJ_DIR) ../bin $(TARGET)

# Create the object file directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Create the binary directory
../bin:
	mkdir -p ../bin

# Link the target binary
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

# Compile each .c file into its corresponding .o file
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) ../bin
