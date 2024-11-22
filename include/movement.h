// movement.h
#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "particle.h"

void apply_force(Particle* p, float fx, float fy);
void integrate(Particle* p, float dt);
int check_collision(const Particle* p1, const Particle* p2);
void collision_force(Particle* p1, Particle* p2);
void handle_wall_collision(Particle* p, int width, int height);
int is_overlapping(Particle** particles, float x, float y, int index, int radius);

#endif // MOVEMENT_H
