#pragma once
#include <GLFW/glfw3.h>

typedef enum { STATE_MENU, STATE_GAS, STATE_IMPULSE, STATE_QUIT } AppState;

AppState run_menu(GLFWwindow* window, int width, int height);
