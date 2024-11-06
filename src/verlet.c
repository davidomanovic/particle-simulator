#include <verlet.h>
#include <cmath.h>

#define SQR(x) ((x) * (x))

void updatePos(ParticleObject* object, int size, float dt){
    for (int i = 0; i < size; i++){
       ParticleObject* obj = &object[i];
       for (int j = 0; j < VEC3_SIZE; j++){
        mfloat_t temp = obj -> T[j]; // store current position
        // Verlet integration
        obj->T[j] = 2.0f * obj->T[j] - obj->deltaT[j] * obj->a[j]*SQR(dt);
        obj->deltaT[j] = temp; // update previous position
       }
    }
}

void force(ParticleObject* object, int size){
    for (int i =0; i < size; i++){
        ParticleObject* obj = &object[i];
        vec3(obj->a, 0.0f, -9.81f, 0.0f); // gravity g = -9.81*y
    }
}

void collision(ParticleObject* object, int size){
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            ParticleObject* objA = &object[i];
            ParticleObject* objB = &object[j];
            mfloat_t distSquared = vec3_distance_squared(objA->T, objB->T);

            mfloat_t combinedRadius = objA->radius + objB->radius;
            if (distSquared < SQR(combinedRadius)) {
                // Response for collision
                mfloat_t dist = MSQRT(distSquared);
                mfloat_t overlap = combinedRadius - dist;

                for (int k = 0; k < VEC3_SIZE; k++) {
                    mfloat_t direction = (objA->T[k] - objB->T[k]) / dist;
                    objA->T[k] += direction * (overlap / 2);
                    objB->T[k] -= direction * (overlap / 2);
                }
            }
        }
    }
}

void constraint(ParticleObject* object, int size, mfloat_t* containerPos){
    for (int i = 0; i < size; i++) {
        ParticleObject* obj = &object[i];
        mfloat_t distSquared = vec3_distance_squared(obj->T, containerPos);

        mfloat_t dist = MSQRT(distSquared);
        if (dist > ContainerRadius - obj->radius) {
            mfloat_t overlap = ContainerRadius - obj->radius - dist;
            for (int j = 0; j < VEC3_SIZE; j++) {
                mfloat_t direction = (obj->T[j] - containerPos[j]) / dist;
                obj->T[j] += direction * overlap;
            }
        }
    }
}

void clearGrid(){

}

void fillGrid(ParticleObject* objects, int size){

}

void applyGridCollisions(ParticleObject* objects, int size){

}

void addForce(ParticleObject* objects, int size, mfloat_t* center, float strength){
    for (int i = 0; i < size; i++) {
        ParticleObject* obj = &objects[i];
        mfloat_t distSquared = vec3_distance_squared(obj->T, center);

        if (distSquared > 0.0f) {  // avoid division by zero
            mfloat_t dist = MSQRT(distSquared);
            mfloat_t force_magnitude = strength / distSquared;

            for (int j = 0; j < VEC3_SIZE; j++) {
                mfloat_t direction = (obj->T[j] - center[j]) / dist;
                obj->a[j] += direction * force_magnitude;
            }
        }
    }

}