CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -Ilibs/unity/src
LDFLAGS = -lm

SRC = src/particle.c src/movement.c
TESTS = tests/test_particle.c
UNITY_SRC = libs/unity/src/unity.c

test: $(SRC) $(TESTS) $(UNITY_SRC)
	$(CC) $(CFLAGS) -o bin/tests $(SRC) $(TESTS) $(UNITY_SRC) $(LDFLAGS)
	./bin/tests
