#include "Vehicle.h"
#include "Resource.h"
#include <GL/glut.h>
#include <iostream>
#include <chrono>

Vehicle::Vehicle(btDiscreteDynamicsWorld* world) {
    btCollisionShape* shape = new btBoxShape(btVector3(1, 1, 1));
    btDefaultMotionState* motionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 1, 0))
    );

    btScalar mass = 10;
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo bodyCI(mass, motionState, shape, inertia);
    body = new btRigidBody(bodyCI);

    body->setActivationState(DISABLE_DEACTIVATION);
    body->setFriction(0.01f);  // Reduce friction to allow movement
    body->setDamping(0.1f, 0.1f);
    body->setGravity(btVector3(0, -9.81, 0));
    world->addRigidBody(body);
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

extern std::vector<btVector3> hazards;
extern bool truckStarted;

// AI logic to move toward target while avoiding hazards
void Vehicle::update(const std::vector<btVector3>& hazards) {
    if (!truckStarted) return;

    btVector3 currentPos = body->getCenterOfMassPosition();
    bool hazardDetected = false;
    btVector3 avoidanceForce(0, 0, 0);  // Corrected variable name

    std::cout << "Truck Position Before: " << currentPos.getX()
        << ", " << currentPos.getY()
        << ", " << currentPos.getZ() << std::endl;

    // Check for nearby hazards
    for (const auto& hazard : hazards) {
        float distanceToHazard = (currentPos - hazard).length();

        if (distanceToHazard < 5.0f) {  // If too close to a hazard, adjust direction
			hazardDetected = true;
            btVector3 avoidDirection = currentPos - hazard;
            avoidDirection.setY(0);  // Keep movement in X-Z plane
            avoidDirection.normalize();
            avoidanceForce += avoidDirection * 500.0f;  // Move away
            std::cout << "Avoiding hazard at: " << hazard.getX() << ", " << hazard.getZ() << std::endl;
            break;
        }
    }

	if (hazardDetected) {
		body->applyCentralForce(avoidanceForce);
    }
    else {
		btVector3 direction = targetPosition - currentPos;
		direction.normalize();
		body->applyTorque(direction * 500.0f);
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

    // Apply a stronger force for debugging
    body->setLinearVelocity(btVector3(0, 0, 0));
    btVector3 force = direction * 500.0f;  // Increase force for testing
    body->applyCentralForce(force);

    std::cout << "Applying force: " << force.getX()
        << ", " << force.getY()
        << ", " << force.getZ() << std::endl;

    btVector3 newPos = body->getCenterOfMassPosition();
    std::cout << "Truck Position After: " << newPos.getX()
        << ", " << newPos.getY()
        << ", " << newPos.getZ() << std::endl;
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
