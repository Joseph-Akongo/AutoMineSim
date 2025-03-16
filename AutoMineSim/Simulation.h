#ifndef SIMULATION_H
#define SIMULATION_H

#include <btBulletDynamicsCommon.h>

extern btDiscreteDynamicsWorld* dynamicsWorld;

void initBullet();
void updatePhysics();

#endif
