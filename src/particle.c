// particle.c
#include <particle.h>
#include <stdlib.h>

Particle* create_particle(float x, float y, float mass) {
    Particle* p = (Particle*)malloc(sizeof(Particle));
    if (p) {
        p->x = x; p->y = y;
        p->vx = p->vy = p->ax = p->ay = 0.0f;
        p->mass = mass;
    }
    return p;
}

void destroy_particle(Particle* p) {
    free(p);
}
