#include "ui.h"
#include "boid.h"
#include "simulacion.h"
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstring>

bool uiPanelOpen = true;
bool simulationPaused = false;
bool showTrails = true;

extern const float PANEL_W = 280.0f;
const float TAB_W = 24.0f;
const float TAB_H = 50.0f;

struct Color {
    float r, g, b, a;
};

const Color COLOR_BG           = {0.06f, 0.07f, 0.13f, 0.85f};
const Color COLOR_BORDER       = {0.0f, 0.75f, 1.0f, 0.3f};
const Color COLOR_ACCENT       = {0.0f, 0.85f, 1.0f, 1.0f};
const Color COLOR_ACCENT_HOVER = {0.0f, 1.0f, 0.8f, 1.0f};
const Color COLOR_TEXT         = {0.95f, 0.96f, 1.0f, 1.0f};
const Color COLOR_TEXT_MUTED   = {0.55f, 0.62f, 0.72f, 0.9f};
const Color COLOR_BTN_BG       = {0.12f, 0.16f, 0.28f, 0.9f};
const Color COLOR_BTN_HOVER    = {0.0f, 0.75f, 1.0f, 0.2f};
const Color COLOR_SLIDER_BG    = {0.09f, 0.10f, 0.18f, 1.0f};

static bool tabHover = false;

struct Button {
    float x, y, w, h;
    const char* label;
    bool hover;
    int id;
};

struct Slider {
    float x, y, w, h;
    const char* label;
    float minVal, maxVal;
    float* floatPtr;
    int* intPtr;
    const char* format;
    bool hover;
    bool dragging;
};

struct Checkbox {
    float x, y, w, h;
    const char* label;
    bool* valPtr;
    bool hover;
};

struct SegmentedControl {
    float x, y, w, h;
    const char* label;
    const char* option0;
    const char* option1;
    int* valPtr;
    bool hover0;
    bool hover1;
};

struct RangeSlider {
    float x, y, w, h;
    const char* label;
    float minVal, maxVal;
    float* floatMinPtr;
    float* floatMaxPtr;
    const char* format;
    bool hoverMin;
    bool hoverMax;
    bool draggingMin;
    bool draggingMax;
};

static Button buttons[] = {
    {20.0f, 560.0f, 110.0f, 32.0f, "Pause", false, 1},
    {150.0f, 560.0f, 110.0f, 32.0f, "Reset", false, 2}
};

static Slider sliders[] = {
    {20.0f, 490.0f, 240.0f, 8.0f, "Boids Count", 1.0f, 300.0f, nullptr, &numBoids, "%d", false, false},
    {20.0f, 430.0f, 240.0f, 8.0f, "Neighbor Radius", 10.0f, 300.0f, &neighborRadius, nullptr, "%.0f px", false, false},
    {20.0f, 370.0f, 240.0f, 8.0f, "Separation Force", 0.0f, 5.0f, &separationWeight, nullptr, "%.2f", false, false},
    {20.0f, 310.0f, 240.0f, 8.0f, "Alignment Force", 0.0f, 5.0f, &alignmentWeight, nullptr, "%.2f", false, false},
    {20.0f, 250.0f, 240.0f, 8.0f, "Cohesion Force", 0.0f, 5.0f, &cohesionWeight, nullptr, "%.2f", false, false}
};

static RangeSlider rangeSlider = {
    20.0f, 160.0f, 240.0f, 8.0f, "Speed Range", 0.5f, 20.0f, &minSpeed, &maxSpeed, "%.1f - %.1f px/f", false, false, false, false
};

static Checkbox checkboxes[] = {
    {20.0f, 45.0f, 18.0f, 18.0f, "Show Trails", &showTrails, false}
};

static SegmentedControl segments[] = {
    {20.0f, 80.0f, 240.0f, 26.0f, "Edge Mode", "Bounce", "Toroidal", &edgeMode, false, false}
};

static void drawRect(float x, float y, float w, float h, Color c) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
    glDisable(GL_BLEND);
}

static void drawRectBorder(float x, float y, float w, float h, Color c, float thickness = 1.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(c.r, c.g, c.b, c.a);
    glLineWidth(thickness);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
}

static void drawLine(float x1, float y1, float x2, float y2, Color c, float thickness = 1.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(c.r, c.g, c.b, c.a);
    glLineWidth(thickness);
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
}

