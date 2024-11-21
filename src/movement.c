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
    float dx = p1->x - p2->x;
    float dy = p1->y - p2->y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance == 0.0f) return; // Prevent division by zero

    float min_distance = p1->radius + p2->radius;
    if (distance < min_distance) {
        // Overlap resolution: separate particles
        float overlap = min_distance - distance;
        float nx = dx / distance;
        float ny = dy / distance;

        // Move particles proportional to their masses
        float p1_factor = p2->mass / (p1->mass + p2->mass);
        float p2_factor = p1->mass / (p1->mass + p2->mass);

        p1->x += nx * overlap * p1_factor;
        p1->y += ny * overlap * p1_factor;

        p2->x -= nx * overlap * p2_factor;
        p2->y -= ny * overlap * p2_factor;

        // Relative velocity
        float rvx = p2->vx - p1->vx;
        float rvy = p2->vy - p1->vy;

        // Relative velocity along the normal
        float velocity_along_normal = rvx * nx + rvy * ny;

        // Do not resolve if particles are separating
        if (velocity_along_normal > 0) return;

        // Calculate impulse scalar
        float restitution = 1.0f; // Perfectly elastic collision
        float impulse = -(1 + restitution) * velocity_along_normal;
        impulse /= (1 / p1->mass) + (1 / p2->mass);

        // Apply impulse
        float impulse_x = impulse * nx;
        float impulse_y = impulse * ny;

        p1->vx -= impulse_x / p1->mass;
        p1->vy -= impulse_y / p1->mass;

        p2->vx += impulse_x / p2->mass;
        p2->vy += impulse_y / p2->mass;
    }
}


