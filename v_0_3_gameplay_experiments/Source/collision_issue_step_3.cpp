/*

  This program reproduces a Bullet physics collision issue. A sphere is smacked and rolls across a plane.

  Since the sphere is resting on the plane, and the force is parallel to that plane, the expected behavior
  is for the sphere to roll with no vertical motion, eventually stopping due to friction.

  The actual behavior is the ball bouncing about 20% of its radius out of plane.

  I believe that instabilities in the rigid body sphere/triangle solver (similar to those documented in a 
  prior message board post and previously fixed) are causing erratic vectors out of plane.

  I am using Bullet3-2.87.

  Prior items which seem similar and/or relevant:

  https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11208
  https://github.com/bulletphysics/bullet3/issues/935
  https://github.com/bulletphysics/bullet3/pull/940/commits/72dd8285e81db8fabe7af0571d667e63386de977


  Repro code:
    - Initializes Bullet
    - For each of 50 angles theta from 0 to PI:
      - Creates a plane on the x and y axes and a unit sphere with what I believe to be sort of standard properties
      - Puts the sphere resting on the plane (at a z value of 1)
      - Steps Bullet a few times to make sure the sphere is resting stably.
      - Smacks the sphere with an impulse in a direction given by rotating theta around the z axis
      - For 120 steps:
        - Steps Bullet by 1/60.0 plus or minus some random jitter
        - Prints the z value of the sphere
        - Records the max z value so far
      - Prints the max z value
      - Destroys the plane and the sphere

  In the ideal situation, from every angle of attack, the sphere should roll stably with a z value of roughly 1,
  indicating it is still resting on the plane. But in this repro, you should expect to see z values of around 1.2.

*/

#include <cstdlib>
#include <string>
#include <random>
#include <chrono>
#include <math.h>

#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"

btSoftRigidDynamicsWorld* dynamics_world;
btSequentialImpulseConstraintSolver* solver;
btCollisionDispatcher* dispatcher;
btBroadphaseInterface* overlapping_pair_cache;
btDefaultCollisionConfiguration* collision_configuration;

btSoftBodyWorldInfo* softBodyWorldInfo;

void initializeBullet() {

  collision_configuration = new btDefaultCollisionConfiguration();

  dispatcher = new btCollisionDispatcher(collision_configuration);

  overlapping_pair_cache = new btDbvtBroadphase();

  solver = new btSequentialImpulseConstraintSolver;

  dynamics_world = new btSoftRigidDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_configuration);

  dynamics_world->setGravity(btVector3(0, 0, -10));

  softBodyWorldInfo = new btSoftBodyWorldInfo();
  softBodyWorldInfo->m_broadphase = overlapping_pair_cache;
  softBodyWorldInfo->m_dispatcher = dispatcher;
  softBodyWorldInfo->m_gravity = dynamics_world->getGravity();
  softBodyWorldInfo->m_sparsesdf.Initialize();
}

btRigidBody* addBall(float radius, float x_pos, float y_pos, float z_pos) {
  btCollisionShape* shape = new btSphereShape(btScalar(radius));

  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(x_pos, y_pos, z_pos));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(0, 0, 0);
  transform.setRotation(rotation_quaternion);

  float mass = 1.0f;
  btScalar bt_mass(mass);
  btVector3 local_inertia(0, 0, 0);
  shape->calculateLocalInertia(bt_mass, local_inertia);

  btDefaultMotionState* motion_state = new btDefaultMotionState(transform);
  btRigidBody::btRigidBodyConstructionInfo rigid_body_info(bt_mass, motion_state, shape, local_inertia);
  btRigidBody* body = new btRigidBody(rigid_body_info);

  float friction = 1.0;
  float rolling_friction = 0.1;
  float restitution = 0.7f;
  body->setFriction(friction);
  body->setRollingFriction(rolling_friction);
  body->setRestitution(restitution);

  body->setAnisotropicFriction(shape->getAnisotropicRollingFrictionDirection(),btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);

  dynamics_world->addRigidBody(body);

  return body;
}

