#include <particle.h>
#include <simulate_impulse.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_PARTICLES 2

int main() {

    Particle* particles[NUM_PARTICLES];
    // Initialize particles with specific properties
    particles[0] = create_particle(80.0f, 300.0f, 10.0f, 10.0f); // x, y, radius
    particles[0]->vx = 200.0f;                            // Initial velocity
    particles[0]->vy = 0.0f;

    particles[1] = create_particle(400.0f, 300.0f, 1000.0f, 30.0f); // x, y, radius
    particles[1]->vx = 0.0f;                             // Initial velocity
    particles[1]->vy = 0.0f;

    // Simulate impulse
    simulate_impulse(particles, NUM_PARTICLES);

    // Cleanup
    for (int i = 0; i < NUM_PARTICLES; i++) {
        destroy_particle(particles[i]);
    }
    return 0;
}