static void drawCircle(float cx, float cy, float radius, Color c, bool fill = true) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(c.r, c.g, c.b, c.a);
    if (fill) {
        glBegin(GL_POLYGON);
    } else {
        glBegin(GL_LINE_LOOP);
    }
    for (int i = 0; i < 30; i++) {
        float theta = 2.0f * 3.1415926f * static_cast<float>(i) / 30.0f;
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
    glDisable(GL_BLEND);
}

static void drawText(float x, float y, const char* text, void* font, Color c) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(c.r, c.g, c.b, c.a);
    glRasterPos2f(x, y);
    for (const char* ch = text; *ch != '\0'; ch++) {
        glutBitmapCharacter(font, *ch);
    }
    glDisable(GL_BLEND);
}

static bool pointInRect(float px, float py, float rx, float ry, float rw, float rh) {
    return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}

void initializeUI() {
}

static void drawButton(const Button& b) {
    const char* displayLabel = b.label;
    if (b.id == 1) {
        displayLabel = simulationPaused ? "Resume" : "Pause";
    }

    Color bgCol = COLOR_BTN_BG;
    Color borderCol = COLOR_BORDER;
    Color textCol = COLOR_TEXT;

    if (b.hover) {
        bgCol = COLOR_BTN_HOVER;
        borderCol = COLOR_ACCENT_HOVER;
    }

    drawRect(b.x, b.y, b.w, b.h, bgCol);
    drawRectBorder(b.x, b.y, b.w, b.h, borderCol, 1.5f);

    int len = strlen(displayLabel);
    float textW = len * 7.0f;
    float tx = b.x + (b.w - textW) * 0.5f;
    float ty = b.y + (b.h - 9.0f) * 0.5f;
    drawText(tx, ty, displayLabel, GLUT_BITMAP_HELVETICA_12, textCol);
}

static void drawSlider(const Slider& s) {
    float val = 0.0f;
    if (s.floatPtr) val = *(s.floatPtr);
    else if (s.intPtr) val = static_cast<float>(*(s.intPtr));

    float t = (val - s.minVal) / (s.maxVal - s.minVal);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    drawRect(s.x, s.y, s.w, s.h, COLOR_SLIDER_BG);
    drawRectBorder(s.x, s.y, s.w, s.h, {1.0f, 1.0f, 1.0f, 0.05f});

    Color fillCol = s.hover || s.dragging ? COLOR_ACCENT_HOVER : COLOR_ACCENT;
    fillCol.a = 0.85f;
    drawRect(s.x, s.y, t * s.w, s.h, fillCol);

    float hx = s.x + t * s.w;
    float hy = s.y + s.h * 0.5f;
    float radius = s.dragging ? 7.0f : (s.hover ? 6.0f : 5.0f);
    Color handleCol = s.dragging || s.hover ? COLOR_ACCENT_HOVER : COLOR_TEXT;
    drawCircle(hx, hy, radius, handleCol, true);
    drawCircle(hx, hy, radius + 1.0f, {0.0f, 0.0f, 0.0f, 0.4f}, false);

    char valStr[32];
    if (s.floatPtr) {
        snprintf(valStr, sizeof(valStr), s.format, *(s.floatPtr));
    } else if (s.intPtr) {
        snprintf(valStr, sizeof(valStr), s.format, *(s.intPtr));
    }

    drawText(s.x, s.y + s.h + 6.0f, s.label, GLUT_BITMAP_HELVETICA_12, COLOR_TEXT_MUTED);

    int len = strlen(valStr);
    float valX = s.x + s.w - (len * 6.5f);
    drawText(valX, s.y + s.h + 6.0f, valStr, GLUT_BITMAP_HELVETICA_12, COLOR_ACCENT);
}

static void drawCheckbox(const Checkbox& cb) {
    Color borderCol = cb.hover ? COLOR_ACCENT_HOVER : COLOR_BORDER;

    drawRect(cb.x, cb.y, cb.w, cb.h, COLOR_SLIDER_BG);
    drawRectBorder(cb.x, cb.y, cb.w, cb.h, borderCol, 1.5f);

    if (*(cb.valPtr)) {
        drawRect(cb.x + 3.0f, cb.y + 3.0f, cb.w - 6.0f, cb.h - 6.0f, COLOR_ACCENT);
    }

    drawText(cb.x + cb.w + 10.0f, cb.y + (cb.h - 9.0f) * 0.5f, cb.label, GLUT_BITMAP_HELVETICA_12, COLOR_TEXT);
}

