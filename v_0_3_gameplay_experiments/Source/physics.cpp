/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "physics.h"

Physics::Physics() {
  object_counter = 0;
}

// Add a body to the dynamics world.
// The body is defined by its shape (a collection of points),
// its transform (a matrix specifying its position and rotation),
// mass, and coefficients of friction, rolling friction, and restitution.
int Physics::addBody(btCollisionShape* shape,
        btTransform transform,
        float mass,
        float friction,
        float rolling_friction,
        float restitution) {
  btScalar bt_mass(mass);

  btVector3 local_inertia(0, 0, 0);

  if (mass != 0.f) {
    shape->calculateLocalInertia(bt_mass, local_inertia);
    printf("I calculated local inertia for this one\n");
  }

  btDefaultMotionState* motion_state = new btDefaultMotionState(transform);
  btRigidBody::btRigidBodyConstructionInfo rigid_body_info(bt_mass, motion_state, shape, local_inertia);
  btRigidBody* body = new btRigidBody(rigid_body_info);

  body->setFriction(friction);
  body->setRollingFriction(rolling_friction);
  body->setRestitution(restitution);

  int identity = object_counter;
  dynamics_world->addRigidBody(body);
  object_counter++;

  return identity;
}

// Calculate the normal of two points as though they were vectors from the origin
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

// Calculate the normal of two points as though they were vectors from the base vector
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

// Add a bumper object to the physics system
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

  btTransform transform;
  transform.setIdentity();

  return addBody(shape, transform, 0.0f, 0.4f, 0.05f, 1.25f);
}

// Add a surface (eg a wall or a floor) object to the physics system.
int Physics::addSurface(Point* p1, Point* p2, Point* p3, Point* p4) {
  btTriangleMesh *mesh = new btTriangleMesh();
  mesh->addTriangle(btVector3(p1->x, p1->y, p1->z),
                    btVector3(p2->x, p2->y, p2->z),
                    btVector3(p3->x, p3->y, p3->z));
  mesh->addTriangle(btVector3(p3->x, p3->y, p3->z),
                    btVector3(p4->x, p4->y, p4->z),
                    btVector3(p1->x, p1->y, p1->z));
  btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);

  collision_shapes.push_back(shape);

  btTransform transform;
  transform.setIdentity();

  return addBody(shape, transform, 0.0f, 0.4f, 0.05f, 0.8f);
}

// Add a whole mesh
int Physics::addMesh(std::list<Triangle*> triangles, Point* position, float rotation) {
  btTriangleMesh *mesh = new btTriangleMesh();
  int counter = 0;
  for (auto triangle = triangles.begin(); triangle != triangles.end(); ++triangle) {
    mesh->addTriangle(btVector3((*triangle)->p3->z, (*triangle)->p3->x, (*triangle)->p3->y),
      btVector3((*triangle)->p2->z, (*triangle)->p2->x, (*triangle)->p2->y),
      btVector3((*triangle)->p1->z, (*triangle)->p1->x, (*triangle)->p1->y));
    printf("Adding triangle with points %0.8f,%0.8f,%0.8f and %0.8f,%0.8f,%0.8f and %0.8f,%0.8f,%0.8f\n",
      (*triangle)->p3->z, (*triangle)->p3->x, (*triangle)->p3->y,
      (*triangle)->p2->z, (*triangle)->p2->x, (*triangle)->p2->y,
      (*triangle)->p1->z, (*triangle)->p1->x, (*triangle)->p1->y);
    counter += 1;
  }

  //printf("Added %d triangles to the physics system.\n", counter);
  btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);

  collision_shapes.push_back(shape);

  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(position->x, position->y, position->z));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(0, 0, rotation);
  transform.setRotation(rotation_quaternion);

  return addBody(shape, transform, 0.0f, hot_config->getFloat("world_friction"), hot_config->getFloat("world_rolling_friction"), 0.8f);
}

