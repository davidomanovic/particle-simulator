# compiler & common flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2 -Iinclude -Ilibs/glfw/include \
          -Wno-missing-braces -Wno-unused-function -Wno-unused-parameter -Wno-stringop-truncation

APP_NAME   = particle_simulator
SRC_DIR    = src
SCRIPT_DIR = scripts
OBJ_DIR    = bin/obj
BIN_DIR    = bin

# platform switches
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
    # assumes libglfw3-dev installed by CI; add more X11 libs if needed
    LDFLAGS  = -lglfw -lGL -lX11 -lpthread -lm
    GLFW_SRC =
endif

# sources / objects
APP_SRC     = $(SRC_DIR)/main.c
SRC_ALL     = $(wildcard $(SRC_DIR)/*.c)
CORE_SRC    = $(filter-out $(APP_SRC), $(SRC_ALL))
SCRIPT_SRC  = $(wildcard $(SCRIPT_DIR)/*.c)

CORE_OBJ    = $(CORE_SRC:%.c=$(OBJ_DIR)/%.o)
SCRIPT_OBJ  = $(SCRIPT_SRC:%.c=$(OBJ_DIR)/%.o)
GLFW_OBJ    = $(GLFW_SRC:%.c=$(OBJ_DIR)/%.o)

# default target
.PHONY: all
all: $(BIN_DIR)/$(APP_NAME)

# generic compile rule
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# link the app
$(BIN_DIR)/$(APP_NAME): $(APP_SRC) $(CORE_OBJ) $(SCRIPT_OBJ) $(GLFW_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# CI-friendly check (doesn't run GUI)
.PHONY: check
check: all
	@echo "Build OK; no headless tests to run."

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
