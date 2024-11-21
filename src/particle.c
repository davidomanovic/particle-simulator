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

        // Assign
        p->r = 0; 
        p->g = 0;
        p->b = 255;
    }
    return p;
}


void destroy_particle(Particle* p) {
    free(p);
}