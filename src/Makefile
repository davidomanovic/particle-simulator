# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I../include -I../libs/glfw/include

# Platform-specific libraries
ifeq ($(OS),Windows_NT)
    LDFLAGS = -L../libs/glfw/src -lglfw3 -lopengl32 -lgdi32
else
    LDFLAGS = -lglfw -lGL -lX11 -lpthread -lm
endif

# Source and Object Files
SRC = main.c particle.c movement.c simulate_impulse.c draw_particle.c
OBJ_DIR = ../bin/obj
TEST_DIR = ../tests
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))
TARGET = ../bin/verlet_simulator

# Build Rules
all: $(OBJ_DIR) ../bin $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

../bin:
	mkdir -p ../bin

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

check:
	@echo "No tests defined. Skipping."

clean:
	rm -rf $(OBJ_DIR) ../bin/verlet_simulator
