#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <simulate_impulse.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 800

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D // Ensure this constant is defined
#endif

// Log particle details
void log_particle(const Particle* p, const char* label) {
    printf("%s: Position=(%.2f, %.2f), Velocity=(%.2f, %.2f)\n",
           label, p->x, p->y, p->vx, p->vy);
}

void test_particle_creation() {
    printf("Test: Particle Creation\n");
    Particle* p = create_particle(WIDTH / 2.0f, HEIGHT / 2.0f, 10.0f, 10.0f);
    p->vx = 100.0f;
    p->vy = -50.0f;
    log_particle(p, "Created Particle");
    destroy_particle(p);
    printf("Particle creation test passed.\n\n");
}

void test_collision() {
    printf("Test: Collision\n");
    Particle* p1 = create_particle(400.0f, 400.0f, 10.0f, 10.0f);
    Particle* p2 = create_particle(420.0f, 400.0f, 10.0f, 10.0f);

    p1->vx = 50.0f;
    p2->vx = -50.0f;

    log_particle(p1, "Before Collision - Particle 1");
    log_particle(p2, "Before Collision - Particle 2");

    // Simulate a simple collision
    if (check_collision(p1, p2)) {
        collision_force(p1, p2);
    }

    log_particle(p1, "After Collision - Particle 1");
    log_particle(p2, "After Collision - Particle 2");

    destroy_particle(p1);
    destroy_particle(p2);
    printf("Collision test passed.\n\n");
}

void test_rendering() {
    printf("Test: Rendering\n");

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Particle Rendering Test", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glEnable(GL_MULTISAMPLE);

    Particle* p = create_particle(WIDTH / 2.0f, HEIGHT / 2.0f, 10.0f, 10.0f);
    p->radius = 20.0f;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Render particle
        draw_particle(p);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    destroy_particle(p);
    glfwDestroyWindow(window);
    glfwTerminate();
    printf("Rendering test passed.\n");
}

int main() {
    test_particle_creation();
    test_collision();
    test_rendering();
    return 0;
}
