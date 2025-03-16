#ifndef VEHICLE_H  
#define VEHICLE_H  
#include "Vehicle.h"
#include <btBulletDynamicsCommon.h>  
#include <GL/glut.h>  

class Vehicle {  
public:  
   Vehicle(btDiscreteDynamicsWorld* world);  
   ~Vehicle();  
   void update();  
   void applyThrottle(float force);  
   void turn(float angle);  
   void draw();  
   btRigidBody* getBody();  // Add this method declaration

private:  
   btRigidBody* chassis;  
   btRaycastVehicle* vehicle;  
   btVehicleRaycaster* raycaster;  
   btDiscreteDynamicsWorld* world;  
};  

#endif