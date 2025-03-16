#include "Vehicle.h"
#include <iostream>

Vehicle::Vehicle(btDiscreteDynamicsWorld* world) : world(world) {
    // Create vehicle chassis
    btBoxShape* chassisShape = new btBoxShape(btVector3(2, 1, 4));
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 5, 0)));

    btScalar mass = 1500;
    btVector3 inertia(0, 0, 0);
    chassisShape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, chassisShape, inertia);
    chassis = new btRigidBody(rbInfo);
    world->addRigidBody(chassis);

    // Set up vehicle suspension
    raycaster = new btDefaultVehicleRaycaster(world);
    btRaycastVehicle::btVehicleTuning tuning;
    vehicle = new btRaycastVehicle(tuning, chassis, raycaster);
    world->addVehicle(vehicle);

    float suspensionRestLength = 0.6f;
    float wheelRadius = 0.5f;
    btVector3 wheelDirectionCS0(0, -1, 0);
    btVector3 wheelAxleCS(1, 0, 0);

    // Add four wheels
    vehicle->addWheel(btVector3(-1.5f, -1.2f, 2.5f), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);
    vehicle->addWheel(btVector3(1.5f, -1.2f, 2.5f), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);
    vehicle->addWheel(btVector3(-1.5f, -1.2f, -2.5f), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);
    vehicle->addWheel(btVector3(1.5f, -1.2f, -2.5f), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);
}

Vehicle::~Vehicle() {
    world->removeVehicle(vehicle);
    delete vehicle;
    delete raycaster;
    delete chassis;
}

void Vehicle::update() {
    vehicle->updateVehicle(1 / 60.f);
}

void Vehicle::applyThrottle(float force) {
    vehicle->applyEngineForce(force, 2);
    vehicle->applyEngineForce(force, 3);
}

void Vehicle::turn(float angle) {
    vehicle->setSteeringValue(angle, 0);
    vehicle->setSteeringValue(angle, 1);
}

void Vehicle::draw() {
    btTransform trans;
    chassis->getMotionState()->getWorldTransform(trans);
    btScalar matrix[16];
    trans.getOpenGLMatrix(matrix);

    glPushMatrix();
    glMultMatrixf(matrix);
    glColor3f(1.0, 0.0, 0.0);  // Red color
    glutSolidCube(3);  // Chassis
    glPopMatrix();
}
