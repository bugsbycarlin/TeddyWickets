/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Game class models one level of the game.
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
#include <chrono>
#include <cstdlib>
#include <list>
#include <string>

// Fmod for sound
#include "fmod.hpp"

// Teddy Wickets includes
#include "bumper.h"
#include "character.h"
#include "globals.h"
#include "physics.h"
#include "screen.h"
#include "surface.h"
#include "textures.h"
#include "wicket.h"

class Game: public Screen {
 public:
  bool shot_rising = false;

  // Game logic
  std::list<Surface*> surfaces = { };
  std::list<Bumper*> bumpers = { };
  std::list<Wicket*> wickets = { };

  Physics* physics;
  Textures* textures;

  Character* character;

  int game_mode;

  float default_speed_ramping;
  float simulation_speed;

  float zoom;

  bool mousedown;
  int drag_x;
  int drag_y;
  float pre_drag_rotation;

  unsigned long start_time;
  unsigned long last_time;

  Game();

  void loop(SDL_Window* window, FMOD::System *sound_system);

  void update();
  void afterUpdate();

  void shoot();

  void render();
  void renderDesign();
  void renderBackground();

  void handleKeys(unsigned char key);
  void handleMouse(SDL_Event e);

  bool initialize();
  bool initializeGamePieces();
  bool initializeTextures();
  bool initializeLighting();
  void shutdown();
};

#endif
