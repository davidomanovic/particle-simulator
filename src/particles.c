#include "particles.h"
#include <stdlib.h>    // For rand()
#include <GL/gl.h>     // For OpenGL functions (glBegin, glVertex, etc.)

#define BOX_SIZE 1.0f  // Half-width of the particle box, so the box spans [-1,1] in each axis

// Initialize particles within a box of size [-BOX_SIZE, BOX_SIZE] in all axes
void initializeParticles(ParticleObject* particles, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < VEC3_SIZE; j++) {
            particles[i].T[j] = ((float) rand() / RAND_MAX) * 2.0f * BOX_SIZE - BOX_SIZE;
            particles[i].deltaT[j] = particles[i].T[j];  // Start with previous position same as current
            particles[i].a[j] = 0.0f;  // No initial acceleration
        }
        particles[i].radius = VerletRadius;  // Set particle radius from your defines
    }
}

// Apply forces, update positions, handle collisions and constraints for particles
void simulateParticles(ParticleObject* particles, int N, float deltaT) {
    float gravityCenter[3] = {0.0f, -5.0f, 0.0f};  // Gravitational center position
    
    // Apply gravitational force and other forces
    force(particles, N);
    
    // Update position using Verlet integration
    updatePos(particles, N, deltaT);
    
    // Handle collisions between particles
    collision(particles, N);
    
    // Constrain particles within bounds (e.g., circular container)
    constraint(particles, N, gravityCenter);
}

// Render particles as points using OpenGL
void renderParticles(ParticleObject* particles, int N) {
    glPointSize(4.0f);  // Set particle point size for visibility
    glBegin(GL_POINTS);
    for (int i = 0; i < N; i++) {
        ParticleObject* p = &particles[i];
        
        // Optional: Set color based on y-position (height)
        float color = 0.5f + 0.5f * (p->T[1] / BOX_SIZE);
        glColor3f(1.0f - color, color, 0.2f);
        
        glVertex2f(p->T[0], p->T[1]);  // Render particle at its position in 2D
    }
    glEnd();
}
