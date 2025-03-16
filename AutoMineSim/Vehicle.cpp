#include "Vehicle.h"
#include "Resource.h"
#include <GL/glut.h>
#include <iostream>
#include <chrono>

Vehicle::Vehicle(btDiscreteDynamicsWorld* world) {
    btCollisionShape* shape = new btBoxShape(btVector3(1, 1, 1));  // Ensure proper shape
    btDefaultMotionState* motionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 2, 0))  // Start slightly above ground
    );

    btScalar mass = 10;
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo bodyCI(mass, motionState, shape, inertia);
    body = new btRigidBody(bodyCI);

    body->setActivationState(DISABLE_DEACTIVATION);
    body->setFriction(0.3f);  // Ensure it's not too sticky
    body->setDamping(0.1f, 0.1f);  // Allow smooth movement
    world->addRigidBody(body);
}

extern std::vector<btVector3> hazards;
extern bool truckStarted;

// AI logic to move toward target while avoiding hazards
void Vehicle::update(const std::vector<btVector3>& hazards) {
    if (!truckStarted) return;

    btVector3 currentPos = body->getCenterOfMassPosition();

    std::cout << "Truck Position Before: " << currentPos.getX()
        << ", " << currentPos.getY()
        << ", " << currentPos.getZ() << std::endl;

    if (currentPos.getY() < 0.5f) {  // If truck is below ground level
        std::cout << "WARNING: Truck is inside the terrain!" << std::endl;
        body->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(currentPos.getX(), 1, currentPos.getZ())));
    }

    if (isMining) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - miningStartTime).count();
        if (elapsed >= 10) {
            isMining = false;
            findClosestResource();
        }
        return;
    }

    btVector3 direction = targetPosition - currentPos;
    float distanceToTarget = direction.length();

    if (distanceToTarget < 2.0f) {
        isMining = true;
        miningStartTime = std::chrono::steady_clock::now();
        std::cout << "Mining resource at: " << targetPosition.getX()
            << ", " << targetPosition.getZ() << std::endl;
        return;
    }

    direction.normalize();

    for (const auto& hazard : hazards) {
        float distance = (currentPos - hazard).length();
        if (distance < 5.0f) {
            direction += btVector3(rand() % 2 - 1, 0, rand() % 2 - 1);
            break;
        }
    }

    body->setLinearVelocity(btVector3(0, 0, 0));

    btVector3 force = direction * 5.0f;
    body->applyCentralForce(force);

    std::cout << "Applying force: " << force.getX()
        << ", " << force.getY()
        << ", " << force.getZ() << std::endl;

    btVector3 newPos = body->getCenterOfMassPosition();
    std::cout << "Truck Position After: " << newPos.getX()
        << ", " << newPos.getY()
        << ", " << newPos.getZ() << std::endl;
}

void Vehicle::draw() {
    btTransform trans;
    body->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();

    glColor3f(1.0, 0.0, 0.0);  // Red color for mining truck
    glPushMatrix();
    glTranslatef(pos.getX(), pos.getY(), pos.getZ());
    glutSolidCube(2);
    glPopMatrix();
}

// AI searches for the closest resource
void Vehicle::findClosestResource() {
    float closestDistance = FLT_MAX;
    btVector3 closestResourcePos;
    bool found = false;

    for (auto& resource : resources) {
        if (!resource->isCollected) {
            float distance = (body->getCenterOfMassPosition() - resource->getBody()->getCenterOfMassPosition()).length();
            if (distance < closestDistance) {
                closestDistance = distance;
                closestResourcePos = resource->getBody()->getCenterOfMassPosition();
                found = true;
            }
        }
    }

    if (found) {
        std::cout << "New target resource at: " << closestResourcePos.getX() << ", " << closestResourcePos.getZ() << std::endl;
        setTarget(closestResourcePos);
    }
    else {
        std::cout << "No more resources found. Moving randomly.\n";
        btVector3 randomWander((rand() % 40) - 20, 0, (rand() % 40) - 20);
        setTarget(randomWander);
    }
}

void Vehicle::setTarget(const btVector3& target) {
    targetPosition = target;
}

btRigidBody* Vehicle::getBody() {
    return body;
}

// Check if the truck is currently mining
bool Vehicle::isCurrentlyMining() {
    return isMining;
}
