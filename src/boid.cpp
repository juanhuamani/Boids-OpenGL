#include "boid.h"

const int WINDOW_W = 1000;
const int WINDOW_H = 700;

int numBoids = 80;
float neighborRadius = 80.0f;
float separationWeight = 1.5f;
float alignmentWeight = 1.0f;
float cohesionWeight = 1.0f;

float minSpeed = 1.5f;
float maxSpeed = 4.5f;

float separationDistance = 25.0f;

int edgeMode = 0;
float edgeMargin = 50.0f;
float edgeForce = 0.3f;

std::vector<Boid> boids;
float globalTime = 0.0f;
