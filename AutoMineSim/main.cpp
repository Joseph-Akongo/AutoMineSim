#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <btBulletDynamicsCommon.h>
#include <iostream>
#include <vector>
#include "Simulation.h"
#include "Vehicle.h"
#include "Resource.h"
#include "UI.h"

GLFWwindow* window;
Vehicle* miningTruck;

const GLint WIDTH = 1280, HEIGHT = 720;
bool truckStarted = false;  // Truck won't move until "S" is pressed

void startGame() {
    gameRunning = true;
    std::cout << "Game Started!" << std::endl;
}

void exitGame() {
    glfwSetWindowShouldClose(window, true);
}

std::vector<btVector3> hazards;  // List of placed hazards

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Convert screen coordinates to normalized device coordinates (-1 to 1)
        float ndcX = (xpos / 1280.0f) * 2.0f - 1.0f;
        float ndcY = 1.0f - (ypos / 720.0f) * 2.0f;

        // Convert to world coordinates (assuming 80x80 world size)
        float worldX = ndcX * 40.0f;
        float worldZ = ndcY * 40.0f;

        btVector3 hazardPosition(worldX, 0, worldZ);
        hazards.push_back(hazardPosition);

        std::cout << "Hazard placed at: " << worldX << ", " << worldZ << std::endl;
    }
}

// Draw hazards on screen
void drawHazards() {
    glColor3f(0.5f, 0.5f, 0.5f);  // Gray color for hazards
    for (const auto& hazard : hazards) {
        glPushMatrix();
        glTranslatef(hazard.getX(), hazard.getY(), hazard.getZ());
        glutSolidSphere(1, 10, 10);
        glPopMatrix();
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !gameRunning) {
        gameRunning = true;
        std::cout << "Game Started!\n";
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && gameRunning && !truckStarted) {
        truckStarted = true;
        std::cout << "Truck Started!\n";  // Debug message
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        handleMenuInput(GLFW_KEY_ESCAPE);
    }
}

void setupCamera() {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void updateCamera() {
    if (!miningTruck) return;

    btVector3 position = miningTruck->getBody()->getCenterOfMassPosition();
    glLoadIdentity();
    gluLookAt(position.getX(), position.getY() + 5, position.getZ() + 15,
        position.getX(), position.getY(), position.getZ(),
        0, 1, 0);
}

void drawGround() {
    glColor3f(0.3, 0.8, 0.3);  // Green color
    glBegin(GL_QUADS);
    for (float x = -50; x < 50; x += 5) {
        for (float z = -50; z < 50; z += 5) {
            glVertex3f(x, 0, z);
            glVertex3f(x + 5, 0, z);
            glVertex3f(x + 5, 0, z + 5);
            glVertex3f(x, 0, z + 5);
        }
    }
    glEnd();
}

bool initGLEW() {
    glewExperimental = GL_TRUE;  // Allow newer OpenGL extensions
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        return false;
    }
    return true;
}

bool initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return false;
    }
    window = glfwCreateWindow(1280, 720, "AutoMineSim", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    return true;
}

int main(int argc, char** argv) {
    if (!initGLFW()) return -1;
    if (!initGLEW()) return -1;
    glutInit(&argc, argv);
    setupCamera();

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    initBullet();
    miningTruck = new Vehicle(dynamicsWorld);
    generateResources(dynamicsWorld);
    createUI();

    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);

        if (gameRunning) {
            updatePhysics();
            updateCamera();
            drawGround();

            if (truckStarted) {
                std::cout << "Truck is moving...\n";  // Debug message
                miningTruck->findClosestResource();
                miningTruck->update(hazards);
                miningTruck->draw();
            }

            drawResources();
            drawHazards();
            checkMining(miningTruck);
        }
        else {
            renderUI();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete miningTruck;
    glfwTerminate();
    return 0;
}



