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
    float distance = sqrtf(dx * dx + dy * dy);
    float min_distance = (p1->radius + p2->radius);

    // Add a small tolerance to prevent re-triggering
    return distance < min_distance;
}

void collision_force(Particle* p1, Particle* p2) {
    float dx = p2->x - p1->x;
    float dy = p2->y - p1->y;
    float distance = sqrtf(dx * dx + dy * dy);
    float min_distance = p1->radius + p2->radius;

    if (distance == 0.0f) return; // Prevent division by zero

    if (distance < min_distance) {
        // Normalize the displacement vector
        float nx = dx / distance;
        float ny = dy / distance;

        // Calculate overlap
        float overlap = 0.5f * (min_distance - distance);

        // Displace particles so they're no longer overlapping
        p1->x -= nx * overlap;
        p1->y -= ny * overlap;
        p2->x += nx * overlap;
        p2->y += ny * overlap;

        // Relative velocity
        float rvx = p2->vx - p1->vx;
        float rvy = p2->vy - p1->vy;

        // Relative velocity along the normal
        float velocity_along_normal = rvx * nx + rvy * ny;

        // Do not resolve if particles are separating
        if (velocity_along_normal > 0) return;

        // Calculate restitution
        float restitution = 1.0f; // Perfectly elastic collision

        // Calculate impulse scalar
        float impulse = -(1 + restitution) * velocity_along_normal;
        impulse /= (1 / p1->mass) + (1 / p2->mass);

        // Apply impulse
        float impulse_per_mass1 = impulse / p1->mass;
        float impulse_per_mass2 = impulse / p2->mass;

        p1->vx -= nx * impulse_per_mass1;
        p1->vy -= ny * impulse_per_mass1;
        p2->vx += nx * impulse_per_mass2;
        p2->vy += ny * impulse_per_mass2;
    }
}
