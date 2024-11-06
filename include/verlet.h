#ifndef __VERLET_H__
#define __VERLET_H__
#include "cmath.h"

#define ContainerRadius 6.0f
#define VerletRadius 0.15f

// Structs
typedef struct {
  mfloat_t T[VEC3_SIZE]; // current particle position
  mfloat_t deltaT[VEC3_SIZE]; // previous particle position
  mfloat_t a[VEC3_SIZE]; // particle acceleration
  mfloat_t radius;
} ParticleObject;

struct NodeStruct {
  ParticleObject* val;
  struct NodeStruct* next;
};

typedef struct NodeStruct Node;

// Functions
void updatePos(ParticleObject* object, int size, float dt);
void force(ParticleObject* object, int size);
void collision(ParticleObject* object, int size);
void constraint(ParticleObject* object, int size, mfloat_t* containerPos);
void clearGrid();
void fillGrid(ParticleObject* objects, int size);
void applyGridCollisions(ParticleObject* objects, int size);
void addForce(ParticleObject* objects, int size, mfloat_t* center, float strength);

#endif
