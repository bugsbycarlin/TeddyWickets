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
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "hot_config.h"
#include "globals.h"
#include "point.h"
#include "triangle.h"

struct CollisionPair {
    const btCollisionObject* collision_object_a;
    const btCollisionObject* collision_object_b;
};

class Physics {
 public:
    btAlignedObjectArray<btCollisionShape*> collision_shapes;
    btSoftRigidDynamicsWorld* dynamics_world;
    btSequentialImpulseConstraintSolver* solver;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlapping_pair_cache;
    btDefaultCollisionConfiguration* collision_configuration;

    btSoftBodyWorldInfo* softBodyWorldInfo;

    int object_counter;

    Physics();

    Point* normalVector(Point* p1, Point* p2);
    Point* normalVectorRelative(Point* p1, Point* p2, Point* base);

    int addBody(btCollisionShape* shape,
        btTransform transform,
        float mass,
        float friction,
        float rolling_friction,
        float restitution);

    int addSurface(Point* p1, Point* p2, Point* p3, Point* p4);
    int addBumper(Point* start, Point* end, Point* normal);
    //int addWicket(Point* pole_1_position, Point* pole_2_position, float height);
    int addBall(float radius, float x_pos, float y_pos, float z_pos);
    int addBallMesh(std::list<Triangle*> triangles, float radius, float x_pos, float y_pos, float z_pos);
    int addMesh(std::list<Triangle*> triangles, Point* position, float rotation);
    int addSoftball(std::list<Triangle*> triangles, Point* position, float rotation);

    void update(float time_step);

    void impulse(int identity, float x_impulse, float y_impulse, float z_impulse);
    void setPosition(int identity, Point* p);
    void setRotation(int identity, float yaw, float pitch, float roll);
    void setPositionAndRotation(int identity, Point* p, float yaw, float pitch, float roll);
    void setTransform(int identity, btTransform transform);
    void stop(int identity);

    void printPositions();
    void updatePoint(Point* p, int identity);
    btRigidBody* getBodyById(int identity);
    btTransform getTransform(int identity);
    float getVelocity(int identity);
    Point* getVelocityVector(int identity);
    bool hasCollision(int identity, int identity2);
    bool checkActive(int identity);

    void activate(int identity);
    void deactivate(int identity);

    bool initializePhysics();

    void shutdown();
};

#endif
