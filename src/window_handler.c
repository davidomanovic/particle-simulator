#include "window_handler.h"
#include <GL/gl.h>          // opengl32 on Windows
#include <stdio.h>
#include "particle.h"     
#include <string.h>
#include <stdlib.h>
#include "stb_easy_font.h"  

// ---------- Single definitions for shared state ----------
bool simulation_active = false;
bool input_active      = false;
char input_text[128]   = "";
static double g_max_msg_until = 0.0;
static double g_hint_until = 0.0;
static char   g_hint_text[64] = "";

// world-size tracking
static int g_prevW = -1, g_prevH = -1;

static struct {
    Particle*** arr;
    int*  count;
    bool  scale_vel;
} g_bound = { NULL, NULL, false };


// ---------- Internal helpers ----------
static void set_ui_top_left(GLFWwindow* w, int* outW, int* outH) {
    int W, H;
    glfwGetWindowSize(w, &W, &H);   // <<< window size, not framebuffer
    if (outW) *outW = W;
    if (outH) *outH = H;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, W, H, 0, -1, 1);     // top-left origin
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static bool buf_has_char(const char* s, char c){
    for (; *s; ++s) if (*s == c) return true;
    return false;
}


static void draw_rect(float x, float y, float w, float h, int filled) {
    glBegin(filled ? GL_QUADS : GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);
    glVertex2f(x, y+h);
    glEnd();
}

void draw_label(float x, float y, const char* text) {
    char buffer[16384];
    int quads = stb_easy_font_print(x, y, (char*)text, NULL, buffer, (int)sizeof(buffer));
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

static void draw_text_centered(float cx, float cy, float scale, const char* text) {
    char buf[20000];
    int quads = stb_easy_font_print(0, 0, (char*)text, NULL, buf, sizeof(buf));

    float minX=1e9f,maxX=-1e9f,minY=1e9f,maxY=-1e9f;
    for (int i=0; i<quads*4; ++i) {
        float* v = (float*)(buf + i*16);
        if (v[0] < minX) minX = v[0];
        if (v[0] > maxX) maxX = v[0];
        if (v[1] < minY) minY = v[1];
        if (v[1] > maxY) maxY = v[1];
    }
    float w = (maxX-minX)*scale, h = (maxY-minY)*scale;
    float x = cx - w*0.5f,       y = cy - h*0.5f;

    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buf);
    glDrawArrays(GL_QUADS, 0, quads*4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
}

static bool point_in(double px, double py, float x, float y, float w, float h) {
    return (px >= x && px <= x+w && py >= y && py <= y+h);
}

// ---------- Public API impls ----------

void get_world_size(GLFWwindow* w, int* outW, int* outH) {
    int ww = 1000, wh = 1000;
    if (w) glfwGetWindowSize(w, &ww, &wh);
    if (outW) *outW = ww;
    if (outH) *outH = wh;
}

void window_bind_particles(Particle*** arr, int* count, bool scale_velocities) {
    g_bound.arr  = arr;
    g_bound.count = count;
    g_bound.scale_vel = scale_velocities;
}

void window_unbind_particles(void) {
    g_bound.arr = NULL;
    g_bound.count = NULL;
    g_bound.scale_vel = false;
}

void draw_label_centered(float cx, float cy, float scale, const char* text) {
    char buffer[16384];
    int quads = stb_easy_font_print(0, 0, (char*)text, NULL, buffer, sizeof(buffer));

    // Compute width & height in "stb font units" (each quad is 8x12 approx)
    float minX = 1e9, maxX = -1e9, minY = 1e9, maxY = -1e9;
    for (int i = 0; i < quads*4; ++i) {
        float* v = (float*)(buffer + i*16);
        if (v[0] < minX) minX = v[0];
        if (v[0] > maxX) maxX = v[0];
        if (v[1] < minY) minY = v[1];
        if (v[1] > maxY) maxY = v[1];
    }
    float w = (maxX - minX) * scale;
    float h = (maxY - minY) * scale;

    float offsetX = cx - w*0.5f;
    float offsetY = cy - h*0.5f;

    // Transform
    glPushMatrix();
    glTranslatef(offsetX, offsetY, 0);
    glScalef(scale, scale, 1);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, quads*4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
}

// returns true if the mouse is over AND this frame is a click edge
static bool draw_button_centered(float cx, float cy, float w, float h,const char* label, double mx, double my,bool clickedEdge) {
    float x = cx - w*0.5f;
    float y = cy - h*0.5f;
    bool over = (mx >= x && mx <= x+w && my >= y && my <= y+h);

    glColor3f(over ? 0.30f : 0.20f, 0.40f, 0.90f);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x+w, y);
    glVertex2f(x+w, y+h); glVertex2f(x, y+h);
    glEnd();
    glColor3f(1,1,1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y); glVertex2f(x+w, y);
    glVertex2f(x+w, y+h); glVertex2f(x, y+h);
    glEnd();

    // centered label
    draw_text_centered(cx, cy, 1.5f, label);

    return over && clickedEdge;
}


