/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Control Setup class shows the screen where you set up game controls.
*/

#ifndef TEDDY_WICKETS_CONTROL_SETUP_H_
#define TEDDY_WICKETS_CONTROL_SETUP_H_

// SDL, for window, user input, and media
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>

// OpenGL, for drawing everything
#include <OpenGL/GLU.h>

// Standard libraries
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <string>

// Teddy Wickets includes
#include "globals.h"
#include "screen.h"
#include "textbox.h"
#include "textures.h"

class ControlSetup: public Screen {
 public:
  int mode;

  Textures* textures;

  TextBox* view_mode_instruction;
  TextBox* configure_mode_instruction;

  ControlSetup();

  void loop(SDL_Window* window, FMOD::System *sound_system);

  void update();

  void render();

  void handleKeys(SDL_Event e);
  void handleMouse(SDL_Event e);
  void handleController(SDL_Event e);

  bool initialize();
  void shutdown();
};

#endif
