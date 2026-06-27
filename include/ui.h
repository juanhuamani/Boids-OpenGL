#ifndef UI_H
#define UI_H

extern bool uiPanelOpen;
extern bool simulationPaused;
extern bool showTrails;
extern const float PANEL_W;

void initializeUI();
void drawUI();
bool handleMouse(int button, int state, int x, int y);
bool handleMouseMove(int x, int y);
bool handleMousePassiveMove(int x, int y);

#endif
