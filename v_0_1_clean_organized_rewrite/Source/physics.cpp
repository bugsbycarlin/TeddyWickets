#include "physics.h"

Physics::Physics() {
  objectCounter = 0;
}

//add the body to the dynamics world
int Physics::addBody(btRigidBody* body) {
  int identity = objectCounter;
  dynamicsWorld->addRigidBody(body);
  objectCounter++;
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
  //printf("Normal = %f,%f,%f\n", p3->x, p3->y, p3->z);
  return p3;
}


int Physics::addBumper(Point* start, Point* end, Point* normal) {
  btTriangleMesh *mesh = new btTriangleMesh();

  // this differs from what is drawn; the collision detection is higher to bounce back more balls
  float bumper_height = 4.0f;

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

  collisionShapes.push_back(shape);

  btTransform groundTransform;
  groundTransform.setIdentity();

  btScalar mass(0.);

  btVector3 localInertia(0, 0, 0);

  //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
  btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
  btRigidBody* body = new btRigidBody(rbInfo);

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

  collisionShapes.push_back(shape);

  btTransform groundTransform;
  groundTransform.setIdentity();

  btScalar mass(0.);

  btVector3 localInertia(0, 0, 0);

  //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
  btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
  btRigidBody* body = new btRigidBody(rbInfo);

  body->setFriction(0.4f);
  body->setRollingFriction(0.05f);

  body->setRestitution(0.8);

  return addBody(body);
}


int Physics::addWall(float xWidth, float yWidth, float zWidth, float xPos, float yPos, float zPos) {
  btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(xWidth), btScalar(yWidth), btScalar(zWidth)));

  collisionShapes.push_back(groundShape);

  btTransform groundTransform;
  groundTransform.setIdentity();
  groundTransform.setOrigin(btVector3(xPos, yPos, zPos));

  btScalar mass(0.);

  btVector3 localInertia(0, 0, 0);

  //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
  btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
  btRigidBody* body = new btRigidBody(rbInfo);

  body->setFriction(0.4f);
  body->setRollingFriction(0.05f);

  body->setRestitution(0.8);

  return addBody(body);
}

int Physics::addWicket(Point* pole_1_position, Point* pole_2_position, float height) {
  // pole radius
  float pole_radius = 0.2f;

  btCollisionShape* colShape_1 = new btCylinderShapeZ(btVector3(pole_radius, pole_radius, height));
  collisionShapes.push_back(colShape_1);

  btCollisionShape* colShape_2 = new btCylinderShapeZ(btVector3(pole_radius, pole_radius, height));
  collisionShapes.push_back(colShape_2);

  // eventual top of the wicket
  //btCollisionShape* colShape_3 = new btCylinderShapeZ(btVector3(pole_radius, pole_radius, height));
  //collisionShapes.push_back(colShape_2);

  btScalar mass(0.);
  btVector3 localInertia(0, 0, 0);

  // set up the first body
  btTransform groundTransform;
  groundTransform.setIdentity();
  groundTransform.setOrigin(btVector3(pole_1_position->x, pole_1_position->y, pole_1_position->z));
  btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape_1, localInertia);
  btRigidBody* body = new btRigidBody(rbInfo);
  body->setRestitution(1.5);
  addBody(body);

  btTransform groundTransform2;
  groundTransform2.setIdentity();
  groundTransform2.setOrigin(btVector3(pole_2_position->x, pole_2_position->y, pole_2_position->z));
  btDefaultMotionState* myMotionState2 = new btDefaultMotionState(groundTransform2);
  btRigidBody::btRigidBodyConstructionInfo rbInfo2(mass, myMotionState2, colShape_2, localInertia);
  btRigidBody* body2 = new btRigidBody(rbInfo2);
  body2->setRestitution(1.5);
  // it's junk to only return 1
  return addBody(body2);
}

int Physics::addBall(float radius, float xPos, float yPos, float zPos) {
  btCollisionShape* colShape = new btSphereShape(btScalar(radius));
  collisionShapes.push_back(colShape);

  /// Create Dynamic Objects
  btTransform startTransform;
  startTransform.setIdentity();

  btScalar mass(1.f);

  //rigidbody is dynamic if and only if mass is non zero, otherwise static
  bool isDynamic = (mass != 0.f);

  btVector3 localInertia(0, 0, 0);
  if (isDynamic)
    colShape->calculateLocalInertia(mass, localInertia);

  startTransform.setOrigin(btVector3(xPos, yPos, zPos));

  btQuaternion quat;
  quat.setEuler(0,0,-0.78);
  //quat.setEuler(0,0,0);
  startTransform.setRotation(quat);

  //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
  btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
  //rbInfo.m_friction = 7.9;
  btRigidBody* body = new btRigidBody(rbInfo);

  body->setFriction(0.4f);
  body->setRollingFriction(0.05f);

  body->setRestitution(0.7);

  return addBody(body);
}

void Physics::impulse(int identity, float xImpulse, float yImpulse, float zImpulse) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(obj);
  body->applyCentralImpulse(btVector3(xImpulse, yImpulse, zImpulse));
}

void Physics::setPosition(int identity, Point* p) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(obj);
  btTransform trans;
  if (body && body->getMotionState())
  {
    body->getMotionState()->getWorldTransform(trans);
  }
  else
  {
    trans = obj->getWorldTransform();
  }
  trans.setOrigin(btVector3(p->x, p->y, p->z));
  body->setWorldTransform(trans);
}

