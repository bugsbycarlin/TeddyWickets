/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Level editor.
*/

#ifndef TEDDY_WICKETS_EDITOR_H_
#define TEDDY_WICKETS_EDITOR_H_

// SDL, for window, user input, and media
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

// Standard libraries
#include <math.h>
#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <list>
#include <string>

// Teddy Wickets includes
#include "bumper.h"
#include "character.h"
#include "globals.h"
#include "graphics.h"
#include "physics.h"
#include "screen.h"
#include "surface.h"
#include "textbox.h"
#include "textures.h"
#include "hazard.h"

class Editor: public Screen {
 public:
  // Stuff
  std::list<Surface*> surfaces = { };
  std::list<Bumper*> bumpers = { };
  std::list<Hazard*> hazards = { };

  Physics* physics;

  unsigned long start_time;
  unsigned long last_time;
  unsigned long mark_time;
  unsigned long framerate_time;
  int frames_since_last;

  Editor();

  void loop(SDL_Window* window, FMOD::System *sound_system);

  void update();

  void render();
  void renderBackground();

  void handleKeys(SDL_Event e);

  bool initialize();
  bool initializeGamePieces();
  bool initializeTextures();
  void shutdown();
};

#endif
