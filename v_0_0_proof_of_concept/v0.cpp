/*

  Teddy Wickets
  Matthew Carlin
  October 2017

  V0 - Proof of Concept

  This is just a teddy ball on a hand coded board bouncing around.

  It's a test of three things:
  1) 2d sprite ball on either a 2d world or a 3d ortho world
  2) Gameplay
  3) Physics engine

*/

/*

Build with:

g++ -w v0.cpp -std=c++11 -o TeddyWickets -framework OpenGL -framework SDL2 -framework SDL2_image -I bullet_src ./libBulletCollision_gmake_x64_release.a ./libBulletDynamics_gmake_x64_release.a ./libLinearMath_gmake_x64_release.a

*/


// This is an attempt at the SDL code:

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include "btBulletDynamicsCommon.h"
#include <OpenGL/GLU.h>
#include <stdio.h>
#include <string>
#include <list>


// Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Starts up SDL, creates window, and initializes OpenGL
bool init();

// Initializes matrices and clear color
bool initGL();

// Input handler
void handleKeys( unsigned char key, int x, int y );

// Per frame update
void update();

// Renders things to the screen
void render();

// Frees media and shuts down SDL
void close();

void initLightingAndPerspective();

// Physics
void initBullet();

void testBullet();

void cleanupBullet();

// The window
SDL_Window* gWindow = NULL;

// Textures
SDL_Surface* gSuspenderBear = NULL;
SDL_Surface* gTiles = NULL;
SDL_Surface* gClouds = NULL;

// The OpenGL context
SDL_GLContext gContext;

// Render flag
bool gRender = true;

GLuint *textures = new GLuint[2];

float rotation_value = 0.0f;

GLUquadric* ball;

class Point
{
public:
  float x,y,z;

  Point() {
  }
  
  Point(float x,float y,float z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  
  void v()
  {
    glVertex3f(x,y,z);
  }

  void add(Point* p2)
  {
    this->x += p2->x;
    this->y += p2->y;
    this->z += p2->z;
  }
};

class Surface {
  public:
    Point* p1;
    Point* p2;
    Point* p3;
    Point* p4;

    Surface(Point* p1, Point* p2, Point* p3, Point* p4) {
      this->p1 = p1;
      this->p2 = p2;
      this->p3 = p3;
      this->p4 = p4;
    }
};

std::list<Surface*> world = { };

Point* ball_position;
Point* ball_velocity;

btAlignedObjectArray<btCollisionShape*> collisionShapes;
btDiscreteDynamicsWorld* dynamicsWorld;
btSequentialImpulseConstraintSolver* solver;
btCollisionDispatcher* dispatcher;
btBroadphaseInterface* overlappingPairCache;
btDefaultCollisionConfiguration* collisionConfiguration;

bool init()
{
  // Initialization flag
  bool success = true;

  // Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    success = false;
  }
  else
  {
    // Use OpenGL 2.1
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

    // Create window
    gWindow = SDL_CreateWindow( "Teddy Wickets", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
      success = false;
    }
    else
    {
      // Create context
      gContext = SDL_GL_CreateContext( gWindow );
      if( gContext == NULL )
      {
        printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
        success = false;
      }
      else
      {
        // Use Vsync
        if( SDL_GL_SetSwapInterval( 1 ) < 0 )
        {
          printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
        }

        // Initialize OpenGL
        if( !initGL() )
        {
          printf( "Unable to initialize OpenGL!\n" );
          success = false;
        }

        initBullet();
      }
    }
  }

  return success;
}

void initBullet()
{
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
}

