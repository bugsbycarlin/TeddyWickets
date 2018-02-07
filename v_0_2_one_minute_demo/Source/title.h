/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Title class shows the title screen.
*/

#ifndef TEDDY_WICKETS_TITLE_H_
#define TEDDY_WICKETS_TITLE_H_

// SDL, for window, user input, and media
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

// Standard libraries
#include <math.h>
#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <string>

// Fmod for sound
#include "fmod.hpp"

// Teddy Wickets includes
#include "globals.h"
#include "screen.h"
#include "textbox.h"
#include "textures.h"


class Title: public Screen {
 public:
  int selection;
  int mode;

  Textures* textures;

  TextBox* bugsby_presents;

  TextBox* title;

  TextBox* one_player_selected;
  TextBox* one_player_unselected;
  TextBox* two_player_selected;
  TextBox* two_player_unselected;
  TextBox* control_setup_selected;
  TextBox* control_setup_unselected;

  unsigned long start_time;
  unsigned long last_time;

  Title();

  void loop(SDL_Window* window, FMOD::System *sound_system);

  void update();

  void render();

  void handleKeys(SDL_Event e);
  void handleMouse(SDL_Event e);

  bool initialize();
  void shutdown();
};

#endif
