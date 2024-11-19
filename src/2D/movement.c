// movement.c
#include <movement.h>
#include <math.h>

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

int check_collision(const Particle* p1, const Particle* p2) {
    float dx = p1->x - p2->x;
    float dy = p1->y - p2->y;
    float distance = sqrt(dx * dx + dy * dy);
    float min_distance = (p1->radius + p2->radius);

    // Add a small tolerance to prevent re-triggering
    return distance < min_distance;
}

void collision_force(Particle* p1, Particle* p2) {
    // Calculate the vector between the particles
    float dx = p1->x - p2->x;
    float dy = p1->y - p2->y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance == 0.0f) return; // Prevent division by zero or overlapping at the same position

    // Check if particles are colliding
    float overlap = (p1->radius + p2->radius) - distance;
    if (overlap > 0) {
        p1->vx -= p2->mass/p1->mass * p1->vx * 0.1;
        p2->vx -= p1->mass/p2->mass * p2->vx * 0.1;
    }
}

