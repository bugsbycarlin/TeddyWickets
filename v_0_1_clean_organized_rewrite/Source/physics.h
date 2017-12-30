#ifndef PHYSICS_H
#define PHYSICS_H

// Bullet, for physics
#include "btBulletDynamicsCommon.h"

#include "point.h"

#include <stdio.h>
#include <math.h>
#include <list>
#include <cstdlib>

struct Pair {
    const btCollisionObject* obA;
    const btCollisionObject* obB;
};

class Physics {
  public:

    // Bullet Physics variables
    btAlignedObjectArray<btCollisionShape*> collisionShapes;
    btDiscreteDynamicsWorld* dynamicsWorld;
    btSequentialImpulseConstraintSolver* solver;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btDefaultCollisionConfiguration* collisionConfiguration;

    int objectCounter;

    Physics();

    int addBody(btRigidBody* body);

    int addWall(float xWidth, float yWidth, float zWidth, float xPos, float yPos, float zPos);
    int addWall(Point* p1, Point* p2, Point* p3, Point* p4);

    int addBumper(Point* start, Point* end, Point* normal);

    int addWicket(Point* pole_1_position, Point* pole_2_position, float height);

    int addBall(float radius, float xPos, float yPos, float zPos);

    Point* normalVector(Point* p1, Point* p2);
    Point* normalVectorRelative(Point* p1, Point* p2, Point* base);

    void impulse(int identity, float xImpulse, float yImpulse, float zImpulse);
    void setPosition(int identity, Point* p);
    void setRotation(int identity, float yaw, float pitch, float roll);
    void setPositionAndRotation(int identity, Point* p, float yaw, float pitch, float roll);
    void setTransform(int identity, btTransform trans);
    void stop(int identity);
    float getVelocity(int identity);

    void update(float timeStep);

    void printPositions();

    void updatePoint(Point* p, int identity);
    btTransform getTransform(int identity);
    bool hasCollision(int identity, int identity2);
    bool checkActive(int identity);
    void activate(int identity);
    void deactivate(int identity);

    bool initializePhysics();

    void shutdown();
};

#endif