#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <draw_particle.h>
#include <GLFW/glfw3.h>
#include <math.h>

void draw_particle(const Particle* p) {
    // Set particle color
    glColor3f(p->r, p->g, p->b);

    // Draw a circle using a triangle fan
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(p->x, p->y); // Center of the circle
    int num_segments = 16; // Number of segments to approximate the circle
    for (int i = 0; i <= num_segments; i++) {
        float angle = 2.0f * M_PI * i / num_segments; // Angle for this segment
        float x = p->x + cosf(angle) * p->radius;    // X coordinate
        float y = p->y + sinf(angle) * p->radius;    // Y coordinate
        glVertex2f(x, y);
    }
    glEnd();
}
