#ifndef UI_H
#define UI_H

void createUI();
void renderUI();
void handleMenuInput(int key);
extern bool gameRunning;
extern bool truckStarted;

// Declare renderText() so it can be used in multiple files without redefining it
void renderText(float x, float y, const char* text);

#endif
