#ifndef VEHICLE_H
#define VEHICLE_H

#include <btBulletDynamicsCommon.h>
#include <vector>
#include <chrono>

class Vehicle {
private:
    btRigidBody* body;
    btVector3 targetPosition;
    bool isMining = false;
    std::chrono::time_point<std::chrono::steady_clock> miningStartTime;

public:
    Vehicle(btDiscreteDynamicsWorld* world);
    void update(const std::vector<btVector3>& hazards);
    void draw();  // Ensure this is declared
    void setTarget(const btVector3& target);
    void findClosestResource();
    btRigidBody* getBody();
    bool isCurrentlyMining();
};

#endif
