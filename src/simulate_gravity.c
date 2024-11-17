#include <simulate_gravity.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600
#define NUM_PARTICLES 50
#define GRAVITY -9.8f

void simulate_gravity() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Verlet Particles", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glPointSize(5.0f); // Set point size for particles

    // Initialize particles
    Particle* particles[NUM_PARTICLES];
    for (int i = 0; i < NUM_PARTICLES; i++) {
        // Randomly spawn particles within the screen bounds
        float x = (float)(rand() % WIDTH);
        float y = (float)(rand() % HEIGHT);
        float mass = 0.5f + (rand() % 100) / 200.0f; // Random mass between 0.5 and 1.0
        particles[i] = create_particle(x, y, mass);
        particles[i]->radius = 5.0f + (rand() % 5); // Random radius between 5 and 10
        particles[i]->vx = (rand() % 200 - 100) / 100.0f; // Random velocity in x
        particles[i]->vy = (rand() % 200 - 100) / 100.0f; // Random velocity in y
    }

    // Main render loop
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Clamp the time step to avoid large steps
        if (deltaTime > 0.01) deltaTime = 0.01; // Limit to 10ms

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Update particles
        for (int i = 0; i < NUM_PARTICLES; i++) {
            // Apply gravity to each particle
            apply_force(particles[i], 0.0f, particles[i]->mass * GRAVITY);

            // Integrate particle movement
            integrate(particles[i], deltaTime);

            // Handle collisions with other particles
            for (int j = i + 1; j < NUM_PARTICLES; j++) {
                if (check_collision(particles[i], particles[j])) {
                    collision_force(particles[i], particles[j]);
                }
            }

            // Draw each particle
            draw_particle(particles[i], particles[i]->radius, particles[i]->r, particles[i]->g, particles[i]->b);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    for (int i = 0; i < NUM_PARTICLES; i++) {
        destroy_particle(particles[i]);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
