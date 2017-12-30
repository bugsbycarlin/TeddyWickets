/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Game class is models one level of the game.
*/

#ifndef TEDDY_WICKETS_GAME_H_
#define TEDDY_WICKETS_GAME_H_

// SDL, for window, user input, and media
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_opengl.h>

// OpenGL, for drawing everything
#include <OpenGL/GLU.h>

// Standard libraries
#include <math.h>
#include <stdio.h>
#include <string>
#include <list>
#include <cstdlib>
#include <chrono>

// Fmod for sound
#include "fmod.hpp"

// Teddy Wickets includes
#include "globals.h"
#include "surface.h"
#include "physics.h"
#include "character.h"
#include "bumper.h"
#include "wicket.h"
#include "textures.h"

class Game {
 public:
  bool quit;

  enum  {
    DROP_MODE = 0,
    PREP_MODE = 1,
    POWER_MODE = 2,
    ACTION_MODE = 3
  };

  bool shot_rising = false;

  // Game logic
  std::list<Surface*> surfaces = { };
  std::list<Bumper*> bumpers = { };
  std::list<Wicket*> wickets = { };

  Physics* physics;

  Character* character;

  int game_mode;

  bool mousedown;
  int drag_x;
  int drag_y;
  float pre_drag_rotation;

  unsigned long startTime;
  unsigned long lastTime;

  // Function declarations
  Game();

  void game_loop(SDL_Window* window, FMOD::System *sound_system);

  void update();
  void postUpdate();
  void render();
  void renderDesign();
  void renderBackground();
  void shutdown();
  void handleKeys(unsigned char key);
  void handleMouse(SDL_Event e);


  bool initialize();
  bool initializeGameLogic();
  bool initializeTextures();
  bool initializeLighting();
};

#endif
