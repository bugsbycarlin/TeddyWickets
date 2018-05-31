/*

  This program reproduces a Bullet physics collision issue. A sphere is smacked and rolls across a plane.

  Since the sphere is resting on the plane, and the force is parallel to that plane, the expected behavior
  is for the sphere to roll with no vertical motion, eventually stopping due to friction.

  The actual behavior is the ball bouncing about 20% of its radius out of plane.

  I believe that instabilities in the sphere/triangle solver (similar to those documented in a prior message
  board post and previously fixed) are causing erratic vectors out of plane.

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
      - Rotates the sphere by theta around the z axis so it points at a different plane direction
      - Smacks the sphere with an impulse
      - For 120 steps:
        - Steps Bullet by 1/60.0 plus or minus some random jitter
        - Prints the z value of the sphere
        - Records the max z value so far
      - Prints the max z value
      - Destroys the plane and the sphere

  In the ideal situation, from every angle of attack, the sphere should roll stably with a z value of roughly 1,
  indicating it is still resting on the plane. But in this repro, you should expect to see z values of around 1.2.

*/

#include <string>
#include <list>


#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"


// btAlignedObjectArray<btCollisionShape*> collision_shapes;
// btSoftRigidDynamicsWorld* dynamics_world;
// btSequentialImpulseConstraintSolver* solver;
// btCollisionDispatcher* dispatcher;
// btBroadphaseInterface* overlapping_pair_cache;
// btDefaultCollisionConfiguration* collision_configuration;

bool initialize() {

}


int main(int argc, char* args[]) {

  printf("Initializing.\n");
  initialize();

  
}
