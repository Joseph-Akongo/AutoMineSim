#include "Resource.h"
#include <GL/glut.h>
#include <iostream>
#include <cstdlib>

std::vector<Resource*> resources;

Resource::Resource(btDiscreteDynamicsWorld* world, float x, float y, float z) {
    btCollisionShape* shape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
    btDefaultMotionState* motionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(x, y, z))
    );

    btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, shape, btVector3(0, 0, 0));
    body = new btRigidBody(rbInfo);
    world->addRigidBody(body);

    resources.push_back(this);
}

btRigidBody* Resource::getBody() {
    return body;
}

// Draw resources in the game
void drawResources() {
    for (auto& resource : resources) {
        if (!resource->isCollected) {
            btTransform trans;
            resource->getBody()->getMotionState()->getWorldTransform(trans);
            btVector3 pos = trans.getOrigin();

            glColor3f(1.0, 1.0, 0.0);  // Yellow for resources
            glPushMatrix();
            glTranslatef(pos.getX(), pos.getY(), pos.getZ());
            glutSolidCube(1);
            glPopMatrix();
        }
    }
}

// Generate random resources in the game world
void generateResources(btDiscreteDynamicsWorld* world) {
    srand(time(NULL));
    std::vector<btVector3> placedPositions;  // Store existing resource positions

    for (int i = 0; i < 5; i++) {  // Spawn 5 resources
        float x, z;
        bool tooClose;

        do {
            x = (rand() % 80) - 40;  // Wider range (-40 to 40)
            z = (rand() % 80) - 40;
            tooClose = false;

            // Check if the new position is too close to existing resources
            for (const auto& pos : placedPositions) {
                if ((btVector3(x, 1, z) - pos).length() < 10.0f) {  // Keep at least 10 units apart
                    tooClose = true;
                    break;
                }
            }
        } while (tooClose);

        placedPositions.push_back(btVector3(x, 1, z));
        new Resource(world, x, 1, z);
    }
}

// Check if the vehicle is mining a resource
void checkMining(Vehicle* vehicle) {
    btVector3 truckPos = vehicle->getBody()->getCenterOfMassPosition();

    for (auto& resource : resources) {
        if (!resource->isCollected) {
            float distance = (truckPos - resource->getBody()->getCenterOfMassPosition()).length();
            if (distance < 3.0f) {  // If the truck is near a resource, collect it
                resource->isCollected = true;
                std::cout << "Resource mined!" << std::endl;
            }
        }
    }
}