// Add a mesh which is to be used as a softbody for the player ball
int Physics::addSoftball(std::list<Triangle*> triangles, Point* position, float rotation) {
  btTriangleMesh *mesh = new btTriangleMesh();
  int counter = 0;
  for (auto triangle = triangles.begin(); triangle != triangles.end(); ++triangle) {
    mesh->addTriangle(btVector3((*triangle)->p1->z, (*triangle)->p1->x, (*triangle)->p1->y),
      btVector3((*triangle)->p2->z, (*triangle)->p2->x, (*triangle)->p2->y),
      btVector3((*triangle)->p3->z, (*triangle)->p3->x, (*triangle)->p3->y));
    counter += 1;
  }
  printf("Adding %d softbody mesh triangles to the physics system.\n", counter);
  btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);

  collision_shapes.push_back(shape);

  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(position->x, position->y, position->z));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(0, 0, rotation);
  transform.setRotation(rotation_quaternion);

  float mass = 1.0f;
  float friction = hot_config->getFloat("ball_friction");
  float rolling_friction = hot_config->getFloat("ball_rolling_friction");
  float restitution = 0.7f;

  const btVector3 meshScaling = mesh->getScaling();

  btAlignedObjectArray<btScalar> vertices;
  btAlignedObjectArray<int> triangz;

  for (int part=0;part< mesh->getNumSubParts(); part++)
  {
      const unsigned char * vertexbase;
      const unsigned char * indexbase;

      int indexstride;
      int stride,numverts,numtriangles;
      PHY_ScalarType type, gfxindextype;

      mesh->getLockedReadOnlyVertexIndexBase(&vertexbase,numverts,type,stride,&indexbase,indexstride,numtriangles,gfxindextype,part);

      for (int gfxindex=0; gfxindex < numverts; gfxindex++)
      {
          float* graphicsbase = (float*)(vertexbase+gfxindex*stride);
          vertices.push_back(graphicsbase[0]*meshScaling.getX());
          vertices.push_back(graphicsbase[1]*meshScaling.getY());
          vertices.push_back(graphicsbase[2]*meshScaling.getZ());
      }

      for (int gfxindex=0;gfxindex < numtriangles; gfxindex++)
      {
          unsigned int* tri_indices= (unsigned int*)(indexbase+gfxindex*indexstride);
          triangz.push_back(tri_indices[0]);
          triangz.push_back(tri_indices[1]);
          triangz.push_back(tri_indices[2]);
      }
  }

  btSoftBody* booty = btSoftBodyHelpers::CreateFromTriMesh(*softBodyWorldInfo, &vertices[0], &triangz[0], triangz.size()/3);

  btVector3 scaling(k_model_scale, k_model_scale, k_model_scale); 

  btSoftBody::Material* pm=booty->appendMaterial();
  pm->m_kLST = 0.75;
  pm->m_flags -= btSoftBody::fMaterial::DebugDraw;
  booty->scale(scaling);
  booty->generateBendingConstraints(4,pm);
  booty->m_cfg.piterations = 2;  
  booty->m_cfg.kDF = 0.75;
  booty->m_cfg.collisions |= btSoftBody::fCollision::VF_SS;  
  booty->randomizeConstraints(); 

  booty->transform(transform);
  booty->setTotalMass(mass); 
  booty->getCollisionShape()->setMargin(0.1f);
  booty->setFriction(friction);
  booty->setRollingFriction(rolling_friction);
  booty->setRestitution(restitution);

  int identity = object_counter;
  dynamics_world->addSoftBody(booty);
  object_counter++;

  return identity;
}

// Add a wicket object to the physics system. This involves adding two poles.
// Currently, for simplicity I just return the identity of the second pole.
// int Physics::addWicket(Point* pole_1_position, Point* pole_2_position, float height) {
//   // pole radius
//   float pole_radius = 0.2f;

//   btCollisionShape* shape_1 = new btCylinderShapeZ(btVector3(pole_radius, pole_radius, height));
//   collision_shapes.push_back(shape_1);

//   btCollisionShape* shape_2 = new btCylinderShapeZ(btVector3(pole_radius, pole_radius, height));
//   collision_shapes.push_back(shape_2);

//   // To do: top of the wicket
//   // btCollisionShape* shape_3 = new btCylinderShapeZ(btVector3(pole_radius, pole_radius, height));
//   // collision_shapes.push_back(shape_3);

//   // set up the first body
//   btTransform transform_1;
//   transform_1.setIdentity();
//   transform_1.setOrigin(btVector3(pole_1_position->x, pole_1_position->y, pole_1_position->z));
//   addBody(shape_1, transform_1, 0.0f, 0.4f, 0.05f, 1.5f);

