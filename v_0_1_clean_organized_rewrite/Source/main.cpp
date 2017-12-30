/*

  Teddy Wickets
  Matthew Carlin

  October 2017 - V0.0 -  First sketch.

  Testing four things:

  - SDL framework
  - 2d sprite ball on either a 2d world or a 3d ortho world
  - Keyboard input to move the ball
  - Bullet Physics engine


  December 2017 - V0.1 - Clean Organized Rewrite

  This is a clean and organized rewrite of the V0.0 sketch, with additional changes:

  +- makefile
  +- make a larger level and (limited) camera tracking
  +- make the ball roll
  +- add a reasonable looking amount of friction
  +- add simple mountain underneath the level
  +- add bumpers
  +- add music to make sure there are no issues there
  +- clear up the color issue with png files
  +- get the background and a token piece of heads up display working
  +- make the 3d bear a little cuter
  +- add lighting
  +- get orthographic perspective working
  +- add a reset mechanic for when the bear stops moving
  +- make the bumpers light up when bumped
  +- take mouse input with SDL
  +- add a shot mechanic for keyboard
  +- add prediction arrows to show the direction of the shot based on a forward bullet simulation
  +- rename textures and refactor texture loading
  +- make physics updates use time instead of framerate
  +- display control information
  +- add a shot mechanic for mouse 
  +- add some obstacles to hit or avoid with the ball
  - lint the code for style
  - submit code review (thanks, Avi Robinson-Mosher)


  Saving these tasks for later versions:
  - add cel shading
  - get several options for the ball sprite

*/

#include "game.h"
#include "fmod.hpp"
#include "fmod_errors.h"

// The window
SDL_Window* gWindow;

// The OpenGL context
SDL_GLContext gContext;

// Music
FMOD_RESULT result;
FMOD::Sound *audioStream;
FMOD::System *sound_system = NULL;

bool initializeOpenGL() {
  // Initialize Projection Matrix
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  // Initialize Modelview Matrix
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  
  // Initialize clear color
  glClearColor( 0.9f, 0.9f, 0.9f, 1.f );
  
  // Check for error
  GLenum error = glGetError();
  if( error != GL_NO_ERROR )
  {
    printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
    return false;
  }

  return true;
}

bool initialize() {
   // Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Use OpenGL 2.1
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  // Create window
  gWindow = SDL_CreateWindow("Teddy Wickets", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if(gWindow == NULL)
  {
    printf("Window could not be created. SDL Error: %s\n", SDL_GetError());
    return false;
  }
  
  // Create context
  gContext = SDL_GL_CreateContext( gWindow );
  if(gContext == NULL)
  {
    printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Use Vsync
  if(SDL_GL_SetSwapInterval(1) < 0)
  {
    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Initialize OpenGL
  if(!initializeOpenGL())
  {
    printf("Unable to initialize OpenGL!\n");
    return false;
  }

  // Initialize Audio
  result = FMOD::System_Create(&sound_system);      // Create the main sound_system object.
  if (result != FMOD_OK)
  {
      printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
      return false;
  }

  result = sound_system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
  if (result != FMOD_OK)
  {
      printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
      return false;
  }
}

void shutdown() {
  // Destroy window  
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;

  sound_system->close();

  // Quit SDL subsystems
  SDL_Quit();
}

int main( int argc, char* args[] ) {
  initialize();

  sound_system->createStream("Sound/Valiance.mp3", FMOD_DEFAULT, 0, &audioStream);

  //sound_system->playSound(audioStream, NULL, false, 0);

  Game* game = new Game();
  bool initialized = game->initialize();
  if (initialized) {
    game->game_loop(gWindow,sound_system);
  }

  shutdown();
}