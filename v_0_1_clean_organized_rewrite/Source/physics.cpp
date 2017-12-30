/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "physics.h"

Physics::Physics() {
  object_counter = 0;
}

// add the body to the dynamics world
int Physics::addBody(btRigidBody* body) {
  int identity = object_counter;
  dynamics_world->addRigidBody(body);
  object_counter++;
  return identity;
}

Point* Physics::normalVector(Point* p1, Point* p2) {
  Point* p3 = new Point();
  p3->x = p1->y*p2->z - p2->y*p1->z;
  p3->y = -1 * (p1->x*p2->z - p2->x*p1->z);
  p3->z = p1->x*p2->y - p2->x*p1->y;
  float scale_factor = sqrt(p3->x*p3->x + p3->y*p3->y + p3->z*p3->z);
  p3->x /= scale_factor;
  p3->y /= scale_factor;
  p3->z /= scale_factor;
  return p3;
}

Point* Physics::normalVectorRelative(Point* p1, Point* p2, Point* base) {
  Point* p3 = new Point();
  p3->x = (p1->y - base->y)*(p2->z - base->z) - (p2->y - base->y)*(p1->z - base->z);
  p3->y = -1 * ((p1->x - base->x)*(p2->z - base->z) - (p2->x - base->x)*(p1->z - base->z));
  p3->z = (p1->x - base->x)*(p2->y - base->y) - (p2->x - base->x)*(p1->y - base->y);
  float scale_factor = sqrt(p3->x*p3->x + p3->y*p3->y + p3->z*p3->z);
  p3->x /= scale_factor;
  p3->y /= scale_factor;
  p3->z /= scale_factor;
  // printf("Normal = %f,%f,%f\n", p3->x, p3->y, p3->z);
  return p3;
}


int Physics::addBumper(Point* start, Point* end, Point* normal) {
  btTriangleMesh *mesh = new btTriangleMesh();

  // this differs from what is drawn; the collision detection is higher to bounce back more balls
  float bumper_height = k_physics_bumper_height;

  mesh->addTriangle(btVector3(start->x, start->y, start->z),
                    btVector3(end->x, end->y, end->z),
                    btVector3(start->x, start->y, start->z + bumper_height));
  mesh->addTriangle(btVector3(start->x, start->y, start->z),
                    btVector3(end->x, end->y, end->z),
                    btVector3(end->x, end->y, end->z + bumper_height));
  mesh->addTriangle(btVector3(start->x + normal->x, start->y + normal->y, start->z + normal->z),
                    btVector3(end->x + normal->x, end->y + normal->y, end->z + normal->z),
                    btVector3(start->x + normal->x, start->y + normal->y, start->z + normal->z + bumper_height));
  mesh->addTriangle(btVector3(start->x + normal->x, start->y + normal->y, start->z + normal->z),
                    btVector3(end->x + normal->x, end->y + normal->y, end->z + normal->z),
                    btVector3(end->x + normal->x, end->y + normal->y, end->z + normal->z + bumper_height));
  btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);

  collision_shapes.push_back(shape);

  btTransform ground_transform;
  ground_transform.setIdentity();

  btScalar mass(0.);

  btVector3 local_inertia(0, 0, 0);

  // using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
  btDefaultMotionState* motion_state = new btDefaultMotionState(ground_transform);
  btRigidBody::btRigidBodyConstructionInfo rigid_body_info(mass, motion_state, shape, local_inertia);
  btRigidBody* body = new btRigidBody(rigid_body_info);

  body->setFriction(0.4f);
  body->setRollingFriction(0.05f);

  body->setRestitution(1.25);

  return addBody(body);
}

int Physics::addWall(Point* p1, Point* p2, Point* p3, Point* p4) {
  btTriangleMesh *mesh = new btTriangleMesh();
  mesh->addTriangle(btVector3(p1->x, p1->y, p1->z),
                    btVector3(p2->x, p2->y, p2->z),
                    btVector3(p3->x, p3->y, p3->z));
  mesh->addTriangle(btVector3(p3->x, p3->y, p3->z),
                    btVector3(p4->x, p4->y, p4->z),
                    btVector3(p1->x, p1->y, p1->z));
  btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);

  collision_shapes.push_back(shape);

  btTransform ground_transform;
  ground_transform.setIdentity();

  btScalar mass(0.);

  btVector3 local_inertia(0, 0, 0);

  // using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
  btDefaultMotionState* motion_state = new btDefaultMotionState(ground_transform);
  btRigidBody::btRigidBodyConstructionInfo rigid_body_info(mass, motion_state, shape, local_inertia);
  btRigidBody* body = new btRigidBody(rigid_body_info);

  body->setFriction(0.4f);
  body->setRollingFriction(0.05f);

  body->setRestitution(0.8);

  return addBody(body);
}