static void drawSegmentedControl(const SegmentedControl& sc) {
    drawText(sc.x, sc.y + sc.h + 6.0f, sc.label, GLUT_BITMAP_HELVETICA_12, COLOR_TEXT_MUTED);

    float halfW = sc.w * 0.5f;

    Color bg0 = (*(sc.valPtr) == 0) ? COLOR_BTN_BG : COLOR_SLIDER_BG;
    if (*(sc.valPtr) == 0) {
        bg0.r += 0.05f; bg0.g += 0.05f; bg0.b += 0.05f;
    }
    Color border0 = (*(sc.valPtr) == 0) ? COLOR_ACCENT : COLOR_BORDER;
    if (sc.hover0 && *(sc.valPtr) != 0) {
        bg0 = COLOR_BTN_HOVER;
        border0 = COLOR_ACCENT_HOVER;
    }

    drawRect(sc.x, sc.y, halfW, sc.h, bg0);
    drawRectBorder(sc.x, sc.y, halfW, sc.h, border0, 1.5f);

    float tx0 = sc.x + (halfW - strlen(sc.option0) * 7.0f) * 0.5f;
    float ty0 = sc.y + (sc.h - 9.0f) * 0.5f;
    drawText(tx0, ty0, sc.option0, GLUT_BITMAP_HELVETICA_12, (*(sc.valPtr) == 0) ? COLOR_TEXT : COLOR_TEXT_MUTED);

    Color bg1 = (*(sc.valPtr) == 1) ? COLOR_BTN_BG : COLOR_SLIDER_BG;
    if (*(sc.valPtr) == 1) {
        bg1.r += 0.05f; bg1.g += 0.05f; bg1.b += 0.05f;
    }
    Color border1 = (*(sc.valPtr) == 1) ? COLOR_ACCENT : COLOR_BORDER;
    if (sc.hover1 && *(sc.valPtr) != 1) {
        bg1 = COLOR_BTN_HOVER;
        border1 = COLOR_ACCENT_HOVER;
    }

    drawRect(sc.x + halfW, sc.y, halfW, sc.h, bg1);
    drawRectBorder(sc.x + halfW, sc.y, halfW, sc.h, border1, 1.5f);

    float tx1 = sc.x + halfW + (halfW - strlen(sc.option1) * 7.0f) * 0.5f;
    float ty1 = sc.y + (sc.h - 9.0f) * 0.5f;
    drawText(tx1, ty1, sc.option1, GLUT_BITMAP_HELVETICA_12, (*(sc.valPtr) == 1) ? COLOR_TEXT : COLOR_TEXT_MUTED);
}

static void drawRangeSlider(const RangeSlider& rs) {
    float tMin = (*rs.floatMinPtr - rs.minVal) / (rs.maxVal - rs.minVal);
    float tMax = (*rs.floatMaxPtr - rs.minVal) / (rs.maxVal - rs.minVal);
    if (tMin < 0.0f) tMin = 0.0f;
    if (tMin > 1.0f) tMin = 1.0f;
    if (tMax < 0.0f) tMax = 0.0f;
    if (tMax > 1.0f) tMax = 1.0f;

    float hxMin = rs.x + tMin * rs.w;
    float hxMax = rs.x + tMax * rs.w;
    float hy = rs.y + rs.h * 0.5f;

    drawRect(rs.x, rs.y, rs.w, rs.h, COLOR_SLIDER_BG);
    drawRectBorder(rs.x, rs.y, rs.w, rs.h, {1.0f, 1.0f, 1.0f, 0.05f});

    Color fillCol = rs.hoverMin || rs.hoverMax || rs.draggingMin || rs.draggingMax ? COLOR_ACCENT_HOVER : COLOR_ACCENT;
    fillCol.a = 0.85f;
    drawRect(hxMin, rs.y, hxMax - hxMin, rs.h, fillCol);

    float rMin = rs.draggingMin ? 7.0f : (rs.hoverMin ? 6.0f : 5.0f);
    Color colMin = rs.draggingMin || rs.hoverMin ? COLOR_ACCENT_HOVER : COLOR_TEXT;
    drawCircle(hxMin, hy, rMin, colMin, true);
    drawCircle(hxMin, hy, rMin + 1.0f, {0.0f, 0.0f, 0.0f, 0.4f}, false);

    float rMax = rs.draggingMax ? 7.0f : (rs.hoverMax ? 6.0f : 5.0f);
    Color colMax = rs.draggingMax || rs.hoverMax ? COLOR_ACCENT_HOVER : COLOR_TEXT;
    drawCircle(hxMax, hy, rMax, colMax, true);
    drawCircle(hxMax, hy, rMax + 1.0f, {0.0f, 0.0f, 0.0f, 0.4f}, false);

    char valStr[64];
    snprintf(valStr, sizeof(valStr), rs.format, *rs.floatMinPtr, *rs.floatMaxPtr);

    drawText(rs.x, rs.y + rs.h + 6.0f, rs.label, GLUT_BITMAP_HELVETICA_12, COLOR_TEXT_MUTED);

    int len = strlen(valStr);
    float valX = rs.x + rs.w - (len * 6.5f);
    drawText(valX, rs.y + rs.h + 6.0f, valStr, GLUT_BITMAP_HELVETICA_12, COLOR_ACCENT);
}

