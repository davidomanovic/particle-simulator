#include "window_handler.h"
#include "particle.h"
#include "gas_simulation.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdlib.h>

static void free_particles_array(Particle*** arr, int* count) {
    if (arr && *arr) {
        for (int i = 0; i < *count; ++i) {
            if ((*arr)[i]) { destroy_particle((*arr)[i]); (*arr)[i] = NULL; }
        }
        free(*arr);
        *arr = NULL;
    }
    if (count) *count = 0;
}

bool simulate_gas(GLFWwindow* window) {
    bool go_back = false;

    simulation_active = false;
    input_active = false;
    input_text[0] = '\0';

    Particle** particles = NULL;
    int num_particles = 0;

    double last = glfwGetTime();
    int prevMouseDown = GLFW_RELEASE;
    bool suppress_menu_click = false;

    // one-time GL state
    glDisable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // *** window handler owns viewport/projection + auto-rescale on resize
        window_begin_frame(window);

        double now = glfwGetTime();
        double dt  = now - last; last = now;
        if (dt > 0.1) dt = 0.1;

        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { go_back = true; break; }

        if (!simulation_active) {
            render_gas_ui(window, input_text, &input_active, &simulation_active, &num_particles);
            glfwSetKeyCallback(window, handle_input);

            {   // Return to main (menu-only)
                int w, h; get_world_size(window, &w, &h);
                glMatrixMode(GL_PROJECTION); glLoadIdentity(); glOrtho(0, w, h, 0, -1, 1);
                glMatrixMode(GL_MODELVIEW);  glLoadIdentity();

                double mx, my; glfwGetCursorPos(window, &mx, &my);
                int mouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
                bool clickedEdge = (prevMouseDown == GLFW_RELEASE && mouseDown == GLFW_PRESS);
                prevMouseDown = mouseDown;

                if (suppress_menu_click) {
                    if (mouseDown == GLFW_RELEASE) suppress_menu_click = false;
                } else {
                    if (draw_button(10, 10, 200, 50, "Return to Main Menu", mx, my, clickedEdge)) {
                        go_back = true; break;
                    }
                }
            }

            if (simulation_active) {
                int val = (int)strtol(input_text, NULL, 10);
                if (val < 1) val = 1;
                if (val > MAX_PARTICLES) val = MAX_PARTICLES;
                num_particles = val;

                int worldW, worldH; get_world_size(window, &worldW, &worldH);

                free_particles_array(&particles, &num_particles);
                num_particles = val;
                particles = (Particle**)malloc((size_t)num_particles * sizeof(Particle*));
                initialize_particles(&particles, num_particles, 10.0f, worldW, worldH);

                // *** tell window handler which particles to rescale on future resizes
                window_bind_particles(&particles, &num_particles, /*scale_velocities=*/true);

                prevMouseDown = GLFW_RELEASE;
                suppress_menu_click = true;
            }
        } else {
            // still keep UI top-left for reset button drawing
            int w, h; get_world_size(window, &w, &h);

            if (handle_reset(window, particles, &simulation_active, input_text, &num_particles)) {
                // stop auto-rescaling before freeing
                window_unbind_particles();
                free_particles_array(&particles, &num_particles);
                prevMouseDown = GLFW_RELEASE;
                suppress_menu_click = true;
                goto swap_and_continue;
            }

            // physics using *current* world size
            for (int i = 0; i < num_particles; ++i) {
                integrate(particles[i], (float)dt);
                handle_wall_collision(particles[i], w, h);
                apply_force(particles[i], 0, -9.81f);
            }
            for (int i = 0; i < num_particles; ++i)
                for (int j = i + 1; j < num_particles; ++j)
                    if (check_collision(particles[i], particles[j]))
                        collision_force(particles[i], particles[j]);

            for (int i = 0; i < num_particles; ++i)
                draw_particle(particles[i]);
        }

swap_and_continue:
        glfwSwapBuffers(window);
    }

    // unbind on exit and free
    window_unbind_particles();
    free_particles_array(&particles, &num_particles);
    return go_back;
}
