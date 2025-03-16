#include "Simulation.h"
#include <iostream>

// Declare Bullet Physics components
btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
btBroadphaseInterface* broadphase = nullptr;
btCollisionDispatcher* dispatcher = nullptr;
btConstraintSolver* solver = nullptr;
btDefaultCollisionConfiguration* collisionConfig = nullptr;

void initBullet() {
    // Bullet Physics Initialization
    collisionConfig = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfig);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

    std::cout << "Bullet Physics Initialized" << std::endl;

    // Create the ground plane
    btStaticPlaneShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -2, 0))
    );

    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundBody);
}

void updatePhysics() {
    if (dynamicsWorld) {
        dynamicsWorld->stepSimulation(1 / 60.f);
    }
}
