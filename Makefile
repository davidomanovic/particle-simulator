# Compiler and Flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2 -Iinclude -Ilibs/glfw/include \
          -Wno-missing-braces -Wno-unused-function -Wno-unused-parameter -Wno-stringop-truncation

# Platform-specific
ifeq ($(OS),Windows_NT)
    CFLAGS  += -D_GLFW_WIN32
    LDFLAGS  = -lopengl32 -lgdi32 -luser32 -lshell32 -lwinmm -limm32
    GLFW_SRC = \
        libs/glfw/src/context.c \
        libs/glfw/src/init.c \
        libs/glfw/src/input.c \
        libs/glfw/src/monitor.c \
        libs/glfw/src/platform.c \
        libs/glfw/src/vulkan.c \
        libs/glfw/src/window.c \
        libs/glfw/src/win32_init.c \
        libs/glfw/src/win32_joystick.c \
        libs/glfw/src/win32_module.c \
        libs/glfw/src/win32_monitor.c \
        libs/glfw/src/win32_time.c \
        libs/glfw/src/win32_thread.c \
        libs/glfw/src/win32_window.c \
        libs/glfw/src/wgl_context.c \
        libs/glfw/src/egl_context.c \
        libs/glfw/src/osmesa_context.c \
        libs/glfw/src/null_init.c \
        libs/glfw/src/null_monitor.c \
        libs/glfw/src/null_window.c \
        libs/glfw/src/null_joystick.c
else
    LDFLAGS  = -lglfw -lGL -lX11 -lpthread -lm
    GLFW_SRC =
endif

# Directories
SRC_DIR    = src
SCRIPT_DIR = scripts
OBJ_DIR    = bin/obj
BIN_DIR    = bin

# Sources
CORE_SRC = $(wildcard $(SRC_DIR)/*.c)
CORE_OBJ = $(CORE_SRC:%.c=$(OBJ_DIR)/%.o)
GLFW_OBJ = $(GLFW_SRC:%.c=$(OBJ_DIR)/%.o)

# Executables
SCRIPTS     = gas_simulation
SCRIPT_BINS = $(SCRIPTS:%=$(BIN_DIR)/%)

# Default target
all: $(SCRIPT_BINS)

.PHONY: check
check: all
	@if command -v xvfb-run >/dev/null 2>&1; then \
	  xvfb-run -a $(BIN_DIR)/gas_simulation --test || echo "UI test skipped/failed"; \
	else \
	  echo "No X display; skipping UI run"; \
	fi

# Pattern rule: compile any .c into a .o under bin/obj
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link each script
$(BIN_DIR)/%: $(SCRIPT_DIR)/%.c $(CORE_OBJ) $(GLFW_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(CORE_OBJ) $(GLFW_OBJ) $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
