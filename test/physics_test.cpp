#include <btBulletDynamicsCommon.h>
#include <GL/freeglut.h>
#include <iostream>

// Bullet Physics Variables
btDiscreteDynamicsWorld* dynamicsWorld;
btRigidBody* fallBody;
btRigidBody* groundBody;

// OpenGL Draw Function
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Step the physics simulation
    dynamicsWorld->stepSimulation(1 / 60.f, 10);

    // Get sphere position
    btTransform trans;
    fallBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();

    // Draw the sphere
    glPushMatrix();
    glTranslatef(pos.getX(), pos.getY(), pos.getZ());
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();

    glutSwapBuffers();
}

// OpenGL Initialization
void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 1.0, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0, 10, 20, 0, 0, 0, 0, 1, 0);
}

// Bullet Physics Initialization
void initBullet() {
    btDefaultCollisionConfiguration* collisionConfig = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfig);
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

    // Ground Plane
    btStaticPlaneShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    groundBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundBody);

    // Falling Sphere
    btSphereShape* fallShape = new btSphereShape(1);
    btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10, 0)));
    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    fallBody = new btRigidBody(fallRigidBodyCI);
    dynamicsWorld->addRigidBody(fallBody);
}

// OpenGL Loop
void update(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Main Function
int main(int argc, char** argv) {
    // Initialize Bullet Physics
    initBullet();

    // Initialize OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Bullet Physics Simulation");

    initOpenGL();

    glutDisplayFunc(renderScene);
    glutTimerFunc(16, update, 0);

    glutMainLoop();

    return 0;
}
