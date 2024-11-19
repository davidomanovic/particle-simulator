// movement.h
#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "particle.h"

void apply_force(Particle* p, float fx, float fy);
void integrate(Particle* p, float dt);
int check_collision(const Particle* p1, const Particle* p2);
void collision_force(Particle* p1, Particle* p2, float restitution);

#endif // MOVEMENT_H
