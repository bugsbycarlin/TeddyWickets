/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Model Viewer class views 3d models in close up.
*/

#ifndef TEDDY_WICKETS_MODEL_VIEWER_H_
#define TEDDY_WICKETS_MODEL_VIEWER_H_

// SDL, for window, user input, and media
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_opengl.h>

// OpenGL, for drawing everything
#include <OpenGL/GLU.h>

// Fmod for sound
#include "fmod.hpp"

// Standard libraries
#include <math.h>
#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <list>
#include <string>

// Teddy Wickets includes
#include "globals.h"
#include "model.h"
#include "screen.h"
#include "textures.h"

class ModelViewer: public Screen {
 public:
  Textures* textures;

  float rotation;

  float zoom;

  bool mousedown;
  int drag_x;
  int drag_y;
  float pre_drag_rotation;

  Model* model;

  ModelViewer();

  void loop(SDL_Window* window, FMOD::System *sound_system);

  void update();

  void render();

  void handleKeys(unsigned char key);
  void handleMouse(SDL_Event e);

  bool initialize();
  void shutdown();
};

#endif
