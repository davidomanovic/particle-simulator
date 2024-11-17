// simulation.c
#include <simulation.h>
#include <movement.h>
#include <stdio.h>

void run_simulation(float duration, float timestep, float x, float y, float z) {
    // Example: single particle simulation
    Particle* p = create_particle(x, y, z);
    for (float t = 0; t < duration; t += timestep) {
        apply_force(p, 0.0f, -9.8f); // Apply gravity
        integrate(p, timestep);
        printf("Time: %f, Position: (%f, %f)\n", t, p->x, p->y);
    }
    destroy_particle(p);
}
