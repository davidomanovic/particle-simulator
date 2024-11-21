#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <simulate_impulse.h> // Include your core logic
#include <math.h>
#include <gas_simulation.h>
#include <time.h>

#define WIDTH 1920
#define HEIGHT 1080
#define NUM_PARTICLES 500
#define PARTICLE_RADIUS 5.0f

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D // Ensure this constant is defined
#endif

// Particle list
Particle** particles;

// Function to check for initial overlaps
int is_overlapping(float x, float y, int index) {
    for (int i = 0; i < index; i++) {
        float dx = x - particles[i]->x;
        float dy = y - particles[i]->y;
        float distance = sqrtf(dx * dx + dy * dy);
        if (distance < 2 * PARTICLE_RADIUS) {
            return 1; // Overlaps with existing particle
        }
    }
    return 0; // No overlap
}

// Initialize particles with random positions and velocities
void initialize_particles() {
    particles = (Particle**)malloc(NUM_PARTICLES * sizeof(Particle*));
    srand((unsigned int)time(NULL));

    for (int i = 0; i < NUM_PARTICLES; i++) {
        float x, y;
        do {
            x = PARTICLE_RADIUS + ((float)rand() / RAND_MAX) * (WIDTH - 2 * PARTICLE_RADIUS);
            y = PARTICLE_RADIUS + ((float)rand() / RAND_MAX) * (HEIGHT - 2 * PARTICLE_RADIUS);
        } while (is_overlapping(x, y, i));

        // Generate random angle and speed
        float angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        float speed = ((float)rand() / RAND_MAX) * 300.0f; // Adjust max speed as needed

        // Calculate velocity components
        float vx = speed * cosf(angle);
        float vy = speed * sinf(angle);

        particles[i] = create_particle(x, y, 1.0f, PARTICLE_RADIUS); // Create particle with mass 1.0
        particles[i]->vx = vx;
        particles[i]->vy = vy;
        particles[i]->radius = PARTICLE_RADIUS;

        // Set random color for visualization
        particles[i]->r = 0;
        particles[i]->g = 0;
        particles[i]->b = 255;
    }
}

// Handle collisions with walls
void handle_wall_collision(Particle* p) {
    float restitution = 1.0f; // Perfectly elastic collision with wall

    if (p->x - p->radius < 0) { // Left wall
        p->x = p->radius;
        p->vx = -p->vx * restitution;
    }
    if (p->x + p->radius > WIDTH) { // Right wall
        p->x = WIDTH - p->radius;
        p->vx = -p->vx * restitution;
    }
    if (p->y - p->radius < 0) { // Bottom wall
        p->y = p->radius;
        p->vy = -p->vy * restitution;
    }
    if (p->y + p->radius > HEIGHT) { // Top wall
        p->y = HEIGHT - p->radius;
        p->vy = -p->vy * restitution;
    }
}

// Set up the coordinate system
void setup_coordinate_system() {
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1); // Left, Right, Bottom, Top, Near, Far
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
    glEnable(GL_MULTISAMPLE);

    setup_coordinate_system();

    initialize_particles();

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Limit deltaTime to prevent large steps
        if (deltaTime > 0.016f) deltaTime = 0.016f; // Approx. 60 FPS

        glClear(GL_COLOR_BUFFER_BIT);

        // Update positions
        for (int i = 0; i < NUM_PARTICLES; i++) {
            integrate(particles[i], (float)deltaTime);
        }

        // Handle collisions
        for (int i = 0; i < NUM_PARTICLES; i++) {
            handle_wall_collision(particles[i]);
            for (int j = i + 1; j < NUM_PARTICLES; j++) {
                collision_force(particles[i], particles[j]);
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