//   // set up the second body
//   // To do: find a nice way to return both bodies to the wicket object
//   btTransform transform_2;
//   transform_2.setIdentity();
//   transform_2.setOrigin(btVector3(pole_2_position->x, pole_2_position->y, pole_2_position->z));
//   return addBody(shape_2, transform_2, 0.0f, 0.4f, 0.05f, 1.5f);
// }

// Add a ball/character object to the physics system.
int Physics::addBall(float radius, float x_pos, float y_pos, float z_pos) {
  btCollisionShape* shape = new btSphereShape(btScalar(radius));
  collision_shapes.push_back(shape);

  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(x_pos, y_pos, z_pos));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(0, 0, k_default_shot_rotation);
  transform.setRotation(rotation_quaternion);

  return addBody(shape, transform, 1.0f, hot_config->getFloat("ball_friction"), hot_config->getFloat("ball_rolling_friction"), 0.7f);
}

int Physics::addFloor(float size) {
  btTriangleMesh *mesh = new btTriangleMesh();
  int counter = 0;
  mesh->addTriangle(
    btVector3(-size, -size, 0),
    btVector3(-size, size, 0),
    btVector3(size, size, 0));
  mesh->addTriangle(
    btVector3(-size, -size, 0),
    btVector3(size, size, 0),
    btVector3(size, -size, 0));

  btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);

  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(0, 0, 0));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(0, 0, 0);
  transform.setRotation(rotation_quaternion);

  float mass = 0.0f;
  btScalar bt_mass(mass);
  btVector3 local_inertia(0, 0, 0);

  btDefaultMotionState* motion_state = new btDefaultMotionState(transform);
  btRigidBody::btRigidBodyConstructionInfo rigid_body_info(bt_mass, motion_state, shape, local_inertia);
  btRigidBody* body = new btRigidBody(rigid_body_info);

  float friction = 1.0;
  float rolling_friction = 0.1;
  float restitution = 0.7f;
  body->setFriction(friction);
  body->setRollingFriction(rolling_friction);
  body->setRestitution(restitution);

  dynamics_world->addRigidBody(body);

  return -500;
}

// Add a ball/character object to the physics system.
int Physics::addBallMesh(std::list<Triangle*> triangles, float radius, float x_pos, float y_pos, float z_pos) {
  btTriangleMesh *mesh = new btTriangleMesh();
  int counter = 0;
  for (auto triangle = triangles.begin(); triangle != triangles.end(); ++triangle) {
    mesh->addTriangle(btVector3((*triangle)->p1->z, (*triangle)->p1->x, (*triangle)->p1->y),
      btVector3((*triangle)->p2->z, (*triangle)->p2->x, (*triangle)->p2->y),
      btVector3((*triangle)->p3->z, (*triangle)->p3->x, (*triangle)->p3->y));
    counter += 1;
  }
  //printf("Added %d triangles to the physics system.\n", counter);
  btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);

  collision_shapes.push_back(shape);

  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(btVector3(x_pos, y_pos, z_pos));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(0, 0, k_default_shot_rotation);
  transform.setRotation(rotation_quaternion);

  return addBody(shape, transform, 1.0f, hot_config->getFloat("ball_friction"), hot_config->getFloat("ball_rolling_friction"), 0.7f);
}

// Get a rigid body by integer identity
btRigidBody* Physics::getBodyById(int identity) {
  btCollisionObject* collision_object = dynamics_world->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(collision_object);
  return body;
}

// Set an impulse force on a body
void Physics::impulse(int identity, float x_impulse, float y_impulse, float z_impulse) {
  btRigidBody* body = getBodyById(identity);
  body->applyCentralImpulse(btVector3(x_impulse, y_impulse, z_impulse));
}

// Set a position
void Physics::setPosition(int identity, Point* p) {
  btRigidBody* body = getBodyById(identity);
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = body->getWorldTransform();
  }
  transform.setOrigin(btVector3(p->x, p->y, p->z));
  body->setWorldTransform(transform);
}

// Set a rotation
void Physics::setRotation(int identity, float yaw, float pitch, float roll) {
  btRigidBody* body = getBodyById(identity);
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = body->getWorldTransform();
  }

  Point* p = new Point(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ());
  transform.setOrigin(btVector3(p->x, p->y, p->z));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(yaw, pitch, roll);
  transform.setRotation(rotation_quaternion);

  body->setWorldTransform(transform);
}

