/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Physics class wraps the Bullet Physics engine with Teddy Wickets specific functionality.
*/

#ifndef TEDDY_WICKETS_PHYSICS_H_
#define TEDDY_WICKETS_PHYSICS_H_

#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <list>

// Bullet, for physics
#include "btBulletDynamicsCommon.h"

#include "globals.h"
#include "point.h"

struct CollisionPair {
    const btCollisionObject* collision_object_a;
    const btCollisionObject* collision_object_b;
};

class Physics {
 public:
    btAlignedObjectArray<btCollisionShape*> collision_shapes;
    btDiscreteDynamicsWorld* dynamics_world;
    btSequentialImpulseConstraintSolver* solver;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlapping_pair_cache;
    btDefaultCollisionConfiguration* collision_configuration;

    int object_counter;

    Physics();

    Point* normalVector(Point* p1, Point* p2);
    Point* normalVectorRelative(Point* p1, Point* p2, Point* base);

    int addBody(btRigidBody* body);

    int addWall(Point* p1, Point* p2, Point* p3, Point* p4);
    int addBumper(Point* start, Point* end, Point* normal);
    int addWicket(Point* pole_1_position, Point* pole_2_position, float height);
    int addBall(float radius, float x_pos, float y_pos, float z_pos);

    void update(float time_step);

    void impulse(int identity, float x_impulse, float y_impulse, float z_impulse);
    void setPosition(int identity, Point* p);
    void setRotation(int identity, float yaw, float pitch, float roll);
    void setPositionAndRotation(int identity, Point* p, float yaw, float pitch, float roll);
    void setTransform(int identity, btTransform transform);
    void stop(int identity);

    void printPositions();
    void updatePoint(Point* p, int identity);
    btTransform getTransform(int identity);
    float getVelocity(int identity);
    bool hasCollision(int identity, int identity2);
    bool checkActive(int identity);

    void activate(int identity);
    void deactivate(int identity);

    bool initializePhysics();

    void shutdown();
};

#endif
