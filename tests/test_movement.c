#include "unity.h"
#include "movement.h"

void setUp(void) {}
void tearDown(void) {}

void test_apply_force(void) {
    Particle p = { .x = 0, .y = 0, .vx = 0, .vy = 0, .mass = 2.0f };
    apply_force(&p, 10.0f, 0.0f); // Apply a force of 10 N to the right
    TEST_ASSERT_EQUAL_FLOAT(5.0f, p.ax); // F = ma => 10 = 2 * a => a = 5
}

void test_integrate(void) {
    Particle p = { .x = 0, .y = 0, .vx = 0, .vy = 0, .mass = 1.0f };
    apply_force(&p, 10.0f, 0.0f); // Apply a force
    integrate(&p, 2.0f);          // Simulate for 2 seconds
    TEST_ASSERT_EQUAL_FLOAT(20.0f, p.x); // x = 0.5 * a * t^2 = 20
    TEST_ASSERT_EQUAL_FLOAT(0.0f, p.y);  // No vertical movement
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_apply_force);
    RUN_TEST(test_integrate);
    return UNITY_END();
}
