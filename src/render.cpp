#include "render.h"
#include "simulacion.h"
#include "ui.h"
#include <GL/freeglut.h>
#include <cstdio>
#include <cmath>

static void hsvToRgb(float h, float s, float v, float& r, float& g, float& b) {
    int i = static_cast<int>(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
}

void drawTrail(const Boid& b) {
    if (b.trailCount < 2) return;

    float hue = fmod(static_cast<float>(b.id) * 0.073f, 1.0f);
    float r, g, bl;
    hsvToRgb(hue, 0.7f, 0.9f, r, g, bl);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int count = b.trailCount < MAX_TRAIL ? b.trailCount : MAX_TRAIL;

    for (int i = 0; i < count - 1; i++) {
        float dx = b.trailX[i] - b.trailX[i + 1];
        float dy = b.trailY[i] - b.trailY[i + 1];
        if (dx * dx + dy * dy > 10000.0f) {
            continue;
        }

        float alpha = 0.35f * (1.0f - static_cast<float>(i) / count);
        float grosor = 2.5f * (1.0f - static_cast<float>(i) / count);

        glLineWidth(grosor > 0.5f ? grosor : 0.5f);
        glColor4f(r, g, bl, alpha);
        glBegin(GL_LINES);
            glVertex2f(b.trailX[i], b.trailY[i]);
            glVertex2f(b.trailX[i + 1], b.trailY[i + 1]);
        glEnd();
    }

    glLineWidth(1.0f);
    glDisable(GL_BLEND);
}

void drawBoid(const Boid& b) {
    float hue = fmod(static_cast<float>(b.id) * 0.073f, 1.0f);
    float r, g, bl;
    hsvToRgb(hue, 0.8f, 1.0f, r, g, bl);

    float rOscuro, gOscuro, blOscuro;
    hsvToRgb(hue, 0.9f, 0.5f, rOscuro, gOscuro, blOscuro);

    float speed = sqrt(b.vx * b.vx + b.vy * b.vy);
    float phase = globalTime * 8.0f + b.id * 1.3f;
    float flap = sin(phase) * (0.3f + speed * 0.1f);

    float S = 1.0f;

    glPushMatrix();
    glTranslatef(b.x, b.y, 0.0f);
    glRotatef(b.direction * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);

    glColor3f(r * 0.9f, g * 0.9f, bl * 0.9f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 20; i++) {
        float ang = 2.0f * M_PI * i / 20.0f;
        float cx = cos(ang) * 5.0f * S;
        float cy = sin(ang) * 2.2f * S;
        glVertex2f(cx, cy);
    }
    glEnd();

    glColor3f(r, g, bl);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 12; i++) {
        float ang = 2.0f * M_PI * i / 12.0f;
        float cx = 5.5f * S + cos(ang) * 2.0f * S;
        float cy = sin(ang) * 1.8f * S;
        glVertex2f(cx, cy);
    }
    glEnd();

    glColor3f(1.0f, 0.85f, 0.2f);
    glBegin(GL_TRIANGLES);
        glVertex2f(9.5f * S, 0.0f);
        glVertex2f(7.0f * S,  1.0f * S);
        glVertex2f(7.0f * S, -1.0f * S);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glPointSize(2.5f);
    glBegin(GL_POINTS);
        glVertex2f(6.0f * S, 0.6f * S);
    glEnd();

    float wingAngle1 = 0.5f + flap;
    glColor3f(r * 0.7f, g * 0.7f, bl * 0.7f);
    glBegin(GL_TRIANGLES);
        glVertex2f( 2.0f * S,  2.0f * S);
        glVertex2f(-3.0f * S,  (5.0f + wingAngle1 * 8.0f) * S);
        glVertex2f(-4.0f * S,  1.5f * S);
    glEnd();

    glColor3f(r * 0.55f, g * 0.55f, bl * 0.55f);
    glBegin(GL_TRIANGLES);
        glVertex2f( 0.0f * S,  2.2f * S);
        glVertex2f(-5.0f * S,  (4.0f + wingAngle1 * 6.0f) * S);
        glVertex2f(-6.0f * S,  1.8f * S);
    glEnd();

    float wingAngle2 = 0.5f + flap;
    glColor3f(r * 0.7f, g * 0.7f, bl * 0.7f);
    glBegin(GL_TRIANGLES);
        glVertex2f( 2.0f * S, -2.0f * S);
        glVertex2f(-3.0f * S, -(5.0f + wingAngle2 * 8.0f) * S);
        glVertex2f(-4.0f * S, -1.5f * S);
    glEnd();

    glColor3f(r * 0.55f, g * 0.55f, bl * 0.55f);
    glBegin(GL_TRIANGLES);
        glVertex2f( 0.0f * S, -2.2f * S);
        glVertex2f(-5.0f * S, -(4.0f + wingAngle2 * 6.0f) * S);
        glVertex2f(-6.0f * S, -1.8f * S);
    glEnd();

    glColor3f(rOscuro, gOscuro, blOscuro);
    glBegin(GL_TRIANGLES);
        glVertex2f(-4.5f * S,  0.0f);
        glVertex2f(-9.0f * S,  2.5f * S);
        glVertex2f(-8.0f * S,  0.5f * S);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(-4.5f * S,  0.0f);
        glVertex2f(-9.0f * S, -2.5f * S);
        glVertex2f(-8.0f * S, -0.5f * S);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(-4.5f * S,  0.0f);
        glVertex2f(-9.5f * S,  0.8f * S);
        glVertex2f(-9.5f * S, -0.8f * S);
    glEnd();

    glColor3f(rOscuro, gOscuro, blOscuro);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 20; i++) {
        float ang = 2.0f * M_PI * i / 20.0f;
        float cx = cos(ang) * 5.0f * S;
        float cy = sin(ang) * 2.2f * S;
        glVertex2f(cx, cy);
    }
    glEnd();

    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (showTrails) {
        for (const auto& b : boids) {
            drawTrail(b);
        }
    }

    for (const auto& b : boids) {
        drawBoid(b);
    }

    drawUI();

    glutSwapBuffers();
}

void timer(int value) {
    updateBoids();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}