void drawUI() {
    float xOffset = uiPanelOpen ? 0.0f : -PANEL_W;

    drawRect(xOffset, 0.0f, PANEL_W, WINDOW_H, COLOR_BG);

    drawLine(xOffset + PANEL_W, 0.0f, xOffset + PANEL_W, WINDOW_H, COLOR_BORDER, 2.0f);

    glPushMatrix();
    glTranslatef(xOffset, 0.0f, 0.0f);

    drawText(20.0f, 660.0f, "BOIDS SIMULATOR", GLUT_BITMAP_HELVETICA_18, COLOR_ACCENT);
    drawLine(20.0f, 628.0f, 260.0f, 628.0f, {0.0f, 0.75f, 1.0f, 0.2f}, 1.0f);

    for (auto& btn : buttons) {
        drawButton(btn);
    }
    for (auto& s : sliders) {
        drawSlider(s);
    }
    drawRangeSlider(rangeSlider);
    for (auto& sc : segments) {
        drawSegmentedControl(sc);
    }
    for (auto& cb : checkboxes) {
        drawCheckbox(cb);
    }

    glPopMatrix();

    float tabX = uiPanelOpen ? PANEL_W : 0.0f;
    float tabY = WINDOW_H * 0.5f - TAB_H * 0.5f;

    Color tabBG = tabHover ? COLOR_BTN_BG : COLOR_BG;
    if (tabHover) {
        tabBG.a = 0.95f;
    }
    drawRect(tabX, tabY, TAB_W, TAB_H, tabBG);

    Color tabBorder = tabHover ? COLOR_ACCENT_HOVER : COLOR_BORDER;
    drawRectBorder(tabX, tabY, TAB_W, TAB_H, tabBorder, 1.5f);

    const char* arrow = uiPanelOpen ? "<" : ">";
    float tx = tabX + (TAB_W - 7.0f) * 0.5f;
    float ty = tabY + (TAB_H - 12.0f) * 0.5f;
    drawText(tx, ty, arrow, GLUT_BITMAP_HELVETICA_12, tabHover ? COLOR_ACCENT_HOVER : COLOR_TEXT);
}

