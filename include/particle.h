// particle.h
#ifndef PARTICLE_H
#define PARTICLE_H

typedef struct {
    float x, y;    // Position
    float vx, vy;  // Velocity
    float ax, ay;  // Acceleration
    float r, g, b; // Color of particle
    float radius;     // Radius of the particle
    float mass;
} Particle;

Particle* create_particle(float x, float y, float mass);
void destroy_particle(Particle* p);

#endif // PARTICLE_H
