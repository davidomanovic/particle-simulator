// menu.c
#include "menu.h"
#include <stdbool.h>
#include <string.h>
#include <GL/gl.h>
#include "stb_easy_font.h"

// --- helpers ---------------------------------------------------------------
static void set_ortho(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);   // top-left UI coords
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void draw_rect(float x, float y, float w, float h, bool filled) {
    glBegin(filled ? GL_QUADS : GL_LINE_LOOP);
    glVertex2f(x, y);       glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);   glVertex2f(x,   y+h);
    glEnd();
}

// centered, scalable text using stb_easy_font
static void draw_text_centered(float cx, float cy, float scale, const char* text) {
    char buf[20000];
    int quads = stb_easy_font_print(0, 0, (char*)text, NULL, buf, sizeof(buf));

    float minX=1e9f,maxX=-1e9f,minY=1e9f,maxY=-1e9f;
    for (int i=0;i<quads*4;i++) {
        float* v = (float*)(buf + i*16);
        if (v[0]<minX) minX=v[0]; if (v[0]>maxX) maxX=v[0];
        if (v[1]<minY) minY=v[1]; if (v[1]>maxY) maxY=v[1];
    }
    float w = (maxX-minX)*scale, h = (maxY-minY)*scale;
    float x = cx - w*0.5f, y = cy - h*0.5f;

    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buf);
    glDrawArrays(GL_QUADS, 0, quads*4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
}

static bool point_in(float px, float py, float x, float y, float w, float h) {
    return (px >= x && px <= x+w && py >= y && py <= y+h);
}

// returns true if this frame produced a click on the button
// returns true if the mouse is over AND this frame is a click edge
static bool draw_button_centered(float cx, float cy, float w, float h,
                                 const char* label, double mx, double my,
                                 bool clickedEdge) {
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
// --- menu -------------------------------------------------------------------
AppState run_menu(GLFWwindow* window, int /*width*/, int /*height*/) {
    // button sizes (in pixels)
    const float bw = 260.f, bh = 70.f, gap = 30.f;

    int prevDown = GLFW_RELEASE;

    while (!glfwWindowShouldClose(window)) {
        int W, H; glfwGetWindowSize(window, &W, &H);
        double mx, my; glfwGetCursorPos(window, &mx, &my);

        int mouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        bool clickedEdge = (prevDown == GLFW_RELEASE && mouseDown == GLFW_PRESS);
        prevDown = mouseDown;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) return STATE_QUIT;

        glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        set_ortho(W, H);

        float cx = W * 0.5f;
        float titleY = H * 0.33f;
        float btn1Y = H * 0.55f;
        float btn2Y = btn1Y + 70.f + 30.f; // bh + gap

        glColor3f(1,1,1);
        draw_text_centered(cx, titleY, 3.0f, "Particle Simulator");

        bool gasClicked =draw_button_centered(cx, btn2Y, 260.f, 70.f, "Gas in a box",     mx, my, clickedEdge);
        bool impulseClicked =draw_button_centered(cx, btn1Y, 260.f, 70.f, "Elastic collisions", mx, my, clickedEdge);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (gasClicked)     return STATE_GAS;
        if (impulseClicked) return STATE_IMPULSE;
    }
    return STATE_QUIT;
}
