#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include "menu.h"

// your sims:
bool simulate_gas(GLFWwindow* window);
bool simulate_impulse(GLFWwindow* window);

#define WIDTH 1000
#define HEIGHT 1000

static void error_callback(int code, const char* desc) {
    fprintf(stderr, "GLFW Error %d: %s\n", code, desc);
}

int main(void) {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return 1;

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Particle Simulator", NULL, NULL);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    AppState state = STATE_MENU;
    while (state != STATE_QUIT && !glfwWindowShouldClose(window)) {
        switch (state) {
            case STATE_MENU:    state = run_menu(window, WIDTH, HEIGHT); break;
            case STATE_GAS:     state = simulate_gas(window)     ? STATE_MENU : STATE_QUIT; break;
            case STATE_IMPULSE: state = simulate_impulse(window) ? STATE_MENU : STATE_QUIT; break;
            default:            state = STATE_QUIT; break;
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
