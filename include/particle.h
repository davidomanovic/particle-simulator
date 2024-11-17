// particle.h
#ifndef PARTICLE_H
#define PARTICLE_H

typedef struct {
    float x, y;       // Position in 3D space
    float previous_x, previous_y; // Previous position for Verlet integration
    float vx, vy;    // Velocity in 3D space
    float ax, ay;    // Acceleration in 3D space
    float mass;          // Mass of the particle
    float radius;        // Radius of the particle
    float r, g, b;       // Color (RGB)
} Particle;

// Particle functions
Particle* create_particle(float x, float y, float mass);
void destroy_particle(Particle* p);

#endif // PARTICLE_H