bool initGL()
{
  bool success = true;
  GLenum error = GL_NO_ERROR;

  // Initialize Projection Matrix
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  
  // Check for error
  error = glGetError();
  if( error != GL_NO_ERROR )
  {
    printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
    success = false;
  }

  // Initialize Modelview Matrix
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // Check for error
  error = glGetError();
  if( error != GL_NO_ERROR )
  {
    printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
    success = false;
  }
  
  // Initialize clear color
  glClearColor( 0.9f, 0.9f, 0.9f, 1.f );
  
  // Check for error
  error = glGetError();
  if( error != GL_NO_ERROR )
  {
    printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
    success = false;
  }


  // Load sample.png into image
  gSuspenderBear = IMG_Load("suspender_bear_2.png");
  if (gSuspenderBear == nullptr) {
      printf( "IMG_Load: %s\n", IMG_GetError() );
  }

  // Load sample.png into image
  gTiles = IMG_Load("tiles.png");
  if (gTiles == nullptr) {
      printf( "IMG_Load: %s\n", IMG_GetError() );
  }

  // Load sample.png into image
  gClouds = IMG_Load("clouds.png");
  if (gClouds == nullptr) {
      printf( "IMG_Load: %s\n", IMG_GetError() );
  }

  // Enable texture! Oh yeah.
  glEnable(GL_TEXTURE_2D);

  int Mode = GL_RGBA;

  glGenTextures(3, textures);

  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, Mode, gSuspenderBear->w, gSuspenderBear->h, 0, Mode, GL_UNSIGNED_BYTE, gSuspenderBear->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, textures[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, Mode, gTiles->w, gTiles->h, 0, Mode, GL_UNSIGNED_BYTE, gTiles->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, textures[2]);
  glTexImage2D(GL_TEXTURE_2D, 0, Mode, gClouds->w, gClouds->h, 0, Mode, GL_UNSIGNED_BYTE, gClouds->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);




  ball = gluNewQuadric();      // Create A Pointer To The Quadric Object  
  gluQuadricTexture(ball, GL_TRUE);    // Create Texture Coords   
  gluQuadricNormals(ball, GLU_SMOOTH); // Create Smooth Normals   

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);  
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);


  //glFrontFace(GL_CW);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  initLightingAndPerspective();

  return success;
}

void initLightingAndPerspective()
{

  // //Far, far too much lighting for far, far too little effect.
  // //I have much to learn.
  // glEnable(GL_LIGHTING);
  
  // GLfloat ambient[] = {0.6f,0.6f,0.6f,1.0f};
  // GLfloat diffuse[] = {0.9f,0.9f,0.9f,1.0f};
  // GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  // GLfloat position[] = {0.0f,0.0f,2.0f,1.0f};
  // GLfloat direction[] = {0.0f,0.0f,-2.0f,1.0f};
  
  // glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
  // glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
  // glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
  // glLightfv(GL_LIGHT1, GL_POSITION, position);
  // glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction);
  // glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 7.0f);
  // glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 30.0f);
  // glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 1.0f);
  // glEnable(GL_LIGHT1);
  
  // glEnable(GL_COLOR_MATERIAL);
  // glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  // glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  // glMateriali(GL_FRONT, GL_SHININESS, 128);
  
  glViewport(0,0,800,600);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f,800.0f/600.0f,0.1f,1000.0f);
  //gluOrtho()
  glMatrixMode(GL_MODELVIEW);
}

void Start2DDraw()
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 800, 0, 600);
  glScalef(1, -1, 1);
  glTranslatef(0, -600, 0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}

void End2DDraw()
{
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void handleKeys( unsigned char key, int x, int y )
{
  // Toggle quad
  if (key == 'q') {
    gRender = !gRender;
  }

  // if (key == 'a') {
  //   rotation_value += 5;
  // }

  // if (key == 'd') {
  //   rotation_value -= 5;
  // }

  if (key == 'd') {
    //ball_velocity = new Point(0.1f, 0, 0.1f);
    btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[1];
    btRigidBody* body = btRigidBody::upcast(obj);
    body->applyCentralImpulse(btVector3(5,0,5));
  }

  if (key == 'w') {
    //ball_velocity = new Point(0.1f, 0, 0.1f);
    btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[1];
    btRigidBody* body = btRigidBody::upcast(obj);
    body->applyCentralImpulse(btVector3(0,5,5));
  }

  if (key == 'e') {
    //ball_velocity = new Point(0.1f, 0, 0.1f);
    btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[1];
    btRigidBody* body = btRigidBody::upcast(obj);
    body->applyCentralImpulse(btVector3(12,10,1));
  }
}

void update()
{
  // No per frame update needed
  // rotation_value += 1;

  // ball_position->add(ball_velocity);

  // if (ball_position->z > 0.5f) {
  //   ball_velocity->z -= 0.005f;
  // } else {
  //   ball_velocity->z = 0.0f;
  //   ball_position->z = 0.5f;
  // }


  dynamicsWorld->stepSimulation(1.f / 60.f, 10);

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
    //printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
  }

  // update the ball
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[1];
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
  ball_position->x = float(trans.getOrigin().getX());
  ball_position->y = float(trans.getOrigin().getY());
  ball_position->z = float(trans.getOrigin().getZ());
}

