#include <GLFW/glfw3.h>
#include <particle.h>
#include <movement.h>
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600

// Convert particle position to OpenGL coordinates
void draw_particle(const Particle* p) {
    float px = (p->x / WIDTH) * 2.0f - 1.0f;  // Map x to [-1, 1]
    float py = (p->y / HEIGHT) * 2.0f - 1.0f; // Map y to [-1, 1]
    glBegin(GL_POINTS); // Draw a single point for the particle
    glVertex2f(px, py);
    glEnd();
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Verlet Particles", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glPointSize(5.0f); // Set point size for particles

    // Initialize particles
    int num_particles = 1000;
    Particle* particles[num_particles];
    for (int i = 0; i < num_particles; i++) {
        particles[i] = create_particle(rand() % WIDTH, rand() % HEIGHT, 1.0f);
    }

    // Main render loop
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Update particle positions
        for (int i = 0; i < num_particles; i++) {
            apply_force(particles[i], 0.0f, -9.8f); // Gravity
            integrate(particles[i], deltaTime);
        }

        // Clear screen and render particles
        glClear(GL_COLOR_BUFFER_BIT);
        for (int i = 0; i < num_particles; i++) {
            draw_particle(particles[i]);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    for (int i = 0; i < num_particles; i++) {
        destroy_particle(particles[i]);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
