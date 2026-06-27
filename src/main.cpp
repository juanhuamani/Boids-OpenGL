#include <GL/freeglut.h>
#include <ctime>
#include <cstdlib>

#include "boid.h"
#include "simulacion.h"
#include "render.h"
#include "input.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(WINDOW_W, WINDOW_H);
    glutCreateWindow("Simulacion Boids - Lab 10");

    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_W, 0, WINDOW_H);
    glMatrixMode(GL_MODELVIEW);

    srand(static_cast<unsigned>(time(nullptr)));

    initializeBoids(numBoids);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);
    glutPassiveMotionFunc(passiveMotionCallback);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();

    return 0;
}
