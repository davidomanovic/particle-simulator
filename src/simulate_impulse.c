#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <simulate_impulse.h>

#define WIDTH 1000
#define HEIGHT 1000

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D // Ensure this constant is defined
#endif

// Particle list and count
Particle** particles = NULL;
int num_particles = 0;
int max_particles = 100; // Maximum number of particles

// Add a new particle at a random location
void add_particle(float x, float y, float mass, float radius, float vx, float vy) {
    if (num_particles >= max_particles) return;
    Particle* new_particle = create_particle(x, y, mass,radius);
    new_particle->vx = vx;
    new_particle->vy = vy;
    particles[num_particles++] = new_particle;
}

// Key callback for handling space bar press
void key_callback(GLFWwindow* window __attribute__((unused)),
                 int key, 
                 int scancode __attribute__((unused)), 
                 int action, 
                 int mods __attribute__((unused))) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        add_particle(80.0f, 300.0f, 10.0f, 10.0f, 200, 0);
    }
}

void simulate_impulse() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    // Enable 4x multi-sampling for anti-aliasing
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Verlet Particles", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glEnable(GL_MULTISAMPLE);
    glfwSetKeyCallback(window, key_callback);

    // Allocate memory for particles
    particles = (Particle**)malloc(max_particles * sizeof(Particle*));
    add_particle(300.0f, 300.0f, 1000.0f, 30.0f, 0, 0); // Big boi
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

    // Clean up
    for (int i = 0; i < num_particles; i++) {
        destroy_particle(particles[i]);
    }
    free(particles);

    glfwDestroyWindow(window);
    glfwTerminate();
}
