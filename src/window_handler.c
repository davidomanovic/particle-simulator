#include "window_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define global variables
bool simulation_active = false;
char input_text[MAX_INPUT_LENGTH] = ""; // Default empty input text
bool input_active = false;

typedef struct {
    const char* message;
    double start_time;
    float duration;
    bool active;
} TemporaryText;

TemporaryText temp_text = {NULL, 0.0, 0.0, false};

GLFWwindow* initialize_window(const char* title) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, title, NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    return window;
}

void setup_coordinate_system() {
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1); // Flip the Y-axis
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

bool is_mouse_in_rect(double mouse_x, double mouse_y, float x, float y, float width, float height) {
    return mouse_x >= x && mouse_x <= (x + width) && mouse_y >= y && mouse_y <= (y + height);
}

void handle_input(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (input_active && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (key == GLFW_KEY_BACKSPACE && strlen(input_text) > 0) {
            input_text[strlen(input_text) - 1] = '\0'; // Remove last character
        } else if (strlen(input_text) < MAX_INPUT_LENGTH - 1) {
            char new_char = '\0';

            if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
                new_char = '0' + (key - GLFW_KEY_0);
            } else if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) {
                new_char = '0' + (key - GLFW_KEY_KP_0);
            }

            if (new_char != '\0') {
                strncat(input_text, &new_char, 1);
            }
        }
    }
}

void trigger_temporary_text(const char* message, float duration) {
    temp_text.message = message;
    temp_text.start_time = glfwGetTime();
    temp_text.duration = duration;
    temp_text.active = true;
}

void render_temporary_text() {
    if (temp_text.active) {
        double current_time = glfwGetTime();
        if (current_time - temp_text.start_time < temp_text.duration) {
            // Calculate text dimensions (adjust width based on text length)
            float text_width = strlen(temp_text.message) * 8.0f; // Approximate width per character
            float text_height = 20.0f; // Fixed height
            float x = (WIDTH - text_width) / 2.0f;
            float y = (HEIGHT - text_height) / 2.0f;

            // Render background rectangle
            render_rectangle(x - 10, y - 5, text_width + 20, text_height + 10, 0.9f, 0.9f, 0.9f); // Light gray background

            // Render the text
            render_text(x, y, temp_text.message);
        } else {
            temp_text.active = false; // Deactivate after duration
        }
    }
}

void terminate_window(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void render_ui(GLFWwindow* window, char* input_text, bool* input_active, bool* simulation_active, int* num_particles) {
    input_field();
    start_button();

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        if (is_mouse_in_rect(mouse_x, mouse_y, START_BUTTON_X, START_BUTTON_Y, START_BUTTON_WIDTH, START_BUTTON_HEIGHT)) {
            *num_particles = atoi(input_text);
            if (*num_particles <= 0) {
                *num_particles = 10;
            } else if (*num_particles > 2000) {
                *num_particles = 2000;
                trigger_temporary_text("Maximum 2000 particles allowed :)", 3.0f); // Trigger message
                render_temporary_text();
                glfwSwapBuffers(window);
            }
            *simulation_active = true;
        } else if (is_mouse_in_rect(mouse_x, mouse_y, INPUT_FIELD_X, INPUT_FIELD_Y, INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT)) {
            *input_active = true;
        } else {
            *input_active = false;
        }
    }
}


bool handle_reset(GLFWwindow* window, Particle** particles, bool* simulation_active, char* input_text, int* num_particles) {
    reset_button();

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        if (is_mouse_in_rect(mouse_x, mouse_y, RESET_BUTTON_X, RESET_BUTTON_Y, RESET_BUTTON_WIDTH, RESET_BUTTON_HEIGHT)) {
            if (particles) {
                for (int i = 0; i < *num_particles; i++) {
                    destroy_particle(particles[i]);
                }
                free(particles);
            }
            *simulation_active = false;
            *num_particles = 0;
            strcpy(input_text, ""); // Clear input field
            return true;
        }
    }
    return false;
}

void render_rectangle(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void render_text(float x, float y, const char* text) {
    char buffer[99999];
    int num_quads = stb_easy_font_print(0, 0, (char*)text, NULL, buffer, sizeof(buffer));

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x, y, 0.0f);

    glColor3f(0.0f, 0.0f, 0.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
}

void input_field() {
    render_rectangle(INPUT_FIELD_X, INPUT_FIELD_Y, INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, 0.8f, 0.8f, 0.8f);
    render_text(INPUT_FIELD_X + 10, INPUT_FIELD_Y + 15, "Particles: ");
    render_text(INPUT_FIELD_X + 70, INPUT_FIELD_Y + 15, input_text);
}

void start_button() {
    render_rectangle(START_BUTTON_X, START_BUTTON_Y, START_BUTTON_WIDTH, START_BUTTON_HEIGHT, 0.2f, 0.6f, 0.8f);
    render_text(START_BUTTON_X + 50, START_BUTTON_Y + 20, "Start Simulation");
}

void reset_button() {
    render_rectangle(RESET_BUTTON_X, RESET_BUTTON_Y, RESET_BUTTON_WIDTH, RESET_BUTTON_HEIGHT, 0.8f, 0.2f, 0.2f);
    render_text(RESET_BUTTON_X + 45, RESET_BUTTON_Y + 18, "Reset");
}
