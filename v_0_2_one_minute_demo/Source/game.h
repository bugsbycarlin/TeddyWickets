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

class Game: public Screen {
 public:

  Physics* physics;

  ControlMap* control_map;

  std::list<Hazard*> hazards = { };

  std::vector<Character*> characters = { };

  Character* current_character;

  bool shot_rising = false;

  int current_character_number;

  int game_mode;

  float zoom;

  float sway;

  float default_speed_ramping;
  float simulation_speed;

  std::string music;
  std::string theme;

  Model* theme_tile;

  std::string map_file;

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
