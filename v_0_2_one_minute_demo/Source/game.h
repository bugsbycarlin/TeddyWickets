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

// Standard libraries
#include <math.h>
#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <list>
#include <string>
#include <iomanip>
#include <sstream>

// Fmod for sound
#include "fmod.hpp"

// TinyXML for level file reading
#include "tinyxml2.h"

// Teddy Wickets includes
#include "hot_config.h"
#include "control_map.h"
#include "character.h"
#include "globals.h"
#include "graphics.h"
#include "physics.h"
#include "screen.h"
#include "textbox.h"
#include "textures.h"
#include "hazard.h"
#include "wicket.h"

class Game: public Screen {
 public:

  Physics* physics;

  ControlMap* control_map;

  std::list<Hazard*> hazards = { };

  std::vector<Character*> characters = { };
  std::vector<Hazard*> starts = { };
  std::vector<Wicket*> wickets = { };

  Character* current_character;

  bool shot_rising = false;

  int current_character_number;

  int game_mode;

  int player_1_score;
  int player_1_display_score;
  int player_2_score;
  int player_2_display_score;

  float zoom;

  float sway;

  float default_speed_ramping;
  float simulation_speed;

  int hud_step;

  std::string theme;

  Model* theme_tile;

  std::string map_file;

  TextBox* bear_velocity_1;
  TextBox* bear_velocity_2;
  TextBox* bear_velocity_3;
  TextBox* bear_velocity_4;
  TextBox* bear_velocity_5;
  TextBox* bear_velocity_6;

  TextBox* player_1_score_box;
  TextBox* player_2_score_box;

  std::vector<std::string> player_1_bears;
  std::vector<std::string> player_2_bears;

  Game(std::vector<std::string> player_1_bears, std::vector<std::string> player_2_bears);

  void loop(SDL_Window* window, FMOD::System *sound_system);

  void update();
  void afterUpdate();

  void shoot();

  void render();
  void renderBackground();

  void handleKeys(SDL_Event e);
  void handleController(SDL_Event e);
  void handleAction(std::string action);

  bool initialize();
  bool initializeGamePieces();
  bool initializeTextures();
  void shutdown();
};

#endif
