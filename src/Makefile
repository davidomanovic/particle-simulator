CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude -Ilibs/glfw/include
LDFLAGS = -Llibs/glfw/src -lglfw3 -lopengl32 -lgdi32
SRC = src/main.c src/particle.c src/movement.c src/simulation.c src/draw_particle.c
OBJ = $(SRC:.c=.o)
TARGET = bin/verlet_simulator

all: bin $(TARGET)

bin:
	mkdir -p bin

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(OBJ) $(TARGET)
