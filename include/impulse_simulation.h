// impulse_simulation
#pragma once
#ifndef IMPULSE_SIMULATION_H
#define IMPULSE_SIMULATION_H

#include <GLFW/glfw3.h>
#include <particle.h>
#include <movement.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <draw_particle.h>

bool simulate_impulse(GLFWwindow* window);

#endif // IMPULSE_SIMULATION_H