bool handleMouse(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON) return false;

    float actualW = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
    float actualH = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
    if (actualW < 1.0f) actualW = 1.0f;
    if (actualH < 1.0f) actualH = 1.0f;

    float mx = static_cast<float>(x) * (static_cast<float>(WINDOW_W) / actualW);
    float my = (actualH - static_cast<float>(y)) * (static_cast<float>(WINDOW_H) / actualH);

    float tabX = uiPanelOpen ? PANEL_W : 0.0f;
    float tabY = WINDOW_H * 0.5f - TAB_H * 0.5f;

    if (pointInRect(mx, my, tabX, tabY, TAB_W, TAB_H)) {
        if (state == GLUT_DOWN) {
            uiPanelOpen = !uiPanelOpen;
            tabHover = false;
        }
        return true;
    }

    if (state == GLUT_UP) {
        bool wasDragging = false;
        for (auto& s : sliders) {
            if (s.dragging) {
                s.dragging = false;
                wasDragging = true;
            }
        }
        if (rangeSlider.draggingMin) {
            rangeSlider.draggingMin = false;
            wasDragging = true;
        }
        if (rangeSlider.draggingMax) {
            rangeSlider.draggingMax = false;
            wasDragging = true;
        }
        if (wasDragging) return true;
    }

    if (!uiPanelOpen) return false;

    if (mx > PANEL_W) return false;

    if (state == GLUT_DOWN) {
        for (auto& btn : buttons) {
            if (pointInRect(mx, my, btn.x, btn.y, btn.w, btn.h)) {
                if (btn.id == 1) {
                    simulationPaused = !simulationPaused;
                } else if (btn.id == 2) {
                    initializeBoids(numBoids);
                }
                return true;
            }
        }

        for (auto& s : sliders) {
            if (pointInRect(mx, my, s.x - 5.0f, s.y - 4.0f, s.w + 10.0f, s.h + 8.0f)) {
                s.dragging = true;
                float t = (mx - s.x) / s.w;
                if (t < 0.0f) t = 0.0f;
                if (t > 1.0f) t = 1.0f;
                float newVal = s.minVal + t * (s.maxVal - s.minVal);
                if (s.floatPtr) {
                    *(s.floatPtr) = newVal;
                } else if (s.intPtr) {
                    int prevVal = *(s.intPtr);
                    *(s.intPtr) = static_cast<int>(newVal);
                    if (*(s.intPtr) != prevVal) {
                        initializeBoids(*(s.intPtr));
                    }
                }
                return true;
            }
        }

        if (pointInRect(mx, my, rangeSlider.x - 5.0f, rangeSlider.y - 6.0f, rangeSlider.w + 10.0f, rangeSlider.h + 12.0f)) {
            float tMin = (*rangeSlider.floatMinPtr - rangeSlider.minVal) / (rangeSlider.maxVal - rangeSlider.minVal);
            float tMax = (*rangeSlider.floatMaxPtr - rangeSlider.minVal) / (rangeSlider.maxVal - rangeSlider.minVal);
            float hxMin = rangeSlider.x + tMin * rangeSlider.w;
            float hxMax = rangeSlider.x + tMax * rangeSlider.w;

            float distToMin = std::abs(mx - hxMin);
            float distToMax = std::abs(mx - hxMax);

            if (distToMin < distToMax) {
                rangeSlider.draggingMin = true;
                rangeSlider.draggingMax = false;
            } else {
                rangeSlider.draggingMax = true;
                rangeSlider.draggingMin = false;
            }

            float t = (mx - rangeSlider.x) / rangeSlider.w;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            float newVal = rangeSlider.minVal + t * (rangeSlider.maxVal - rangeSlider.minVal);

            if (rangeSlider.draggingMin) {
                *rangeSlider.floatMinPtr = newVal;
                if (*rangeSlider.floatMinPtr > *rangeSlider.floatMaxPtr - 0.5f) {
                    *rangeSlider.floatMinPtr = *rangeSlider.floatMaxPtr - 0.5f;
                }
            } else {
                *rangeSlider.floatMaxPtr = newVal;
                if (*rangeSlider.floatMaxPtr < *rangeSlider.floatMinPtr + 0.5f) {
                    *rangeSlider.floatMaxPtr = *rangeSlider.floatMinPtr + 0.5f;
                }
            }
            return true;
        }

        for (auto& cb : checkboxes) {
            float hitW = cb.w + 180.0f;
            if (pointInRect(mx, my, cb.x, cb.y, hitW, cb.h)) {
                *(cb.valPtr) = !(*(cb.valPtr));
                return true;
            }
        }

        for (auto& sc : segments) {
            float halfW = sc.w * 0.5f;
            if (pointInRect(mx, my, sc.x, sc.y, halfW, sc.h)) {
                *(sc.valPtr) = 0;
                return true;
            } else if (pointInRect(mx, my, sc.x + halfW, sc.y, halfW, sc.h)) {
                *(sc.valPtr) = 1;
                return true;
            }
        }
    }

    return true;
}

