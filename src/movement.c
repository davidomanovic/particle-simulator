// movement.c
#include <movement.h>

void apply_force(Particle* p, float fx, float fy) {
    p->ax += fx / p->mass;
    p->ay += fy / p->mass;
}

void integrate(Particle* p, float dt) {
    p->vx += p->ax * dt;
    p->vy += p->ay * dt;
    p->x += p->vx * dt;
    p->y += p->vy * dt;
    p->ax = p->ay = 0.0f;  // Reset acceleration
}
