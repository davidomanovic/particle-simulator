// particle.c
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <particle.h>
#include <math.h>
#include <stdlib.h>

Particle* create_particle(float x, float y, float mass, float radius) {
    Particle* p = (Particle*)malloc(sizeof(Particle));
    if (p) {
        p->x = x; p->y = y;
        p->vx = p->vy = p->ax = p->ay = 0.0f;
        p->radius = radius;
        p->mass = mass;

        // Assign random colors
        p->r = (float)rand() / RAND_MAX; // Random red
        p->g = (float)rand() / RAND_MAX; // Random green
        p->b = (float)rand() / RAND_MAX; // Random blue
    }
    return p;
}


void destroy_particle(Particle* p) {
    free(p);
}