void Physics::setRotation(int identity, float yaw, float pitch, float roll) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(obj);
  btTransform trans;
  if (body && body->getMotionState())
  {
    body->getMotionState()->getWorldTransform(trans);
  }
  else
  {
    trans = obj->getWorldTransform();
  }
  //btTransform newTransform;

  //newTransform.setIdentity();
  Point* p = new Point(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
  trans.setOrigin(btVector3(p->x, p->y, p->z));

  btQuaternion quat;
  quat.setEuler(yaw, pitch, roll);
  trans.setRotation(quat);
  

  body->setWorldTransform(trans);
}

void Physics::stop(int identity) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(obj);
  body->setLinearVelocity(btVector3(0,0,0));
  body->setAngularVelocity(btVector3(0,0,0));
}

float Physics::getVelocity(int identity) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(obj);
  btVector3 v = body->getLinearVelocity();
  return (float) v.length();
}

// Can't get rotation and translation to stop overwriting each other. Somewhere I'm implicitly setting an identity. 
void Physics::setPositionAndRotation(int identity, Point* p, float yaw, float pitch, float roll) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(obj);
  btTransform trans;
  if (body && body->getMotionState())
  {
    body->getMotionState()->getWorldTransform(trans);
  }
  else
  {
    trans = obj->getWorldTransform();
  }
  trans.setOrigin(btVector3(p->x, p->y, p->z));

  btQuaternion quat;
  quat.setEuler(yaw, pitch, roll);
  trans.setRotation(quat);
  

  body->setWorldTransform(trans);
}



void Physics::setTransform(int identity, btTransform trans) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(obj);
  body->setWorldTransform(trans);
}

void Physics::update(float timeStep) {
  dynamicsWorld->stepSimulation(timeStep, 10);
}

void Physics::printPositions() {
  //print positions of all objects
  for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
  {
    btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
    btRigidBody* body = btRigidBody::upcast(obj);
    btTransform trans;
    if (body && body->getMotionState())
    {
      body->getMotionState()->getWorldTransform(trans);
    }
    else
    {
      trans = obj->getWorldTransform();
    }
    printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
  }
}

// Update point p with the position of the body with id identity
void Physics::updatePoint(Point* p, int identity) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(obj);
  btTransform trans;
  if (body && body->getMotionState())
  {
    body->getMotionState()->getWorldTransform(trans);
  }
  else
  {
    trans = obj->getWorldTransform();
  }
  p->x = float(trans.getOrigin().getX());
  p->y = float(trans.getOrigin().getY());
  p->z = float(trans.getOrigin().getZ());
}

btTransform Physics::getTransform(int identity) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  btRigidBody* body = btRigidBody::upcast(obj);
  btTransform trans;
  if (body && body->getMotionState())
  {
    body->getMotionState()->getWorldTransform(trans);
  }
  else
  {
    trans = obj->getWorldTransform();
  }
  return trans;
  //btQuaternion rot = trans.getRotation();
  //q = new btQuaternion(rot.x(), rot.y(), rot.z(), rot.w());
  //printf("rotation = %f,%f,%f,%f\n", float(rot.w()), float(rot.x()), float(rot.y()), float(rot.z()));
}

bool Physics::checkActive(int identity) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  if (obj->isActive()) {
    return true;
  }
  return false;
}

void Physics::activate(int identity) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  obj->activate(true);
}

void Physics::deactivate(int identity) {
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  obj->setActivationState(0);
}

std::list<Pair*> contacts = { };

bool Physics::hasCollision(int identity, int identity2) {
  const btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[identity];
  const btCollisionObject* obj2 = dynamicsWorld->getCollisionObjectArray()[identity2];

  for (auto contact = contacts.begin(); contact != contacts.end(); ++contact) {
    if (((*contact)->obA == obj && (*contact)->obB == obj2) || ((*contact)->obA == obj2 && (*contact)->obB == obj)) {
      return true;
    }
  }
  return false;
}

void myTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    contacts.clear();
    //printf("The world just ticked by %f seconds\n", (float)timeStep);

    int numManifolds = world->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        Pair* x = new Pair();
        x->obA = obA;
        x->obB = obB;
        contacts.push_front(x);
    }
}

bool Physics::initializePhysics() {
  // collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
  collisionConfiguration = new btDefaultCollisionConfiguration();

  // use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
  dispatcher = new btCollisionDispatcher(collisionConfiguration);

  // btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
  overlappingPairCache = new btDbvtBroadphase();

  // the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
  solver = new btSequentialImpulseConstraintSolver;

  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

  dynamicsWorld->setGravity(btVector3(0, 0, -10));

  dynamicsWorld->setInternalTickCallback(myTickCallback);

  // To do: learn how to catch the potential initialization errors in Bullet.
  return true;
}

void Physics::shutdown() {
  // Clean up Bullet physics
  int i;

  //remove the rigidbodies from the dynamics world and delete them
  for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
  {
    btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
    btRigidBody* body = btRigidBody::upcast(obj);
    if (body && body->getMotionState())
    {
      delete body->getMotionState();
    }
    dynamicsWorld->removeCollisionObject(obj);
    delete obj;
  }

  //delete collision shapes
  for (int j = 0; j < collisionShapes.size(); j++)
  {
    btCollisionShape* shape = collisionShapes[j];
    collisionShapes[j] = 0;
    delete shape;
  }

  //delete dynamics world
  delete dynamicsWorld;

  //delete solver
  delete solver;

  //delete broadphase
  delete overlappingPairCache;

  //delete dispatcher
  delete dispatcher;

  delete collisionConfiguration;

  //next line is optional: it will be cleared by the destructor when the array goes out of scope
  collisionShapes.clear();
}
