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
  unsigned long mark_time;
  unsigned long framerate_time;
  int frames_since_last;

  // Bear select stuff
  TextBox* choose_your_bears_text;
  TextBox* player_1_choose_text;
  TextBox* player_2_choose_text;
  TextBox* bear_name_text;
  TextBox* bear_description_text;
  TextBox* go_text;

  int bear_choice;

  std::map<int, std::string> bear_choices = {
    {0, "lil_jon"},
    {1, "mortimer"},
    {2, "gluke"},
    {3, "mags"},
    {4, "bob_smith"},
    {5, "lord_lonsdale"},
    {6, "hpf_swinnerton_dyer"},
    {7, "jeff_bridges"},
    {8, "grim"}
  };

  std::map<int, int> available_bear_choices;

  std::map<std::string, std::string> bear_pretty_names = {
    {"lil_jon", "Lil' Jon"},
    {"mortimer", "Mortimer"},
    {"gluke", "Gluke"},
    {"mags", "Mags"},
    {"bob_smith", "Bob Smith"},
    {"lord_lonsdale", "Lord Lonsdale"},
    {"hpf_swinnerton_dyer", "H.P.F. Swinnerton-Dyer"},
    {"jeff_bridges", "Jeff Bridges"},
    {"grim", "Grim"}
  };

  std::map<std::string, std::string> bear_descriptions = {
    {"lil_jon", "Extra bouncy"},
    {"mortimer", "Changes the trajectory of other bears"},
    {"gluke", "Can teleport randomly"},
    {"mags", "Magnetically attracted to wickets"},
    {"bob_smith", "Other nearby bears get sad and slow down"},
    {"lord_lonsdale", "Sticks in the ground on a high shot"},
    {"hpf_swinnerton_dyer", "Uses crampons to stop on a dime"},
    {"jeff_bridges", "Leaves a deadly digital trail"},
    {"grim", "Knocks other bears out of commission"}
  };

  std::vector<std::string> player_1_bears;
  std::vector<std::string> player_2_bears;

  Game();

  void loop(SDL_Window* window, FMOD::System *sound_system);

  void update();
  void afterUpdate();

  void shoot();

  void render();
  void renderBackground();
  void renderBearSelectMode();
  void renderDesign();

  void handleKeys(SDL_Event e);
  void handleMouse(SDL_Event e);

  bool initialize();
  bool initializeGamePieces();
  bool initializeTextures();
  void shutdown();
};

#endif
