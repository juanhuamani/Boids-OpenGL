#include "input.h"
#include "boid.h"
#include "simulacion.h"
#include "ui.h"
#include <GL/freeglut.h>
#include <cstdlib>

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case '+': case '=':
            numBoids += 10;
            initializeBoids(numBoids);
            break;
        case '-': case '_':
            numBoids -= 10;
            if (numBoids < 10) numBoids = 10;
            initializeBoids(numBoids);
            break;

        case 'q': case 'Q':
            neighborRadius += 5.0f;
            if (neighborRadius > 300.0f) neighborRadius = 300.0f;
            break;
        case 'a': case 'A':
            neighborRadius -= 5.0f;
            if (neighborRadius < 10.0f) neighborRadius = 10.0f;
            break;

        case 'w': case 'W':
            separationWeight += 0.1f;
            if (separationWeight > 5.0f) separationWeight = 5.0f;
            break;
        case 's': case 'S':
            separationWeight -= 0.1f;
            if (separationWeight < 0.0f) separationWeight = 0.0f;
            break;

        case 'e': case 'E':
            alignmentWeight += 0.1f;
            if (alignmentWeight > 5.0f) alignmentWeight = 5.0f;
            break;
        case 'd': case 'D':
            alignmentWeight -= 0.1f;
            if (alignmentWeight < 0.0f) alignmentWeight = 0.0f;
            break;

        case 'r': case 'R':
            cohesionWeight += 0.1f;
            if (cohesionWeight > 5.0f) cohesionWeight = 5.0f;
            break;
        case 'f': case 'F':
            cohesionWeight -= 0.1f;
            if (cohesionWeight < 0.0f) cohesionWeight = 0.0f;
            break;

        case 't': case 'T':
            minSpeed += 0.5f;
            if (minSpeed > maxSpeed - 0.5f) minSpeed = maxSpeed - 0.5f;
            break;
        case 'g': case 'G':
            minSpeed -= 0.5f;
            if (minSpeed < 0.5f) minSpeed = 0.5f;
            break;

        case 'y': case 'Y':
            maxSpeed += 0.5f;
            if (maxSpeed > 15.0f) maxSpeed = 15.0f;
            break;
        case 'h': case 'H':
            maxSpeed -= 0.5f;
            if (maxSpeed < minSpeed + 0.5f) maxSpeed = minSpeed + 0.5f;
            break;

        case 'b': case 'B':
            edgeMode = (edgeMode + 1) % 2;
            break;
        case ' ':
            initializeBoids(numBoids);
            break;
        case 'i': case 'I':
            uiPanelOpen = !uiPanelOpen;
            break;
        case 27:
            exit(0);
            break;
    }
}

void mouseCallback(int button, int state, int x, int y) {
    if (handleMouse(button, state, x, y)) {
        glutPostRedisplay();
        return;
    }
}

void motionCallback(int x, int y) {
    if (handleMouseMove(x, y)) {
        glutPostRedisplay();
        return;
    }
}

void passiveMotionCallback(int x, int y) {
    if (handleMousePassiveMove(x, y)) {
        glutPostRedisplay();
        return;
    }
}