bool handleMouseMove(int x, int y) {
    float actualW = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
    float actualH = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
    if (actualW < 1.0f) actualW = 1.0f;
    if (actualH < 1.0f) actualH = 1.0f;

    float mx = static_cast<float>(x) * (static_cast<float>(WINDOW_W) / actualW);
    float my = (actualH - static_cast<float>(y)) * (static_cast<float>(WINDOW_H) / actualH);

    bool handled = false;

    for (auto& s : sliders) {
        if (s.dragging) {
            float t = (mx - s.x) / s.w;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            float newVal = s.minVal + t * (s.maxVal - s.minVal);
            if (s.floatPtr) {
                *(s.floatPtr) = newVal;
            } else if (s.intPtr) {
                int prevVal = *(s.intPtr);
                *(s.intPtr) = static_cast<int>(newVal);
                if (*(s.intPtr) != prevVal) {
                    initializeBoids(*(s.intPtr));
                }
            }
            handled = true;
        }
    }

    if (rangeSlider.draggingMin) {
        float t = (mx - rangeSlider.x) / rangeSlider.w;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        float newVal = rangeSlider.minVal + t * (rangeSlider.maxVal - rangeSlider.minVal);
        *rangeSlider.floatMinPtr = newVal;
        if (*rangeSlider.floatMinPtr > *rangeSlider.floatMaxPtr - 0.5f) {
            *rangeSlider.floatMinPtr = *rangeSlider.floatMaxPtr - 0.5f;
        }
        handled = true;
    } else if (rangeSlider.draggingMax) {
        float t = (mx - rangeSlider.x) / rangeSlider.w;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        float newVal = rangeSlider.minVal + t * (rangeSlider.maxVal - rangeSlider.minVal);
        *rangeSlider.floatMaxPtr = newVal;
        if (*rangeSlider.floatMaxPtr < *rangeSlider.floatMinPtr + 0.5f) {
            *rangeSlider.floatMaxPtr = *rangeSlider.floatMinPtr + 0.5f;
        }
        handled = true;
    }

    if (handled) return true;

    if (!uiPanelOpen) {
        float tabX = uiPanelOpen ? PANEL_W : 0.0f;
        float tabY = WINDOW_H * 0.5f - TAB_H * 0.5f;
        return pointInRect(mx, my, tabX, tabY, TAB_W, TAB_H);
    }

    return (mx <= PANEL_W);
}

bool handleMousePassiveMove(int x, int y) {
    float actualW = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
    float actualH = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
    if (actualW < 1.0f) actualW = 1.0f;
    if (actualH < 1.0f) actualH = 1.0f;

    float mx = static_cast<float>(x) * (static_cast<float>(WINDOW_W) / actualW);
    float my = (actualH - static_cast<float>(y)) * (static_cast<float>(WINDOW_H) / actualH);

    float tabX = uiPanelOpen ? PANEL_W : 0.0f;
    float tabY = WINDOW_H * 0.5f - TAB_H * 0.5f;
    tabHover = pointInRect(mx, my, tabX, tabY, TAB_W, TAB_H);

    if (!uiPanelOpen) {
        for (auto& btn : buttons) btn.hover = false;
        for (auto& s : sliders) s.hover = false;
        rangeSlider.hoverMin = false;
        rangeSlider.hoverMax = false;
        for (auto& cb : checkboxes) cb.hover = false;
        for (auto& sc : segments) { sc.hover0 = false; sc.hover1 = false; }
        return tabHover;
    }

    for (auto& btn : buttons) {
        btn.hover = pointInRect(mx, my, btn.x, btn.y, btn.w, btn.h);
    }

    for (auto& s : sliders) {
        s.hover = pointInRect(mx, my, s.x - 5.0f, s.y - 4.0f, s.w + 10.0f, s.h + 8.0f);
    }

    {
        float tMin = (*rangeSlider.floatMinPtr - rangeSlider.minVal) / (rangeSlider.maxVal - rangeSlider.minVal);
        float tMax = (*rangeSlider.floatMaxPtr - rangeSlider.minVal) / (rangeSlider.maxVal - rangeSlider.minVal);
        float hxMin = rangeSlider.x + tMin * rangeSlider.w;
        float hxMax = rangeSlider.x + tMax * rangeSlider.w;
        float hy = rangeSlider.y + rangeSlider.h * 0.5f;

        rangeSlider.hoverMin = (std::abs(mx - hxMin) <= 8.0f && std::abs(my - hy) <= 8.0f);
        rangeSlider.hoverMax = (std::abs(mx - hxMax) <= 8.0f && std::abs(my - hy) <= 8.0f);
    }

    for (auto& cb : checkboxes) {
        float hitW = cb.w + 180.0f;
        cb.hover = pointInRect(mx, my, cb.x, cb.y, hitW, cb.h);
    }

    for (auto& sc : segments) {
        float halfW = sc.w * 0.5f;
        sc.hover0 = pointInRect(mx, my, sc.x, sc.y, halfW, sc.h);
        sc.hover1 = pointInRect(mx, my, sc.x + halfW, sc.y, halfW, sc.h);
    }

    return (mx <= PANEL_W) || tabHover;
}