void render()
{

  update();

  if( gRender )
  {
    // Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start2DDraw();
    // glColor4f(1.0, 1.0, 1.0, 1.0);
    // glBindTexture(GL_TEXTURE_2D, textures[2]);
    // glBegin( GL_QUADS );
    //   glTexCoord2f(0, 0); glVertex2f( 0, 0 );
    //   glTexCoord2f(0, 1); glVertex2f( 0, 600 );
    //   glTexCoord2f(1, 1); glVertex2f( 800, 600 );
    //   glTexCoord2f(1, 0); glVertex2f( 800, 0 );
    // glEnd();
    // End2DDraw();

    glLoadIdentity();
    gluLookAt(-8, -8, 8, 0, 0, 0, 0, 0, 1);

    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    for (auto surface = world.begin(); surface != world.end(); ++surface) {
      glBegin( GL_QUADS );
      

        glTexCoord2f(0, 0); glVertex3f( (*surface)->p1->x, (*surface)->p1->y, (*surface)->p1->z );
        glTexCoord2f(3, 0); glVertex3f( (*surface)->p2->x, (*surface)->p2->y, (*surface)->p2->z );
        glTexCoord2f(3, 3); glVertex3f( (*surface)->p3->x, (*surface)->p3->y, (*surface)->p3->z );
        glTexCoord2f(0, 3); glVertex3f( (*surface)->p4->x, (*surface)->p4->y, (*surface)->p4->z );



        // glTexCoord2f(0, 0); glVertex3f( -3, -3, 0 );
        // glTexCoord2f(3, 0); glVertex3f( 3, -3, 0 );
        // glTexCoord2f(3, 3); glVertex3f( 3, 3, 0);
        // glTexCoord2f(0, 3); glVertex3f( -3, 3, 0 );
      glEnd();
    }

    // glLoadIdentity();

    // gluLookAt(8, 8, 8, 0, 0, 0, 0, 0, 1);
    
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    //glColor4f(1.0, 1.0, 1.0, 0.5);

    //glRotatef(180, 0.0f, 1.0f, 0.0f);
    //glRotatef(-30, 1.0f, 0.0f, 0.0f);
    glTranslatef(ball_position->x, ball_position->y, ball_position->z);
    glRotatef(rotation_value, 0.0f, 0.0f, 1.0f);
    gluSphere(ball, 0.5, 20, 20);
    glTranslatef(-ball_position->x, -ball_position->y, -ball_position->z);


  }
}

void close()
{
  // Destroy window  
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  // Quit SDL subsystems
  SDL_Quit();

  // Clean up Bullet physics
  cleanupBullet();
}

