#include "UI.h"
#include "Vehicle.h"
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <iostream>

bool gameRunning = false;

void createUI() {
    std::cout << "UI Initialized. Waiting for user input..." << std::endl;
}

extern bool gameRunning;
bool gameOver = false;
extern bool truckStarted;

void renderText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
}

void renderUI() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);

    if (!gameRunning) {
        renderText(350, 400, "Press 'S' to Start Game");
    }
    else if (gameOver) {
        renderText(350, 400, "Game Over! All resources collected");
    }
    else if (!truckStarted) {
        renderText(300, 400, "Press 'E' to Start Truck");
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Fix: Update game state based on input
void handleMenuInput(int key) {
    if (key == GLFW_KEY_S) {
        gameRunning = true;
        std::cout << "Game Started!" << std::endl;
    }
    if (key == GLFW_KEY_ESCAPE) {
        std::cout << "Exiting game..." << std::endl;
        glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
    }
}

extern std::vector<btVector3> hazards;  // List of placed hazards

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);  // Get window size dynamically

        float worldX = ((xpos / width) * 80.0f) - 40.0f;
        float worldZ = ((ypos / height) * 80.0f) - 40.0f;

        hazards.push_back(btVector3(worldX, 1, worldZ));

        std::cout << "Mouse Click at (" << xpos << ", " << ypos << ") -> Hazard at ("
            << worldX << ", " << worldZ << ")" << std::endl;
    }
}

void initUI(GLFWwindow* window) {
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
}


