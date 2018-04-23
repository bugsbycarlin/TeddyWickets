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

// Standard libraries
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <map>
#include <string>

// Teddy Wickets includes
#include "hot_config.h"
#include "control_map.h"
#include "globals.h"
#include "graphics.h"
#include "screen.h"
#include "textbox.h"
#include "textures.h"

class ControlSetup: public Screen {
 public:
  int mode;

  TextBox* view_mode_instruction;
  TextBox* configure_mode_instruction;
  TextBox* return_to_title_instruction;

  TextBox* player_1_up;
  TextBox* player_1_down;
  TextBox* player_1_left;
  TextBox* player_1_right;
  TextBox* player_1_shoot_accept;
  TextBox* player_1_cancel_switch;
  TextBox* player_1_special;
  TextBox* player_1_view_taunt;
  TextBox* player_2_up;
  TextBox* player_2_down;
  TextBox* player_2_left;
  TextBox* player_2_right;
  TextBox* player_2_shoot_accept;
  TextBox* player_2_cancel_switch;
  TextBox* player_2_special;
  TextBox* player_2_view_taunt;

  std::list<std::string>::iterator control_iterator;

  std::map<std::string, TextBox*> action_map;

  ControlMap* control_map;

  ControlSetup();

  void loop(SDL_Window* window, FMOD::System *sound_system);

  void update();

  void render();

  void handleKeys(SDL_Event e);
  void handleMouse(SDL_Event e);
  void handleController(SDL_Event e);

  void setTextToMap();
  void setEmptyTextToMap();

  bool initialize();
  void shutdown();
};

#endif