void window_begin_frame(GLFWwindow* w) {
    // viewport = framebuffer size (HiDPI-correct)
    int fbW = 1000, fbH = 1000;
    if (w) glfwGetFramebufferSize(w, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);

    // projection = window size (cursor coords)
    int ww = 1000, wh = 1000;
    if (w) glfwGetWindowSize(w, &ww, &wh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, ww, 0, wh, -1, 1);   // bottom-left world
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // if window size changed, rescale bound particles
    if (g_prevW > 0 && g_prevH > 0 && (ww != g_prevW || wh != g_prevH)) {
        if (g_bound.arr && *g_bound.arr && g_bound.count && *g_bound.count > 0) {
            float sx = (float)ww / (float)g_prevW;
            float sy = (float)wh / (float)g_prevH;
            int n = *g_bound.count;
            Particle** a = *g_bound.arr;
            for (int i = 0; i < n; ++i) {
                Particle* p = a[i];
                if (!p) continue;
                p->x *= sx;  p->y *= sy;
                if (g_bound.scale_vel) { p->vx *= sx; p->vy *= sy; }
                // clamp inside new bounds
                if (p->x < p->radius) p->x = p->radius;
                if (p->y < p->radius) p->y = p->radius;
                if (p->x > ww - p->radius) p->x = ww - p->radius;
                if (p->y > wh - p->radius) p->y = wh - p->radius;
            }
        }
    }
    g_prevW = ww; g_prevH = wh;
}

