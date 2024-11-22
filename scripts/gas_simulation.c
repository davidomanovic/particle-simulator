
#include "gas_simulation.h"

#define WIDTH 1000
#define HEIGHT 1000

#define BUTTON_X 400
#define BUTTON_Y 450
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 50

#define INPUT_FIELD_X 400
#define INPUT_FIELD_Y 350
#define INPUT_FIELD_WIDTH 200
#define INPUT_FIELD_HEIGHT 40

#define RESET_BUTTON_X 850
#define RESET_BUTTON_Y 20
#define RESET_BUTTON_WIDTH 130
#define RESET_BUTTON_HEIGHT 40


#define PARTICLE_RADIUS 5.0f
#define MAX_INPUT_LENGTH 16

// Globals
Particle** particles = NULL;
int num_particles = 0;
bool simulation_active = false;
char input_text[MAX_INPUT_LENGTH] = ""; // Default input value
bool input_active = false;

// Render a rectangle
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
    char buffer[99999]; // Temporary buffer for vertices
    int num_quads;

    glPushMatrix();
    glLoadIdentity();  // Reset transformations
    glTranslatef(x, y, 0.0f); // Correct Y-axis position

    glColor3f(0.0f, 0.0f, 0.0f); // Set text color
    num_quads = stb_easy_font_print(0, 0, (char*)text, NULL, buffer, sizeof(buffer));
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
}


// Check if mouse is in a rectangle
bool is_mouse_in_rect(double mouse_x, double mouse_y, float x, float y, float width, float height) {
    return mouse_x >= x && mouse_x <= (x + width) && mouse_y >= y && mouse_y <= (y + height);
}

// Handle input for the input field
void handle_input(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (input_active && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (key == GLFW_KEY_BACKSPACE && strlen(input_text) > 0) {
            input_text[strlen(input_text) - 1] = '\0'; // Remove last character
        } else if (strlen(input_text) < MAX_INPUT_LENGTH - 1) {
            char new_char = '\0';

            // Handle main number keys (0-9)
            if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
                new_char = '0' + (key - GLFW_KEY_0); // Correct mapping
            }
            // Handle Numpad number keys (if needed)
            else if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) {
                new_char = '0' + (key - GLFW_KEY_KP_0);
            }

            if (new_char != '\0') {
                strncat(input_text, &new_char, 1);
            }
        }
    }
}


// Set up the coordinate system
void setup_coordinate_system() {
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1); // Flip the Y-axis by setting Top to 0 and Bottom to HEIGHT
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


// Main simulation function
void simulate_gas() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Gas Simulation", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, handle_input);
    setup_coordinate_system();
    glDisable(GL_DEPTH_TEST);

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        bool mouse_pressed = false;
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT);

        if (!simulation_active) {
            // Render input field
            render_rectangle(INPUT_FIELD_X, INPUT_FIELD_Y, INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT, 0.8f, 0.8f, 0.8f);
            render_text(INPUT_FIELD_X + 10, INPUT_FIELD_Y + 15, "Particles: ");
            render_text(INPUT_FIELD_X + 70, INPUT_FIELD_Y + 15, input_text);

            // Render button
            render_rectangle(BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, 0.2f, 0.6f, 0.8f);
            render_text(BUTTON_X + 50, BUTTON_Y + 20, "Start Simulation");

            // Handle mouse input
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                double mouse_x, mouse_y;
                glfwGetCursorPos(window, &mouse_x, &mouse_y);

                // Check if mouse is in the button
                if (is_mouse_in_rect(mouse_x, mouse_y, BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT)) {
                    num_particles = atoi(input_text); // Convert input to number
                    if (num_particles <= 0 || num_particles > 2000) num_particles = 500; // Validate input
                    particles = (Particle**)malloc(num_particles * sizeof(Particle*));
                    initialize_particles(&particles, num_particles, PARTICLE_RADIUS, WIDTH, HEIGHT);
                    simulation_active = true;
                }
                // Check if mouse is in the input field
                else if (is_mouse_in_rect(mouse_x, mouse_y, INPUT_FIELD_X, INPUT_FIELD_Y, INPUT_FIELD_WIDTH, INPUT_FIELD_HEIGHT)) {
                    input_active = true; // Activate input field
                }
                else {
                    input_active = false; // Deactivate input field
                }
            }
        } else {
            
            // Render the reset button
            render_rectangle(RESET_BUTTON_X, RESET_BUTTON_Y, RESET_BUTTON_WIDTH, RESET_BUTTON_HEIGHT, 0.8f, 0.2f, 0.2f);
            render_text(RESET_BUTTON_X + 45, RESET_BUTTON_Y + 18, "Reset");    

            // Update positions
            for (int i = 0; i < num_particles; i++) {
                integrate(particles[i], (float)deltaTime);
            }
            double mouse_x, mouse_y;
            glfwGetCursorPos(window, &mouse_x, &mouse_y);
            // Handle mouse input for reset button
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                if (!mouse_pressed) {
                    mouse_pressed = true;

                    double mouse_x, mouse_y;
                    glfwGetCursorPos(window, &mouse_x, &mouse_y);
                    if(is_mouse_in_rect(mouse_x, mouse_y, RESET_BUTTON_X, RESET_BUTTON_Y, RESET_BUTTON_WIDTH, RESET_BUTTON_HEIGHT)) {
                        // Reset the simulation
                        if (particles) {
                            for (int i = 0; i < num_particles; i++) {
                                destroy_particle(particles[i]);
                            }
                            free(particles);
                            particles = NULL;
                        }
                        simulation_active = false;
                        num_particles = 0;
                        input_active = false;
                        input_text[0] = '\0'; // Clear input text if desired
                    }
                }
            } else {
                mouse_pressed = false;
            }

            // Handle collisions
            for (int i = 0; i < num_particles; i++) {
                handle_wall_collision(particles[i], WIDTH, HEIGHT);
                for (int j = i + 1; j < num_particles; j++) {
                    if (check_collision(particles[i], particles[j])) {
                        collision_force(particles[i], particles[j]);
                    }
                }
            }

            // Render particles
            for (int i = 0; i < num_particles; i++) {
                draw_particle(particles[i]);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    if (particles) {
        for (int i = 0; i < num_particles; i++) {
            destroy_particle(particles[i]);
        }
        free(particles);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

