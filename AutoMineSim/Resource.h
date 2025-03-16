#ifndef RESOURCE_H
#define RESOURCE_H

#include <btBulletDynamicsCommon.h>
#include <vector>
#include "Vehicle.h"

// Forward declare 'Vehicle' to avoid incomplete type error
class Vehicle;

class Resource {
public:
    Resource(btDiscreteDynamicsWorld* world, float x, float y, float z);
    void draw();
    btRigidBody* getBody();
    bool isCollected = false;  // Track if the resource has been mined

private:
    btRigidBody* body;
};

// Declare the global list of resources
extern std::vector<Resource*> resources;

void generateResources(btDiscreteDynamicsWorld* world);  // Generate random resources
void drawResources();
void checkMining(Vehicle* vehicle);  // Check if vehicle is mining a resource

#endif
