/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Control Map is a map of keys and gamepad actions to controls.
*/

#ifndef TEDDY_WICKETS_CONTROL_MAP_H_
#define TEDDY_WICKETS_CONTROL_MAP_H_

// SDL, for window, user input, and media
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

// Standard libraries
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <list>
#include <map>
#include <regex>
#include <string>

// Teddy Wickets includes
#include "globals.h"

class ControlMap {
 public:
  // Control map and default control map
  std::map<std::string, std::string> control_map;
  std::map<std::string, std::string> default_control_map;
  std::list<std::string> ordered_controls;
  std::map<std::string, std::string> new_control_map;

  ControlMap();

  std::string getAction(std::string input);
  void swapMapsAndSave();

  std::string translateControllerEvent(SDL_Event e);
  std::string translateKeyEvent(SDL_Event e);
};

#endif
