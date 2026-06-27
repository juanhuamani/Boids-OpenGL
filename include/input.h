#ifndef INPUT_H
#define INPUT_H

void keyboard(unsigned char key, int x, int y);
void mouseCallback(int button, int state, int x, int y);
void motionCallback(int x, int y);
void passiveMotionCallback(int x, int y);

#endif
