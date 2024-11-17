// particle.c
#include <particle.h>
#include <stdlib.h>

Particle* create_particle(float x, float y, float mass) {
    Particle* p = (Particle*)malloc(sizeof(Particle));
    if (p) {
        p->x = x; p->y = y;
        p->vx = p->vy = p->ax = p->ay = 0.0f;
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
