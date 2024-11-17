// movement.h
#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "particle.h"

void apply_force(Particle* p, float fx, float fy);
void integrate(Particle* p, float dt);

#endif // MOVEMENT_H
