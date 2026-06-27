#ifndef BOID_H
#define BOID_H

#include <cmath>
#include <vector>

const int MAX_TRAIL = 12;

struct Boid {
    float x, y;
    float vx, vy;
    float direction;
    int id;

    float trailX[MAX_TRAIL];
    float trailY[MAX_TRAIL];
    int trailCount;

    void updateDirection() {
        direction = atan2(vy, vx);
    }

    void savePosition() {
        for (int i = MAX_TRAIL - 1; i > 0; i--) {
            trailX[i] = trailX[i - 1];
            trailY[i] = trailY[i - 1];
        }
        trailX[0] = x;
        trailY[0] = y;
        if (trailCount < MAX_TRAIL) trailCount++;
    }
};

extern const int WINDOW_W;
extern const int WINDOW_H;

extern int numBoids;
extern float neighborRadius;
extern float separationWeight;
extern float alignmentWeight;
extern float cohesionWeight;

extern float minSpeed;
extern float maxSpeed;

extern float separationDistance;

extern int edgeMode;
extern float edgeMargin;
extern float edgeForce;

extern std::vector<Boid> boids;
extern bool showTrails;
extern float globalTime;

#endif
