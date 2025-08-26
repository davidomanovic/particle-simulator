#pragma once
#ifndef GAS_SIMULATION_H
#define GAS_SIMULATION_H

#define STB_EASY_FONT_IMPLEMENTATION
#include "window_handler.h"
#include "particle.h"
#include "draw_particle.h"
#include "movement.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdlib.h>

bool simulate_gas(GLFWwindow* window);
#endif // GAS_SIMULATION_H