// Can't get rotation and translation to stop overwriting each other. Somewhere I'm implicitly setting an identity.
// In the meantime, here's a hack.
void Physics::setPositionAndRotation(int identity, Point* p, float yaw, float pitch, float roll) {
  btRigidBody* body = getBodyById(identity);
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = body->getWorldTransform();
  }
  transform.setOrigin(btVector3(p->x, p->y, p->z));

  btQuaternion rotation_quaternion;
  rotation_quaternion.setEuler(yaw, pitch, roll);
  transform.setRotation(rotation_quaternion);

  body->setWorldTransform(transform);
}

// Stop a body from moving
void Physics::stop(int identity) {
  btRigidBody* body = getBodyById(identity);
  body->setLinearVelocity(btVector3(0, 0, 0));
  body->setAngularVelocity(btVector3(0, 0, 0));
}

// Get the velocity of a body
float Physics::getVelocity(int identity) {
  btRigidBody* body = getBodyById(identity);
  btVector3 velocity = body->getLinearVelocity();
  return (float) velocity.length();
}

Point* Physics::getVelocityVector(int identity) {
  btRigidBody* body = getBodyById(identity);
  btVector3 velocity = body->getLinearVelocity();
  return new Point(velocity.x(), velocity.y(), velocity.z());
}

// Set the transform of a body
void Physics::setTransform(int identity, btTransform transform) {
  btRigidBody* body = getBodyById(identity);
  body->setWorldTransform(transform);
}

// Update the entire physics system.
// This is sort of revokable by saving everything and restoring it afterwards.
void Physics::update(float time_step) {
  dynamics_world->stepSimulation(time_step, 10);
}

// Update point p with the position of the body with id identity
void Physics::updatePoint(Point* p, int identity) {
  btRigidBody* body = getBodyById(identity);
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = body->getWorldTransform();
  }
  p->x = float(transform.getOrigin().getX());
  p->y = float(transform.getOrigin().getY());
  p->z = float(transform.getOrigin().getZ());
}

// Print the positions of all objects
void Physics::printPositions() {
  for (int j = dynamics_world->getNumCollisionObjects() - 1; j >= 0; j--) {
    btRigidBody* body = getBodyById(j);
    btTransform transform;
    if (body && body->getMotionState()) {
      body->getMotionState()->getWorldTransform(transform);
    } else {
      transform = body->getWorldTransform();
    }
    printf("world pos object %d = %f,%f,%f\n", j,
      float(transform.getOrigin().getX()),
      float(transform.getOrigin().getY()),
      float(transform.getOrigin().getZ()));
  }
}

// Get the transform of a body
btTransform Physics::getTransform(int identity) {
  btRigidBody* body = getBodyById(identity);
  btTransform transform;
  if (body && body->getMotionState()) {
    body->getMotionState()->getWorldTransform(transform);
  } else {
    transform = body->getWorldTransform();
  }
  return transform;
}

// Check if a body is active in the physics system
bool Physics::checkActive(int identity) {
  btRigidBody* body = getBodyById(identity);
  if (body->isActive()) {
    return true;
  }
  return false;
}

// Activate a body in the physics system
void Physics::activate(int identity) {
  btRigidBody* body = getBodyById(identity);
  body->activate(true);
}

// Deactivate a body in the physics system
void Physics::deactivate(int identity) {
  btRigidBody* body = getBodyById(identity);
  body->setActivationState(0);
}

// weird hack; myTickCallBack and hasCollision together allow me to track
// collisions by id. I need this to update game logic when specific collisions happen.
// One example is the player hitting a bumper; I need to cause that bumper to light up.
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

  dynamics_world = new btSoftRigidDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_configuration);

  dynamics_world->setGravity(btVector3(0, 0, -10));

  dynamics_world->setInternalTickCallback(myTickCallback);

  softBodyWorldInfo = new btSoftBodyWorldInfo();
  softBodyWorldInfo->m_broadphase = overlapping_pair_cache;
  softBodyWorldInfo->m_dispatcher = dispatcher;
  softBodyWorldInfo->m_gravity = dynamics_world->getGravity();
  softBodyWorldInfo->m_sparsesdf.Initialize();

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