void setup_coordinate_system(void) {
    GLFWwindow* w = glfwGetCurrentContext();

    // Use framebuffer for the viewport (correct on HiDPI), window size for world units
    int fbW = 1000, fbH = 1000;
    if (w) glfwGetFramebufferSize(w, &fbW, &fbH);

    int worldW = 1000, worldH = 1000;
    if (w) glfwGetWindowSize(w, &worldW, &worldH);

    glViewport(0, 0, fbW, fbH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Bottom-left origin; world units track the window (so particles fill the resized area)
    glOrtho(0, worldW, 0, worldH, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void handle_input(GLFWwindow* w, int key, int scancode, int action, int mods) {
    (void)w; (void)scancode; (void)mods;
    if (!input_active) return;
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

    size_t len = strlen(input_text);

    // Backspace
    if (key == GLFW_KEY_BACKSPACE) {
        if (len > 0) input_text[len-1] = '\0';
        return;
    }
    // Enter ends editing focus
    if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
        input_active = false;
        return;
    }

    // Also handle keypad minus/decimal as a fallback (char callback covers most cases)
    if (key == GLFW_KEY_KP_SUBTRACT) {
        if (len==0 && len+1 < sizeof(input_text)) { input_text[len]='-'; input_text[len+1]='\0'; }
        return;
    }
    if (key == GLFW_KEY_KP_DECIMAL) {
        if (!buf_has_char(input_text, '.') && len+1 < sizeof(input_text)) { input_text[len]='.'; input_text[len+1]='\0'; }
        return;
    }
}

void handle_char(GLFWwindow* w, unsigned int codepoint) {
    (void)w;
    if (!input_active) return;

    char c = 0;
    if (codepoint < 128) c = (char)codepoint;      // ASCII only
    if (c == ',') c = '.';                          // Norwegian keypad decimal → '.'

    size_t len = strlen(input_text);
    if (len + 1 >= sizeof(input_text)) return;

    // Allow digits always
    if (c >= '0' && c <= '9') {
        input_text[len] = c; input_text[len+1] = '\0'; return;
    }
    // Allow exactly one leading '-' at position 0
    if (c == '-') {
        if (len == 0) { input_text[0] = '-'; input_text[1] = '\0'; }
        return;
    }
    // Allow a single '.'
    if (c == '.') {
        if (!buf_has_char(input_text, '.')) { input_text[len] = '.'; input_text[len+1] = '\0'; }
        return;
    }
    // Ignore everything else
}


bool draw_button(float x, float y, float w, float h,
                 const char* label, double mx, double my, bool clicked) {
    bool hover = point_in(mx, my, x, y, w, h);

    glColor3f(hover ? 0.30f : 0.20f, 0.40f, 0.90f);
    draw_rect(x, y, w, h, 1);
    glColor3f(1,1,1);
    draw_rect(x, y, w, h, 0);

    // Label
    glColor3f(1,1,1);
    draw_label(x + 10, y + 22, label);

    return hover && clicked;
}

void render_gas_ui(GLFWwindow* w,
               char*       text_buf,
               bool*       text_active,
               bool*       out_simulation_active,
               int*        out_num)
{
    // --- layout sizes ---
    const float fieldW = 420.f, fieldH = 56.f;
    const float btnW   = 56.f,  btnH   = 56.f;
    const float gap    = 14.f;
    const float startW = 200.f, startH = 60.f;

    // --- UI space (TOP-LEFT, window coords) ---
    int W, H;
    glfwGetWindowSize(w, &W, &H);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); glOrtho(0, W, H, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();

    // --- center anchors ---
    float cx = W * 0.5f;
    float cy = H * 0.5f;
    
    const char* maxMsg = "Max " STRINGIFY(MAX_PARTICLES) " particles!"; // helper below

    // positions
    float fieldX = cx - fieldW * 0.5f;
    float fieldY = cy - fieldH * 0.5f;
    float minusX = fieldX + fieldW + gap;
    float minusY = fieldY;
    float plusX  = minusX + btnW + gap;
    float plusY  = fieldY;
    float startX = cx - startW * 0.5f;
    float startY = fieldY + fieldH + 2*gap;

    // mouse / click edge
    static int prevDown = GLFW_RELEASE;
    double mx, my; glfwGetCursorPos(w, &mx, &my);
    int down = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT);
    bool clicked = (prevDown == GLFW_RELEASE && down == GLFW_PRESS);
    prevDown = down;

    // title
    draw_label(cx - 140, fieldY - 60, "Gas Simulation - Particle count");

    // input box
    bool overInput = (mx >= fieldX && mx <= fieldX+fieldW && my >= fieldY && my <= fieldY+fieldH);
    glColor3f(overInput ? 0.25f : 0.18f, 0.18f, 0.18f);  // fill
    glBegin(GL_QUADS); glVertex2f(fieldX,fieldY); glVertex2f(fieldX+fieldW,fieldY);
                      glVertex2f(fieldX+fieldW,fieldY+fieldH); glVertex2f(fieldX,fieldY+fieldH); glEnd();
    glColor3f(1,1,1);  // border
    glBegin(GL_LINE_LOOP); glVertex2f(fieldX,fieldY); glVertex2f(fieldX+fieldW,fieldY);
                          glVertex2f(fieldX+fieldW,fieldY+fieldH); glVertex2f(fieldX,fieldY+fieldH); glEnd();

    if (clicked && overInput) { *text_active = true;  input_active = true;  glfwSetKeyCallback(w, handle_input); }
    else if (clicked && !overInput) { *text_active = false; input_active = false; }

    {
        char* buf = (text_buf && text_buf[0]) ? text_buf : input_text;
        if (buf[0]) {
            long v = strtol(buf, NULL, 10);
            if (v > MAX_PARTICLES) {
                v = MAX_PARTICLES;
                snprintf(buf, 128, "%ld", v);
                g_max_msg_until = glfwGetTime() + 2.0; // show message for 2 seconds
            }
        }
    }
    
    // show typed text
    char shown[160];
    snprintf(shown, sizeof(shown), "%s%s", text_buf ? text_buf : input_text, (*text_active ? "_" : ""));
    draw_label(fieldX + 12, fieldY + 34, shown);

    // +/- buttons
    if (draw_button(minusX, minusY, btnW, btnH, "-", mx, my, clicked)) {
        long v = strtol(text_buf, NULL, 10);
        if (v <= 1) v = 1; else v /= 2;
        if (v > MAX_PARTICLES) { v = MAX_PARTICLES; g_max_msg_until = glfwGetTime() + 2.0; }
        snprintf(text_buf, 128, "%ld", v);
    }

    if (draw_button(plusX, plusY, btnW, btnH, "+", mx, my, clicked)) {
        long v = strtol(text_buf, NULL, 10);
        if (v < 1) v = 1; else if (v < MAX_PARTICLES) v *= 2;
        if (v > MAX_PARTICLES) { v = MAX_PARTICLES; g_max_msg_until = glfwGetTime() + 2.0; }
        snprintf(text_buf, 128, "%ld", v);
    }
    
    if (glfwGetTime() < g_max_msg_until) {
    // place the message just above the input
    draw_label(cx - 110, fieldY - 30, maxMsg);
    }

    // Start button
    bool doStart = draw_button(startX, startY, startW, startH, "Start", mx, my, clicked);
    if (*text_active && (glfwGetKey(w, GLFW_KEY_ENTER)==GLFW_PRESS || glfwGetKey(w, GLFW_KEY_KP_ENTER)==GLFW_PRESS))
        doStart = true;

    if (doStart) {
        long v = strtol(text_buf, NULL, 10);
        if (v < 1) v = 1;
        if (v > MAX_PARTICLES) { v = MAX_PARTICLES; g_max_msg_until = glfwGetTime() + 2.0; }
        if (out_num) *out_num = (int)v;
        if (out_simulation_active) *out_simulation_active = true;
        *text_active = false; input_active = false;
    }
}


bool handle_reset(GLFWwindow* w,Particle** particles,bool* simulation_active_ref,char* text_buf,int*  num_particles_ref) {
    (void)particles; (void)num_particles_ref;

    int W, H; set_ui_top_left(w, &W, &H);

    static int prevDown = GLFW_RELEASE;
    double mx, my; glfwGetCursorPos(w, &mx, &my);
    int down = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT);
    bool clicked = (prevDown == GLFW_RELEASE && down == GLFW_PRESS);
    prevDown = down;

    if (draw_button(10, 10, 140, 46, "Reset", mx, my, clicked)) {
        if (simulation_active_ref) *simulation_active_ref = false; // back to gas menu
        if (text_buf) text_buf[0] = '\0';
        return true;  // caller frees and stays inside simulate_gas()
    }
    return false;
}

