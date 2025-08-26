#pragma once
#include <stdbool.h>
#include <GLFW/glfw3.h>
#include "particle.h"

#define WIDTH 1000
#define HEIGHT 1000
#define MAX_PARTICLES 1000
#define MAX_MASS   1000.0f
#define MAX_SPEED   500.0f
#define STR_HELPER(x) #x
#define STRINGIFY(x) STR_HELPER(x)

//shared UI state (defined in window_handler.c)
extern bool simulation_active;
extern bool input_active;
extern char input_text[128];

// world projection
void setup_coordinate_system(void);

// menu input
void handle_input(GLFWwindow* w, int key, int scancode, int action, int mods);

// gas simulation menu UI
void render_gas_ui(GLFWwindow* w, char* text_buf, bool* text_active, bool* out_simulation_active, int*  out_num);

// impulse simulation menu UI
void render_impulse_ui(GLFWwindow* w,char* m1_buf, char* m2_buf, char* v1_buf, char* v2_buf, bool* out_start);


// simple button (top-left coords)
bool draw_button(float x, float y, float w, float h,const char* label, double mx, double my, bool clicked);

void draw_label(float x, float y, const char* text);
void draw_label_centered(float cx, float cy, float scale, const char* text);

void handle_char(GLFWwindow* w, unsigned int codepoint);

// RESET during RUNNING sim: MUST NOT free particles. Only flips to menu state.
bool handle_reset(GLFWwindow* w,Particle** particles,bool* simulation_active_ref,char* text_buf,int*  num_particles_ref);

// Current world size (in window coords)
void get_world_size(GLFWwindow* w, int* outW, int* outH);

// Centralized per-frame setup + auto-rescale on resize
void window_begin_frame(GLFWwindow* w);

// Bind/unbind the active particle array so window handler can rescale on resize
void window_bind_particles(Particle*** arr, int* count, bool scale_velocities);
void window_unbind_particles(void);