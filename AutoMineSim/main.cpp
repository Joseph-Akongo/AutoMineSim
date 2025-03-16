#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <btBulletDynamicsCommon.h>
#include <iostream>
#include "Simulation.h"
#include "Vehicle.h"

GLFWwindow* window;
Vehicle* miningTruck;

// Window settings
const GLint WIDTH = 800, HEIGHT = 600;

// Handle key inputs
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        miningTruck->applyThrottle(500);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        miningTruck->applyThrottle(-500);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        miningTruck->turn(-0.5);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        miningTruck->turn(0.5);
    }
}

// Initialize OpenGL window
bool initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return false;
    }
    window = glfwCreateWindow(WIDTH, HEIGHT, "AutoMineSim", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    return true;
}

// Initialize GLEW
bool initGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        return false;
    }
    return true;
}

// Setup OpenGL camera
void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)WIDTH / HEIGHT, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Update camera to follow vehicle
void updateCamera() {
    btVector3 position = miningTruck->getBody()->getCenterOfMassPosition();
    btVector3 lookAt = position + btVector3(0, 3, -10);

    glLoadIdentity();
    gluLookAt(lookAt.getX(), lookAt.getY(), lookAt.getZ(),
        position.getX(), position.getY(), position.getZ(),
        0, 1, 0);
}

// Draw simple terrain
void drawGround() {
    glColor3f(0.3, 0.8, 0.3);
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

// Main function
int main() {
    if (!initGLFW()) return -1;
    if (!initGLEW()) return -1;
    setupCamera();

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    initBullet();
    miningTruck = new Vehicle(dynamicsWorld);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);
        updatePhysics();
        updateCamera();

        drawGround();  // Draw terrain
        miningTruck->draw();  // Draw mining truck

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete miningTruck;
    glfwTerminate();
    return 0;
}
