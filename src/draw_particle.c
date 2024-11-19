#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <draw_particle.h>
#include <GLFW/glfw3.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

void draw_particle(const Particle* p) {
    float px = (p->x / WIDTH) * 2.0f - 1.0f;  // Map x to [-1, 1]
    float py = (p->y / HEIGHT) * 2.0f - 1.0f; // Map y to [-1, 1]

    // Scale the radius to normalized device coordinates
    float scaled_radius = p->radius / WIDTH * 2.0f;

    // Set particle color
    glColor3f(p->r, p->g, p->b);

    // Draw a circle using a triangle fan
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(px, py); // Center of the circle
    int num_segments = 100; // Number of segments to approximate the circle
    for (int i = 0; i <= num_segments; i++) {
        float angle = 2.0f * M_PI * i / num_segments; // Angle for this segment
        float x = px + cos(angle) * scaled_radius;    // X coordinate
        float y = py + sin(angle) * scaled_radius;    // Y coordinate
        glVertex2f(x, y);
    }
    glEnd();
}