int Physics::addWicket(Point* pole_1_position, Point* pole_2_position, float height) {
  // pole radius
  float pole_radius = 0.2f;

  btCollisionShape* shape_1 = new btCylinderShapeZ(btVector3(pole_radius, pole_radius, height));
  collision_shapes.push_back(shape_1);

  btCollisionShape* shape_2 = new btCylinderShapeZ(btVector3(pole_radius, pole_radius, height));
  collision_shapes.push_back(shape_2);

  // eventual top of the wicket
  // btCollisionShape* shape_3 = new btCylinderShapeZ(btVector3(pole_radius, pole_radius, height));
  // collision_shapes.push_back(shape_3);

  btScalar mass(0.);
  btVector3 local_inertia(0, 0, 0);

  // set up the first body
  btTransform ground_transform;
  ground_transform.setIdentity();
  ground_transform.setOrigin(btVector3(pole_1_position->x, pole_1_position->y, pole_1_position->z));
  btDefaultMotionState* motion_state = new btDefaultMotionState(ground_transform);
  btRigidBody::btRigidBodyConstructionInfo rigid_body_info(mass, motion_state, shape_1, local_inertia);
  btRigidBody* body = new btRigidBody(rigid_body_info);
  body->setRestitution(1.5);
  addBody(body);

  btTransform ground_transform_2;
  ground_transform_2.setIdentity();
  ground_transform_2.setOrigin(btVector3(pole_2_position->x, pole_2_position->y, pole_2_position->z));
  btDefaultMotionState* motion_state_2 = new btDefaultMotionState(ground_transform_2);
  btRigidBody::btRigidBodyConstructionInfo rigid_body_info_2(mass, motion_state_2, shape_2, local_inertia);
  btRigidBody* body2 = new btRigidBody(rigid_body_info_2);
  body2->setRestitution(1.5);
  // it's junk to only return 1
  return addBody(body2);
}

int Physics::addBall(float radius, float x_pos, float y_pos, float z_pos) {
  btCollisionShape* shape = new btSphereShape(btScalar(radius));
  collision_shapes.push_back(shape);

  // Create Dynamic Objects
  btTransform start_transform;
  start_transform.setIdentity();

  btScalar mass(1.f);

  btVector3 local_inertia(0, 0, 0);

  // ball is dynamic, so calculate local inertia
  shape->calculateLocalInertia(mass, local_inertia);

  start_transform.setOrigin(btVector3(x_pos, y_pos, z_pos));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(0, 0, k_default_shot_rotation);
  start_transform.setRotation(rotation_quaternion);

  // using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
  btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);
  btRigidBody::btRigidBodyConstructionInfo rigid_body_info(mass, motion_state, shape, local_inertia);
  btRigidBody* body = new btRigidBody(rigid_body_info);

  body->setFriction(0.4f);
  body->setRollingFriction(0.05f);

  body->setRestitution(0.7);

  return addBody(body);
}

void Physics::impulse(int identity, float x_impulse, float y_impulse, float z_impulse) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(collision_object);
  body->applyCentralImpulse(btVector3(x_impulse, y_impulse, z_impulse));
}

void Physics::setPosition(int identity, Point* p) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(collision_object);
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = collision_object->getWorldTransform();
  }
  transform.setOrigin(btVector3(p->x, p->y, p->z));
  body->setWorldTransform(transform);
}

void Physics::setRotation(int identity, float yaw, float pitch, float roll) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(collision_object);
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = collision_object->getWorldTransform();
  }

  Point* p = new Point(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ());
  transform.setOrigin(btVector3(p->x, p->y, p->z));

  btQuaternion rotation_quaterion;
  rotation_quaterion.setEuler(yaw, pitch, roll);
  transform.setRotation(rotation_quaterion);

  body->setWorldTransform(transform);
}

void Physics::stop(int identity) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(collision_object);
  body->setLinearVelocity(btVector3(0, 0, 0));
  body->setAngularVelocity(btVector3(0, 0, 0));
}

float Physics::getVelocity(int identity) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(collision_object);
  btVector3 velocity = body->getLinearVelocity();
  return (float) velocity.length();
}

// Can't get rotation and translation to stop overwriting each other. Somewhere I'm implicitly setting an identity.
// In the meantime, here's a hack.
void Physics::setPositionAndRotation(int identity, Point* p, float yaw, float pitch, float roll) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(collision_object);
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = collision_object->getWorldTransform();
  }
  transform.setOrigin(btVector3(p->x, p->y, p->z));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(yaw, pitch, roll);
  transform.setRotation(rotation_quaternion);

  body->setWorldTransform(transform);
}

