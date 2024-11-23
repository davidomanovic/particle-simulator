#include "window_handler.h"
#include "particle.h"
#include "gas_simulation.h"
#include <stdlib.h>

void simulate_gas() {
    // Initialize the window, buttons, and input field
    GLFWwindow* window = initialize_window("Gas Simulation");
    setup_coordinate_system();
    glDisable(GL_DEPTH_TEST);
    Particle** particles = NULL;
    int num_particles = 0;

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT);

        // If the simulation isn't active, render the UI
        if (!simulation_active) {
            render_ui(window, input_text, &input_active, &simulation_active, &num_particles);
            glfwSetKeyCallback(window, handle_input);

            // If the simulation has started, initialize particles
            if (simulation_active) {
                particles = (Particle**)malloc(num_particles * sizeof(Particle*));
                initialize_particles(&particles, num_particles, 5.0f, WIDTH, HEIGHT);
            }
        } else {
            // Render reset button and handle reset
            if (handle_reset(window, particles, &simulation_active, input_text, &num_particles)) {
                continue; // Skip simulation logic after reset
            }

            // Update and render particles
            for (int i = 0; i < num_particles; i++) {
                integrate(particles[i], (float)deltaTime);
                handle_wall_collision(particles[i], WIDTH, HEIGHT);
                apply_force(particles[i], 0 , -9.81f);
            }

            // Check for particle collisions
            for (int i = 0; i < num_particles; i++) {
                for (int j = i + 1; j < num_particles; j++) {
                    if (check_collision(particles[i], particles[j])) {
                        collision_force(particles[i], particles[j]);
                    }
                }
            }

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

    terminate_window(window);
}