void render_impulse_ui(GLFWwindow* w,
                       char* m1_buf,
                       char* m2_buf,
                       char* v1_buf,
                       char* v2_buf,
                       bool* out_start)
{
    enum { UI_FIELD_CAP = 32 };      // your field buffers are char[32]
    const size_t UI_COPY_MAX = UI_FIELD_CAP - 1;

    int W, H; glfwGetWindowSize(w, &W, &H);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); glOrtho(0, W, H, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();

    const float fieldW = 160.f, fieldH = 44.f;
    const float rowGap = 18.f;
    const float startW = 240.f, startH = 56.f;

    float cx = W * 0.5f;
    float cy = H * 0.5f;
    float firstY = cy - (2*fieldH + 1.5f*rowGap);

    struct Item { const char* label; char* buf; } items[4] = {
        { "Mass m1",     m1_buf },
        { "Mass m2",     m2_buf },
        { "Velocity v1", v1_buf },
        { "Velocity v2", v2_buf },
    };

    static int prevDown = GLFW_RELEASE;
    double mx, my; glfwGetCursorPos(w, &mx, &my);
    int down = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT);
    bool clicked = (prevDown == GLFW_RELEASE && down == GLFW_PRESS);
    prevDown = down;

    static int focus = -1;

    // title + (optional) hint
    draw_label(cx - 150, firstY - 28, "Impulse Simulation Parameters");
    if (glfwGetTime() < g_hint_until && g_hint_text[0]) {
        draw_label(cx - 120, firstY - 48, g_hint_text);
    }

    for (int i = 0; i < 4; ++i) {
        float x = cx - fieldW * 0.5f;
        float y = firstY + i * (fieldH + rowGap);
        bool over = (mx >= x && mx <= x + fieldW && my >= y && my <= y + fieldH);

        glColor3f((over || focus == i) ? 0.25f : 0.18f, 0.18f, 0.18f);
        glBegin(GL_QUADS);
            glVertex2f(x, y); glVertex2f(x + fieldW, y);
            glVertex2f(x + fieldW, y + fieldH); glVertex2f(x, y + fieldH);
        glEnd();
        glColor3f(1,1,1);
        glBegin(GL_LINE_LOOP);
            glVertex2f(x, y); glVertex2f(x + fieldW, y);
            glVertex2f(x + fieldW, y + fieldH); glVertex2f(x, y + fieldH);
        glEnd();

        draw_label(x - 110, y + 28, items[i].label);

        if (clicked && over) {
            focus = i;
            input_active = true;
            glfwSetKeyCallback(w, handle_input);
            glfwSetCharCallback(w, handle_char);

            // seed edit buffer from this field (bounded)
            if (items[i].buf) {
                size_t n = strnlen(items[i].buf, UI_COPY_MAX);
                memcpy(input_text, items[i].buf, n);
                input_text[n] = '\0';
            } else {
                input_text[0] = '\0';
            }
        }

        // Only focused field mirrors input_text (bounded copy)
        if (focus == i && items[i].buf) {
            size_t n = strnlen(input_text, UI_COPY_MAX);
            memcpy(items[i].buf, input_text, n);
            items[i].buf[n] = '\0';

            // LIVE CLAMP + hint
            if (i == 0 || i == 1) { // masses
                double v = strtod(items[i].buf, NULL);
                if (v > MAX_MASS) {
                    v = MAX_MASS;
                    int nn = snprintf(items[i].buf, UI_FIELD_CAP, "%.0f", v);
                    (void)nn;
                    snprintf(g_hint_text, sizeof(g_hint_text), "Max mass %.0f!", (double)MAX_MASS);
                    g_hint_until = glfwGetTime() + 2.0;
                } else if (v <= 0.0 && items[i].buf[0]) {
                    v = 1.0; // keep masses positive
                    int nn = snprintf(items[i].buf, UI_FIELD_CAP, "%.0f", v);
                    (void)nn;
                }
            } else { // velocities (keep sign, clamp magnitude)
                double v = strtod(items[i].buf, NULL);
                if (fabs(v) > MAX_SPEED) {
                    double s = (v < 0 ? -1.0 : 1.0);
                    v = s * MAX_SPEED;
                    int nn = snprintf(items[i].buf, UI_FIELD_CAP, "%.2f", v);
                    (void)nn;
                    snprintf(g_hint_text, sizeof(g_hint_text), "Max |velocity| %.0f!", (double)MAX_SPEED);
                    g_hint_until = glfwGetTime() + 2.0;
                }
            }
        }

        // show text (caret on focused)
        char shown[UI_FIELD_CAP + 2];
        if (items[i].buf && items[i].buf[0]) {
            size_t n = strnlen(items[i].buf, UI_COPY_MAX);
            memcpy(shown, items[i].buf, n);
            if (focus == i && n + 1 < sizeof(shown)) { shown[n] = '_'; shown[n+1] = '\0'; }
            else { shown[n] = '\0'; }
        } else {
            if (focus == i) { shown[0] = '_'; shown[1] = '\0'; }
            else { shown[0] = '\0'; }
        }
        draw_label(x + 10, y + 28, shown);
    }

    // Start button below last field
    float lastY  = firstY + 3 * (fieldH + rowGap);
    float startX = cx - startW * 0.5f;
    float startY = lastY + fieldH + 1.5f * rowGap;

    bool doStart = draw_button(startX, startY, startW, startH, "Start Simulation", mx, my, clicked);
    if (focus != -1 &&
        (glfwGetKey(w, GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetKey(w, GLFW_KEY_KP_ENTER) == GLFW_PRESS))
        doStart = true;

    if (out_start) *out_start = doStart;
    if (doStart) { focus = -1; input_active = false; }
}