void Physics::setTransform(int identity, btTransform transform) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(collision_object);
  body->setWorldTransform(transform);
}

void Physics::update(float time_step) {
  dynamics_world->stepSimulation(time_step, 10);
}

void Physics::printPositions() {
  // print positions of all objects
  for (int j = dynamics_world->getNumCollisionObjects() - 1; j >= 0; j--) {
    btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[j];
    btRigidBody* body = btRigidBody::upcast(collision_object);
    btTransform transform;
    if (body && body->getMotionState()) {
      body->getMotionState()->getWorldTransform(transform);
    } else {
      transform = collision_object->getWorldTransform();
    }
    printf("world pos object %d = %f,%f,%f\n", j,
      float(transform.getOrigin().getX()),
      float(transform.getOrigin().getY()),
      float(transform.getOrigin().getZ()));
  }
}

// Update point p with the position of the body with id identity
void Physics::updatePoint(Point* p, int identity) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(collision_object);
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = collision_object->getWorldTransform();
  }
  p->x = float(transform.getOrigin().getX());
  p->y = float(transform.getOrigin().getY());
  p->z = float(transform.getOrigin().getZ());
}

btTransform Physics::getTransform(int identity) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(collision_object);
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = collision_object->getWorldTransform();
  }
  return transform;
}

bool Physics::checkActive(int identity) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  if (collision_object->isActive()) {
    return true;
  }
  return false;
}

void Physics::activate(int identity) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  collision_object->activate(true);
}

void Physics::deactivate(int identity) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  collision_object->setActivationState(0);
}

// weird hack
std::list<CollisionPair*> contacts = { };

void myTickCallback(btDynamicsWorld *world, btScalar time_step) {
    contacts.clear();

    int num_manifolds = world->getDispatcher()->getNumManifolds();
    for (int i = 0; i < num_manifolds; i++) {
        btPersistentManifold* contact_manifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* collision_object_a = contact_manifold->getBody0();
        const btCollisionObject* collision_object_b = contact_manifold->getBody1();

        CollisionPair* x = new CollisionPair();
        x->collision_object_a = collision_object_a;
        x->collision_object_b = collision_object_b;
        contacts.push_front(x);
    }
}

bool Physics::hasCollision(int identity, int identity2) {
  const btCollisionObject* collision_object1 = dynamics_world->getCollisionObjectArray()[identity];
  const btCollisionObject* collision_object2 = dynamics_world->getCollisionObjectArray()[identity2];

  for (auto contact = contacts.begin(); contact != contacts.end(); ++contact) {
    if (((*contact)->collision_object_a == collision_object1
      && (*contact)->collision_object_b == collision_object2)
      || ((*contact)->collision_object_a == collision_object2
      && (*contact)->collision_object_b == collision_object1)) {
      return true;
    }
  }
  return false;
}

bool Physics::initializePhysics() {
  // collision configuration contains default setup for memory, collision setup.
  // Advanced users can create their own configuration.
  collision_configuration = new btDefaultCollisionConfiguration();

  // use the default collision dispatcher.
  dispatcher = new btCollisionDispatcher(collision_configuration);

  // btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
  overlapping_pair_cache = new btDbvtBroadphase();

  // the default constraint solver.
  solver = new btSequentialImpulseConstraintSolver;

  dynamics_world = new btDiscreteDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_configuration);

  dynamics_world->setGravity(btVector3(0, 0, -10));

  dynamics_world->setInternalTickCallback(myTickCallback);

  // To do: learn how to catch the potential initialization errors in Bullet.
  return true;
}

void Physics::shutdown() {
  // Clean up Bullet physics
  int i;

  // remove the rigidbodies from the dynamics world and delete them
  for (i = dynamics_world->getNumCollisionObjects() - 1; i >= 0; i--) {
    btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[i];
    btRigidBody* body = btRigidBody::upcast(collision_object);
    if (body && body->getMotionState()) {
      delete body->getMotionState();
    }
    dynamics_world->removeCollisionObject(collision_object);
    delete collision_object;
  }

  // delete collision shapes
  for (int j = 0; j < collision_shapes.size(); j++) {
    btCollisionShape* shape = collision_shapes[j];
    collision_shapes[j] = 0;
    delete shape;
  }

  // delete dynamics world
  delete dynamics_world;

  // delete solver
  delete solver;

  // delete broadphase
  delete overlapping_pair_cache;

  // delete dispatcher
  delete dispatcher;

  delete collision_configuration;

  // next line is optional: it will be cleared by the destructor when the array goes out of scope
  collision_shapes.clear();
}