// static bool CustomMaterialCombinerCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0,int partId0,int index0,const btCollisionObjectWrapper* colObj1,int partId1,int index1)
// {
//   btAdjustInternalEdgeContacts(cp,colObj1,colObj0, partId1,index1);
//   return true;
// }

// extern ContactAddedCallback   gContactAddedCallback;

btRigidBody* addFloorTile(float x, float y, float size) {
  btTriangleMesh *mesh = new btTriangleMesh();
  int counter = 0;
  mesh->addTriangle(
    btVector3(x + -size, y + -size, 0),
    btVector3(x + -size, y + size, 0),
    btVector3(x + size, y + size, 0));
  mesh->addTriangle(
    btVector3(x + -size, y + -size, 0),
    btVector3(x + size, y + size, 0),
    btVector3(x + size, y + -size, 0));

  btConvexTriangleMeshShape* shape = new btConvexTriangleMeshShape(mesh, true);

  // shape->setCollisionFlags(shape->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

  // btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
  // btGenerateInternalEdgeInfo(shape, triangleInfoMap);

  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(0, 0, 0.0));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(0, 0, 0);
  transform.setRotation(rotation_quaternion);

  float mass = 0.0f;
  btScalar bt_mass(mass);
  btVector3 local_inertia(0, 0, 0);
  shape->calculateLocalInertia(bt_mass, local_inertia);

  btDefaultMotionState* motion_state = new btDefaultMotionState(transform);
  btRigidBody::btRigidBodyConstructionInfo rigid_body_info(bt_mass, motion_state, shape, local_inertia);
  btRigidBody* body = new btRigidBody(rigid_body_info);

  float friction = 1.0;
  float rolling_friction = 0.1;
  float restitution = 0.8f;
  body->setFriction(friction);
  body->setRollingFriction(rolling_friction);
  body->setRestitution(restitution);

  body->setAnisotropicFriction(shape->getAnisotropicRollingFrictionDirection(),btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);

  dynamics_world->addRigidBody(body);

  return body;
}

btRigidBody* addFloor(float size) {
  btTriangleMesh *mesh = new btTriangleMesh();
  int counter = 0;
  for (int k = -size/20; k <= size/20; k++) {
    for (int l = -size/20; l <= size/20; l++) {
      //btRigidBody* floor = addFloor(20 * k, 20 * l, 10.0f);

      mesh->addTriangle(
        btVector3(20 * k + -10, 20 * l + -10, 0),
        btVector3(20 * k + -10, 20 * l + 10, 0),
        btVector3(20 * k + 10, 20 * l + 10, 0));
      mesh->addTriangle(
        btVector3(20 * k + -10, 20 * l + -10, 0),
        btVector3(20 * k + 10, 20 * l + 10, 0),
        btVector3(20 * k + 10, 20 * l + -10, 0));
    }
  }

  btConvexTriangleMeshShape* shape = new btConvexTriangleMeshShape(mesh, true);

  // shape->setCollisionFlags(shape->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

  // btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
  // btGenerateInternalEdgeInfo(shape, triangleInfoMap);

  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(0, 0, 0.0));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(0, 0, 0);
  transform.setRotation(rotation_quaternion);

  float mass = 0.0f;
  btScalar bt_mass(mass);
  btVector3 local_inertia(0, 0, 0);
  shape->calculateLocalInertia(bt_mass, local_inertia);

  btDefaultMotionState* motion_state = new btDefaultMotionState(transform);
  btRigidBody::btRigidBodyConstructionInfo rigid_body_info(bt_mass, motion_state, shape, local_inertia);
  btRigidBody* body = new btRigidBody(rigid_body_info);

  float friction = 1.0;
  float rolling_friction = 0.1;
  float restitution = 0.8f;
  body->setFriction(friction);
  body->setRollingFriction(rolling_friction);
  body->setRestitution(restitution);

  dynamics_world->addRigidBody(body);

  return body;
}

void setRotation(btRigidBody* body, float yaw, float pitch, float roll) {
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = body->getWorldTransform();
  }

  transform.setOrigin(btVector3(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ()));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(yaw, pitch, roll);
  transform.setRotation(rotation_quaternion);

  body->setWorldTransform(transform);
}

