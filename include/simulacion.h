#ifndef SIMULACION_H
#define SIMULACION_H

#include "boid.h"

void initializeBoids(int n);
void limitSpeed(Boid& b);
void applyBoundaries(Boid& b);

void calculateDistance(const Boid& a, const Boid& b, float& dx, float& dy, float& dist);

void ruleSeparation(int i, float& fx, float& fy);
void ruleAlignment(int i, float& fx, float& fy);
void ruleCohesion(int i, float& fx, float& fy);

void updateBoids();

#endif
