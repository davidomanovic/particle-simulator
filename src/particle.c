// particle.c
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <particle.h>
#include <movement.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

Particle* create_particle(float x, float y, float mass, float radius) {
    Particle* p = (Particle*)malloc(sizeof(Particle));
    if (p) {
        p->x = x; p->y = y;
        p->vx = p->vy = p->ax = p->ay = 0.0f;
        p->radius = radius;
        p->mass = mass;

        // Assign
        p->r = 0; 
        p->g = 0;
        p->b = 255;
    }
    return p;
}


// Initialize particles with random positions and velocities inside a window
void initialize_particles(Particle*** particles, int num_particles, int radius, int width, int height) {
    *particles = (Particle**)malloc(num_particles * sizeof(Particle*)); // Allocate particle array


    srand((unsigned int)time(NULL)); // Seed random number generator

    for (int i = 0; i < num_particles; i++) {
        float x, y;

        // Ensure particles do not overlap
        do {
            x = radius + ((float)rand() / RAND_MAX) * (width - 2 * radius);
            y = radius + ((float)rand() / RAND_MAX) * (height - 2 * radius);
        } while (is_overlapping(*particles, x, y, i, radius));

        // Generate random velocity
        float angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI; // Random direction
        float speed = 100.0f + ((float)rand() / RAND_MAX) * 100.0f; // Random speed
        float vx = speed * cosf(angle);
        float vy = speed * sinf(angle);

        // Create and initialize the particle
        (*particles)[i] = create_particle(x, y, 1.0f, radius); // Mass = 1.0, customizable
        (*particles)[i]->vx = vx;
        (*particles)[i]->vy = vy;

        // Set color (red for visualization, normalized to [0, 1])
        (*particles)[i]->r = 1.0f; // Red
        (*particles)[i]->g = 0.0f; // Green
        (*particles)[i]->b = 0.0f; // Blue
    }
}



void destroy_particle(Particle* p) {
    free(p);
}