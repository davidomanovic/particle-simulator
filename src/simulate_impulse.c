#include <simulate_impulse.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600

void simulate_impulse(Particle** particles, int num_particles) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Verlet Particles", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glPointSize(5.0f);

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (deltaTime > 0.1) deltaTime = 0.1;

        glClear(GL_COLOR_BUFFER_BIT);

        // Simulate particles
        for (int i = 0; i < num_particles; i++) {
            integrate(particles[i], deltaTime);
        }

        // Check collisions
        for (int i = 0; i < num_particles; i++) {
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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
