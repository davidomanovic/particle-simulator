// scripts/impulse_simulation.c
#include "window_handler.h"
#include "particle.h"
#include "movement.h"      // <-- use your existing physics helpers
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// simple colored disc (so we can color A/B differently)
static void draw_disc(float cx, float cy, float r) {
    const int N = 48;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= N; ++i) {
        float a = (float)i * 6.28318530718f / (float)N;
        glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
    }
    glEnd();
}

static void free_particles_array(Particle*** arr, int* count) {
    if (arr && *arr) {
        for (int i = 0; i < *count; ++i) {
            if ((*arr)[i]) { destroy_particle((*arr)[i]); (*arr)[i] = NULL; }
        }
        free(*arr); *arr = NULL;
    }
    if (count) *count = 0;
}

// Forward decl from window handler (you added this earlier)
void render_impulse_ui(GLFWwindow* w,
                       char* m1_buf, char* m2_buf,
                       char* v1_buf, char* v2_buf,
                       bool* out_start);

// Entry point. Return true to go back to main menu.
bool simulate_impulse(GLFWwindow* window) {
    bool go_back = false;

    // Menu fields (strings edited by window_handler input)
    char m1_buf[32] = "10";
    char m2_buf[32] = "1000";
    char v1_buf[32] = "300";   // rightwards
    char v2_buf[32] = "0";     // stationary

    bool running = false;

    Particle** particles = NULL;   // [0]=left, [1]=right
    int num_particles = 0;

    // local sim params
    float m1 = 10.f, m2 = 1000.f;
    float vx1 = 0.f,  vx2 = 0.f;   // we drive velocities here; movement.c integrates

    double last = glfwGetTime();
    glDisable(GL_DEPTH_TEST);

    // menu click suppression (same pattern as gas sim)
    int  prevMouseDown = GLFW_RELEASE;
    bool suppress_menu_click = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        window_begin_frame(window);                 // viewport/projection + resize rescale

        double now = glfwGetTime();
        double dt  = now - last; last = now;
        if (dt > 0.05) dt = 0.05;                   // clamp big stalls for stability

        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { go_back = true; break; }

        if (!running) {
            bool start = false;
            render_impulse_ui(window, m1_buf, m2_buf, v1_buf, v2_buf, &start);

            // menu-only "Return to Main Menu"
            {
                int w, h; get_world_size(window, &w, &h);
                glMatrixMode(GL_PROJECTION); glLoadIdentity(); glOrtho(0, w, h, 0, -1, 1);
                glMatrixMode(GL_MODELVIEW);  glLoadIdentity();

                double mx, my; glfwGetCursorPos(window, &mx, &my);
                int md = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
                bool clickEdge = (prevMouseDown == GLFW_RELEASE && md == GLFW_PRESS);
                prevMouseDown = md;

                if (suppress_menu_click) {
                    if (md == GLFW_RELEASE) suppress_menu_click = false;
                } else {
                    if (draw_button(10, 10, 200, 50, "Return to Main Menu", mx, my, clickEdge)) {
                        go_back = true; break;
                    }
                }
            }

            if (start) {
                // parse/clamp
                m1  = (float)strtod(m1_buf, NULL); if (m1 <= 0.f) m1 = 1.f; if (m1 > MAX_MASS) m1 = MAX_MASS;
                m2  = (float)strtod(m2_buf, NULL); if (m2 <= 0.f) m2 = 1.f; if (m2 > MAX_MASS) m2 = MAX_MASS;

                vx1 = (float)strtod(v1_buf, NULL); if (fabsf(vx1) > MAX_SPEED) vx1 = copysignf(MAX_SPEED, vx1);
                vx2 = (float)strtod(v2_buf, NULL); if (fabsf(vx2) > MAX_SPEED) vx2 = copysignf(MAX_SPEED, vx2);

                // Normalize buffers to reflect the clamped numbers (keeps UI consistent)
                snprintf(m1_buf, 32, "%.0f", (double)m1);
                snprintf(m2_buf, 32, "%.0f", (double)m2);
                snprintf(v1_buf, 32, "%.2f", (double)vx1);
                snprintf(v2_buf, 32, "%.2f", (double)vx2);

                int w, h; get_world_size(window, &w, &h);
                const float y = h * 0.5f;

                // radius ~ sqrt(mass) with sanity clamps
                const float r1 = fmaxf(6.f, 3.0f * sqrtf(m1));
                const float r2 = fmaxf(6.f, 3.0f * sqrtf(m2));

                // spawn with non-overlapping separation
                float x1 = (float)w * 0.30f;
                float x2 = (float)w * 0.70f;
                if (x2 - x1 < (r1 + r2 + 4.f)) {
                    float mid = (float)w * 0.5f;
                    x1 = mid - (r1 + 2.f);
                    x2 = mid + (r2 + 2.f);
                }

                free_particles_array(&particles, &num_particles);
                num_particles = 2;
                particles = (Particle**)malloc(2 * sizeof(Particle*));
                particles[0] = create_particle(x1, y, m1, r1);
                particles[1] = create_particle(x2, y, m2, r2);

                // set initial velocities (movement.c uses vx/vy in integrate)
                particles[0]->vx = vx1; particles[0]->vy = 0.f;
                particles[1]->vx = vx2; particles[1]->vy = 0.f;

                // let window handler rescale on resize (scale velocities too)
                window_bind_particles(&particles, &num_particles, /*scale_velocities=*/true);

                running = true;
                suppress_menu_click = true;
                prevMouseDown = GLFW_RELEASE;
            }
        } else {
            int w, h; get_world_size(window, &w, &h);
            if (w <= 0 || h <= 0) { glfwSwapBuffers(window); continue; }

            // Reset → back to impulse menu
            if (handle_reset(window, particles, &running, NULL, &num_particles)) {
                window_unbind_particles();
                free_particles_array(&particles, &num_particles);
                suppress_menu_click = true;
                prevMouseDown = GLFW_RELEASE;
                glfwSwapBuffers(window);
                continue;
            }

            Particle* A = particles[0];
            Particle* B = particles[1];

            // Zero forces (no gravity). If you ever want gravity, add apply_force(...).
            // integrate using your movement.c (semi-implicit Euler, stable with small dt)
            integrate(A, (float)dt);
            integrate(B, (float)dt);

            // collide with walls (perfectly elastic in your helper)
            handle_wall_collision(A, w, h);
            handle_wall_collision(B, w, h);

            // pair collision (your helper resolves overlap + elastic impulse)
            if (check_collision(A, B)) {
                collision_force(A, B);
            }

            // draw with distinct colors
            glColor3f(0.15f, 0.75f, 1.0f);   // A = cyan-ish
            draw_disc(A->x, A->y, A->radius);
            glColor3f(1.0f, 0.35f, 0.2f);   // B = orange-red
            draw_disc(B->x, B->y, B->radius);
        }

        glfwSwapBuffers(window);
    }

    window_unbind_particles();
    free_particles_array(&particles, &num_particles);
    return go_back;
}
