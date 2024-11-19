#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <simulate_impulse.h> // Include your core logic
#include <math.h>
#include <gas_simulation.h>
#include <time.h>

#define WIDTH 1000
#define HEIGHT 1000
#define NUM_PARTICLES 100
#define PARTICLE_RADIUS 10.0f

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D // Ensure this constant is defined
#endif

// Particle list
Particle** particles;

// Initialize particles with random positions and velocities
void initialize_particles() {
    particles = (Particle**)malloc(NUM_PARTICLES * sizeof(Particle*));
    srand((unsigned int)time(NULL));

    for (int i = 0; i < NUM_PARTICLES; i++) {
        float x = PARTICLE_RADIUS + (rand() % (WIDTH - (int)(2 * PARTICLE_RADIUS)));
        float y = PARTICLE_RADIUS + (rand() % (HEIGHT - (int)(2 * PARTICLE_RADIUS)));
        float vx = (rand() % 200 - 100) / 10.0f; // Random velocity in range [-10, 10]
        float vy = (rand() % 200 - 100) / 10.0f;

        particles[i] = create_particle(x, y, 1.0f, 10.0f); // Create particle with mass 1.0
        particles[i]->vx = vx;
        particles[i]->vy = vy;
        particles[i]->radius = PARTICLE_RADIUS;
    }
}

// Handle collisions with walls
void handle_wall_collision(Particle* p) {
    if (p->x - p->radius < 0) { // Left wall
        p->x = p->radius;
        p->vx = -p->vx;
    }
    if (p->x + p->radius > WIDTH) { // Right wall
        p->x = WIDTH - p->radius;
        p->vx = -p->vx;
    }
    if (p->y - p->radius < 0) { // Bottom wall
        p->y = p->radius;
        p->vy = -p->vy;
    }
    if (p->y + p->radius > HEIGHT) { // Top wall
        p->y = HEIGHT - p->radius;
        p->vy = -p->vy;
    }
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
    glEnable(GL_MULTISAMPLE);
    glPointSize(5.0f);

    initialize_particles();

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (deltaTime > 0.1) deltaTime = 0.1;

        glClear(GL_COLOR_BUFFER_BIT);

        // Simulate particles
        for (int i = 0; i < NUM_PARTICLES; i++) {
            integrate(particles[i], deltaTime); // Update position and velocity
            handle_wall_collision(particles[i]); // Handle wall collisions
        }

        // Check collisions between particles
        for (int i = 0; i < NUM_PARTICLES; i++) {
            for (int j = i + 1; j < NUM_PARTICLES; j++) {
                if (check_collision(particles[i], particles[j])) {
                    collision_force(particles[i], particles[j]);
                }
            }
        }

        // Render particles
        for (int i = 0; i < NUM_PARTICLES; i++) {
            draw_particle(particles[i]);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    for (int i = 0; i < NUM_PARTICLES; i++) {
        destroy_particle(particles[i]);
    }
    free(particles);

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {
    simulate_gas();
    return 0;
}
