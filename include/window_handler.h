#ifndef WINDOW_HANDLER_H
#define WINDOW_HANDLER_H

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "stb_easy_font.h"
#include "particle.h"

// Window dimensions
#define WIDTH 1000
#define HEIGHT 1000
#define MAX_INPUT_LENGTH 16

// UI Element Dimensions
#define START_BUTTON_X 400
#define START_BUTTON_Y 450
#define START_BUTTON_WIDTH 200
#define START_BUTTON_HEIGHT 50

#define INPUT_FIELD_X 400
#define INPUT_FIELD_Y 350
#define INPUT_FIELD_WIDTH 200
#define INPUT_FIELD_HEIGHT 40

#define RESET_BUTTON_X 850
#define RESET_BUTTON_Y 20
#define RESET_BUTTON_WIDTH 130
#define RESET_BUTTON_HEIGHT 40

// Declare global variables as extern
extern bool simulation_active;
extern char input_text[MAX_INPUT_LENGTH];
extern bool input_active;



// Function Prototypes
GLFWwindow* initialize_window(const char* title);
void setup_coordinate_system();
void terminate_window(GLFWwindow* window);

void render_rectangle(float x, float y, float width, float height, float r, float g, float b);
void render_text(float x, float y, const char* text);


void trigger_temporary_text(const char* message, float duration);

void render_temporary_text();

void input_field();
void start_button();
void reset_button();

void render_ui(GLFWwindow* window, char* input_text, bool* input_active, bool* simulation_active, int* num_particles);
bool handle_reset(GLFWwindow* window, Particle** particles, bool* simulation_active, char* input_text, int* num_particles);

bool is_mouse_in_rect(double mouse_x, double mouse_y, float x, float y, float width, float height);
void handle_input(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif
