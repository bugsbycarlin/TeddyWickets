/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Bear select screen. That's where you select your bears.
*/

#ifndef TEDDY_WICKETS_BEAR_SELECT_H_
#define TEDDY_WICKETS_BEAR_SELECT_H_

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
#include "control_map.h"
#include "globals.h"
#include "graphics.h"
#include "screen.h"
#include "textbox.h"
#include "textures.h"

class BearSelect: public Screen {
 public:

  int game_mode;

  ControlMap* control_map;

  // Bear select stuff
  TextBox* choose_your_bears_text;
  TextBox* player_1_choose_text;
  TextBox* player_2_choose_text;
  TextBox* bear_name_text;
  TextBox* bear_description_text;
  TextBox* go_text;

  int bear_choice;

  int first;

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
    {"lil_jon", "Lil Bounce Bear"},
    {"mortimer", "Boss Bear"},
    {"gluke", "Glitchy Bear"},
    {"mags", "Mags"},
    {"bob_smith", "Emo Bob"},
    {"lord_lonsdale", "Lord Dartsdale"},
    {"hpf_swinnerton_dyer", "H.P.F. Swinnerton-Dyer"},
    {"jeff_bridges", "Jeff Bridges"},
    {"grim", "Grim Bear"}
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

  BearSelect();

  void loop(SDL_Window* window, FMOD::System *sound_system);

  void update();

  void render();

  void handleKeys(SDL_Event e);
  void handleController(SDL_Event e);
  void handleAction(std::string action);

  bool initialize();
  bool initializeTextures();
  void shutdown();
};

#endif
