#ifndef PARTICLES_H
#define PARTICLES_H
#include <verlet.h>

#define N_PARTICLES 5 // amount of particles

void initializeParticles(ParticleObject* particles, int N);
void simulateParticles(ParticleObject* particles, int N, float deltaT);
void renderParticles(ParticleObject* particles, int N);

#endif