void cleanupBullet()
{
  ///-----cleanup_start-----
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

void testBullet()
{
  ///create a few basic rigid bodies

  //the ground is a sheet of side 3 at position z = 0.
  {
    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(3.), btScalar(3.), btScalar(0.0)));

    collisionShapes.push_back(groundShape);

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, 0, 0));

    btScalar mass(0.);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
      groundShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setRestitution(1.0);

    //add the body to the dynamics world
    dynamicsWorld->addRigidBody(body);
  }

  {
    //create a dynamic rigidbody

    //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
    btCollisionShape* colShape = new btSphereShape(btScalar(0.5));
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

    startTransform.setOrigin(btVector3(0, 0, 3.5f));

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setRestitution(0.7);

    dynamicsWorld->addRigidBody(body);
  }


  // right side wall
  {
    btCollisionShape* rightWallShape = new btBoxShape(btVector3(btScalar(0.), btScalar(3.), btScalar(3.)));

    collisionShapes.push_back(rightWallShape);

    btTransform rightWallTransform;
    rightWallTransform.setIdentity();
    rightWallTransform.setOrigin(btVector3(3, 0, 3));

    btScalar mass(0.);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
      rightWallShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(rightWallTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, rightWallShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setRestitution(1.0);

    //add the body to the dynamics world
    dynamicsWorld->addRigidBody(body);
  }


  // back side wall
  {
    btCollisionShape* backWallShape = new btBoxShape(btVector3(btScalar(3.), btScalar(0), btScalar(3.)));

    collisionShapes.push_back(backWallShape);

    btTransform backWallTransform;
    backWallTransform.setIdentity();
    backWallTransform.setOrigin(btVector3(0, 3, 3));

    btScalar mass(0.);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
      backWallShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(backWallTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, backWallShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setRestitution(1.0);

    //add the body to the dynamics world
    dynamicsWorld->addRigidBody(body);
  }


  /// Do some simulation

  // int i;

  // ///-----stepsimulation_start-----
  // for (i = 0; i < 150; i++)
  // {
  //   dynamicsWorld->stepSimulation(1.f / 60.f, 10);

  //   //print positions of all objects
  //   for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
  //   {
  //     btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
  //     btRigidBody* body = btRigidBody::upcast(obj);
  //     btTransform trans;
  //     if (body && body->getMotionState())
  //     {
  //       body->getMotionState()->getWorldTransform(trans);
  //     }
  //     else
  //     {
  //       trans = obj->getWorldTransform();
  //     }
  //     printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
  //   }
  // }
}



int main( int argc, char* args[] )
{

  ball_position = new Point(0, 0, 3.5f);
  ball_velocity = new Point(0, 0, 0);
  
  // Floor
  world.push_front(
    new Surface( 
      new Point(-3, -3, 0),
      new Point(3, -3, 0),
      new Point(3, 3, 0),
      new Point(-3, 3, 0)
      
    )
  );

  // Right Side Wall
  world.push_front(
    new Surface(
      new Point(3, -3, 0),
      new Point(3, -3, 6),
      new Point(3, 3, 6),
      new Point(3, 3, 0)
    )
  );

  // Back Wall
  world.push_front(
    new Surface( 
      new Point(3, 3, 0),
      new Point(3, 3, 6),
      new Point(-3, 3, 6),
      new Point(-3, 3, 0)
    )
  );

  // front bumper:
  // 3,-3,0
  // 3,-3,1
  // 3,3,1
  // 3,3,0

  // left side bumper:
  // -3,-3,0
  // -3,-3,1
  // 3,-3,1
  // 3,-3,0
  
  // Start up SDL and create window
  if( !init() )
  {
    printf( "Failed to initialize!\n" );
  }
  else
  {
    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;
    
    // Enable text input
    SDL_StartTextInput();

    // Do a quick test bullet simulation
    testBullet();

    // While application is running
    while( !quit )
    {
      // Handle events on queue
      while( SDL_PollEvent( &e ) != 0 )
      {
        // User requests quit
        if( e.type == SDL_QUIT )
        {
          quit = true;
        }
        // Handle keypress with current mouse position
        else if( e.type == SDL_TEXTINPUT )
        {
          int x = 0, y = 0;
          SDL_GetMouseState( &x, &y );
          handleKeys( e.text.text[ 0 ], x, y );
        }
      }

      // Render quad
      render();
      
      // Update screen
      SDL_GL_SwapWindow( gWindow );
    }
    
    // Disable text input
    SDL_StopTextInput();
  }

  // Free resources and close SDL
  close();

  return 0;
}