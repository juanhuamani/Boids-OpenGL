#include "simulacion.h"
#include "ui.h"
#include <cstdlib>
#include <cmath>
#include <vector>

void initializeBoids(int n) {
    boids.clear();
    float minX = uiPanelOpen ? PANEL_W : 0.0f;
    float simW = static_cast<float>(WINDOW_W) - minX;

    for (int i = 0; i < n; i++) {
        Boid b;
        b.x = minX + 50.0f + static_cast<float>(rand() % static_cast<int>(simW - 100.0f));
        b.y = 50.0f + static_cast<float>(rand() % (WINDOW_H - 100));
        b.id = i;
        b.trailCount = 0;

        for (int t = 0; t < MAX_TRAIL; t++) {
            b.trailX[t] = b.x;
            b.trailY[t] = b.y;
        }

        float angulo = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
        float velocidad = minSpeed + static_cast<float>(rand()) / RAND_MAX * (maxSpeed - minSpeed);
        b.vx = cos(angulo) * velocidad;
        b.vy = sin(angulo) * velocidad;
        b.updateDirection();

        boids.push_back(b);
    }
}

void limitSpeed(Boid& b) {
    float speed = sqrt(b.vx * b.vx + b.vy * b.vy);
    if (speed < 0.0001f) return;

    if (speed > maxSpeed) {
        b.vx = (b.vx / speed) * maxSpeed;
        b.vy = (b.vy / speed) * maxSpeed;
    } else if (speed < minSpeed) {
        b.vx = (b.vx / speed) * minSpeed;
        b.vy = (b.vy / speed) * minSpeed;
    }
}

void applyBoundaries(Boid& b) {
    float minX = uiPanelOpen ? PANEL_W : 0.0f;
    float maxX = static_cast<float>(WINDOW_W);
    float minY = 0.0f;
    float maxY = static_cast<float>(WINDOW_H);

    if (edgeMode == 0) {
        if (b.x < minX + edgeMargin)
            b.vx += edgeForce;
        if (b.x > maxX - edgeMargin)
            b.vx -= edgeForce;
        if (b.y < minY + edgeMargin)
            b.vy += edgeForce;
        if (b.y > maxY - edgeMargin)
            b.vy -= edgeForce;

        if (b.x < minX) {
            b.x = minX;
            b.vx = std::abs(b.vx);
        } else if (b.x > maxX) {
            b.x = maxX;
            b.vx = -std::abs(b.vx);
        }
        if (b.y < minY) {
            b.y = minY;
            b.vy = std::abs(b.vy);
        } else if (b.y > maxY) {
            b.y = maxY;
            b.vy = -std::abs(b.vy);
        }
    } else {
        if (b.x < minX)         b.x += (maxX - minX);
        if (b.x > maxX)         b.x -= (maxX - minX);
        if (b.y < minY)         b.y += (maxY - minY);
        if (b.y > maxY)         b.y -= (maxY - minY);
    }
}

void calculateDistance(const Boid& a, const Boid& b, float& dx, float& dy, float& dist) {
    dx = b.x - a.x;
    dy = b.y - a.y;

    if (edgeMode == 1) {
        float minX = uiPanelOpen ? PANEL_W : 0.0f;
        float simW = static_cast<float>(WINDOW_W) - minX;
        float simH = static_cast<float>(WINDOW_H);

        if (dx >  simW * 0.5f) dx -= simW;
        if (dx < -simW * 0.5f) dx += simW;
        if (dy >  simH * 0.5f) dy -= simH;
        if (dy < -simH * 0.5f) dy += simH;
    }

    dist = sqrt(dx * dx + dy * dy);
}

void ruleSeparation(int i, float& fx, float& fy) {
    fx = 0.0f;
    fy = 0.0f;
    int neighbors = 0;

    for (int j = 0; j < static_cast<int>(boids.size()); j++) {
        if (i == j) continue;

        float dx, dy, dist;
        calculateDistance(boids[i], boids[j], dx, dy, dist);

        if (dist < separationDistance && dist > 0.001f) {
            fx -= dx / dist;
            fy -= dy / dist;
            neighbors++;
        }
    }

    if (neighbors > 0) {
        fx /= neighbors;
        fy /= neighbors;
    }
}

void ruleAlignment(int i, float& fx, float& fy) {
    fx = 0.0f;
    fy = 0.0f;
    float avgVx = 0.0f, avgVy = 0.0f;
    int neighbors = 0;

    for (int j = 0; j < static_cast<int>(boids.size()); j++) {
        if (i == j) continue;

        float dx, dy, dist;
        calculateDistance(boids[i], boids[j], dx, dy, dist);

        if (dist < neighborRadius) {
            avgVx += boids[j].vx;
            avgVy += boids[j].vy;
            neighbors++;
        }
    }

    if (neighbors > 0) {
        avgVx /= neighbors;
        avgVy /= neighbors;
        fx = avgVx - boids[i].vx;
        fy = avgVy - boids[i].vy;
    }
}

void ruleCohesion(int i, float& fx, float& fy) {
    fx = 0.0f;
    fy = 0.0f;
    float centerX = 0.0f, centerY = 0.0f;
    int neighbors = 0;

    for (int j = 0; j < static_cast<int>(boids.size()); j++) {
        if (i == j) continue;

        float dx, dy, dist;
        calculateDistance(boids[i], boids[j], dx, dy, dist);

        if (dist < neighborRadius) {
            centerX += boids[i].x + dx;
            centerY += boids[i].y + dy;
            neighbors++;
        }
    }

    if (neighbors > 0) {
        centerX /= neighbors;
        centerY /= neighbors;
        fx = centerX - boids[i].x;
        fy = centerY - boids[i].y;

        float mag = sqrt(fx * fx + fy * fy);
        if (mag > 0.001f) {
            fx /= mag;
            fy /= mag;
        }
    }
}

void updateBoids() {
    if (simulationPaused) return;
    int n = static_cast<int>(boids.size());

    for (int i = 0; i < n; i++) {
        float sepX, sepY;
        ruleSeparation(i, sepX, sepY);

        float aliX, aliY;
        ruleAlignment(i, aliX, aliY);

        float cohX, cohY;
        ruleCohesion(i, cohX, cohY);

        boids[i].vx += sepX * separationWeight
                     + aliX * alignmentWeight * 0.05f
                     + cohX * cohesionWeight  * 0.05f;

        boids[i].vy += sepY * separationWeight
                     + aliY * alignmentWeight * 0.05f
                     + cohY * cohesionWeight  * 0.05f;

        limitSpeed(boids[i]);
        applyBoundaries(boids[i]);

        boids[i].savePosition();

        boids[i].x += boids[i].vx;
        boids[i].y += boids[i].vy;

        boids[i].updateDirection();
    }

    globalTime += 0.05f;
}
