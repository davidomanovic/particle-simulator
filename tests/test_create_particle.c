#include "unity.h"
#include "particle.h"

void setUp(void) {
    // Setup code (called before each test)
}

void tearDown(void) {
    // Teardown code (called after each test)
}

void test_create_particle(void) {
    Particle* p = create_particle(10.0f, 20.0f, 1.0f);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_FLOAT(10.0f, p->x);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, p->y);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, p->mass);
    destroy_particle(p);
}

void test_particle_motion(void) {
    Particle* p = create_particle(10.0f, 20.0f, 1.0f);
    p->vx = 5.0f;
    p->vy = 0.0f;

    integrate(p, 2.0f); // Simulate for 2 seconds
    TEST_ASSERT_EQUAL_FLOAT(20.0f, p->x); // 10 + (5 * 2)
    TEST_ASSERT_EQUAL_FLOAT(20.0f, p->y);
    destroy_particle(p);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_particle);
    RUN_TEST(test_particle_motion);
    return UNITY_END();
}
