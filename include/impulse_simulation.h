// impulse_simulation
#pragma once
#ifndef IMPULSE_SIMULATION_H
#define IMPULSE_SIMULATION_H

#include "window_handler.h"
#include "particle.h"
#include "movement.h"     
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool simulate_impulse(GLFWwindow* window);

#endif // IMPULSE_SIMULATION_H