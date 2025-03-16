#include "Simulation.h"
#include "Vehicle.h"
#include <iostream>

// Bullet Physics components
btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
btBroadphaseInterface* broadphase = nullptr;
btCollisionDispatcher* dispatcher = nullptr;
btConstraintSolver* solver = nullptr;
btDefaultCollisionConfiguration* collisionConfig = nullptr;

void initBullet() {
    collisionConfig = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfig);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);

    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

    // Create the ground plane
    btStaticPlaneShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0))
    );

    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundBody = new btRigidBody(groundRigidBodyCI);
    groundBody->setFriction(0.8f);  // Increase ground friction for stability
    groundBody->setRestitution(0.0f);  // Ensure no bouncing effect
    dynamicsWorld->addRigidBody(groundBody);
}

extern Vehicle* miningTruck;

void updatePhysics() {
    if (!dynamicsWorld) {
        std::cerr << "Error: dynamicsWorld is NULL!" << std::endl;
        return;
    }

    dynamicsWorld->stepSimulation(1 / 60.f, 10);

    for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            btTransform trans;
            body->getMotionState()->getWorldTransform(trans);
            std::cout << "Updated Object Position: "
                << trans.getOrigin().getX() << ", "
                << trans.getOrigin().getY() << ", "
                << trans.getOrigin().getZ() << std::endl;

            // If truck isn't moving, apply a small push
            if (body == miningTruck->getBody() && trans.getOrigin().getX() == 0) {
                std::cout << "WARNING: Truck is stuck! Applying push force.\n";
                body->applyCentralImpulse(btVector3(5, 0, 5));  // Small push to get it moving
            }
        }
    }
}