int main(int argc, char* args[]) {

  initializeBullet();

  // Random numbers
  std::mt19937_64 random_generator;
  uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
  random_generator.seed(ss);
  std::uniform_real_distribution<float> uniform_distribution(0, 1);

  // Time step is 1/60, but we'll jitter that each step
  float regular_time_step = 1.0f / 60.0f;

  // Smack the ball this hard
  float shot_power = 20.0f;

  // Create the floor
  btRigidBody* floor = addFloor(200.0f);

  // Or create floor tiles
  // float size = 200.0f;
  // for (int k = -size/20; k <= size/20; k++) {
  //   for (int l = -size/20; l <= size/20; l++) {
  //     addFloorTile(20 * k, 20 * l, 10.0f);
  //   }
  // }

  // Run this simulation with each of 50 ball angles from 0 to PI.
  for (int i = 0; i < 50; i++) {

    // Set the angle
    float theta = i * (M_PI / 50.0f);
    printf ("Testing the shot at angle %0.3f.\n", theta);

    // Create the ball
    btRigidBody* ball = addBall(1.0f, 0.0f, 0.0f, 1.0f);

    // Let the simulation rest for a few steps
    for (int j = 0; j < 200; j++) {
      dynamics_world->stepSimulation(regular_time_step, 10);
    }

    ball->activate(true);

    // Rotate like a chump
    setRotation(ball, 0, 0, theta);

    // Smack the ball in the theta direction
    float impulse_x = shot_power * sin(theta);
    float impulse_y = -1 * shot_power * cos(theta);
    ball->applyCentralImpulse(btVector3(impulse_x, impulse_y, 0.5));

    // Step forward 120 steps, each 1/60 plus or minus some jitter
    float max_z = -10;
    float min_z = 10;
    float min_ratio = 70;
    float max_ratio = 0.001;
    for (int j = 0; j < 120; j++) {
      float jitter = uniform_distribution(random_generator) / 600.0f;
      float time_step = regular_time_step - 0.5f / 600.0f + jitter;
      dynamics_world->stepSimulation(time_step, 10);

      // Get the transform of the ball and find the z value
      btTransform transform;
      if (ball->getMotionState()) {
        ball->getMotionState()->getWorldTransform(transform);
      } else {
        transform = ball->getWorldTransform();
      }
      float x = float(transform.getOrigin().getX());
      float y = float(transform.getOrigin().getY()); 
      float z = float(transform.getOrigin().getZ());

      // if (z > 1.01) {
      //   transform.setOrigin(btVector3(x, y, 1.01));
      //   ball->setWorldTransform(transform);
      // }

      // if (ball->getMotionState()) {
      //   ball->getMotionState()->getWorldTransform(transform);
      // } else {
      //   transform = ball->getWorldTransform();
      // }
      // x = float(transform.getOrigin().getX());
      // y = float(transform.getOrigin().getY()); 
      // z = float(transform.getOrigin().getZ());

      if (abs(impulse_x) > 0.001 && abs(impulse_y) > 0.001 && abs(x) > 0.001 && abs(y) > 0.001) {
        float expected_ratio = impulse_x / impulse_y;
        float observed_ratio = x / y;
        float ratio = expected_ratio / observed_ratio;

        if (ratio < min_ratio) {
          min_ratio = ratio;
        }

        if (ratio > max_ratio) {
          max_ratio = ratio;
        }
      }

      // Measure min and max z
      if (z < min_z) {
        min_z = z;
      }
      if (z > max_z) {
        max_z = z;
      }

      // Print the value
      //printf("Position: %0.5f, %0.5f, %0.5f\n", x, y, z);
    }

    // After 120 steps, print the min and max z values.
    printf("Min Z value: %0.5f\n", min_z);
    printf("Max Z value: %0.5f\n", max_z);
    printf("Min shot deviation ratio: %0.5f\n", min_ratio);
    printf("Max shot deviation ratio: %0.5f\n", max_ratio);
    printf("\n\n");

    // Remove the ball and floor
    dynamics_world->removeCollisionObject(ball);
    //dynamics_world->removeCollisionObject(floor);
  }
}
