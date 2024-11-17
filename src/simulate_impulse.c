#include <simulate_impulse.h>

#define WIDTH 800
#define HEIGHT 600


void simulate_impulse(){
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

    // Spawn two particles
    Particle* p1 = create_particle(WIDTH * 0.2, HEIGHT * 0.5, 0.5f); // Left particle
    Particle* p2 = create_particle(WIDTH * 0.8, HEIGHT * 0.5, 1.0f); // Right particle
    p1->radius = 10.0f;
    p2->radius = 20.0f;

    // Initial velocities to shoot particles at each other
    p1->vx = 100.0f;
    p2->vx = -100.0f;

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
        //apply_force(p1, 0.0f, -9.8f); // Gravity for p1
        //apply_force(p2, 0.0f, -9.8f); // Gravity for p2

        integrate(p1, deltaTime);
        integrate(p2, deltaTime);

        // Check and resolve collision
        if (check_collision(p1, p2)) {
            collision_force(p1, p2);
        }

        // Draw particles
        draw_particle(p1, p1->radius, p1->r, p1->g, p1->b);
        draw_particle(p2, p2->radius, p2->r, p2->g, p2->b);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    destroy_particle(p1);
    destroy_particle(p2);

    glfwDestroyWindow(window);
    glfwTerminate();
}