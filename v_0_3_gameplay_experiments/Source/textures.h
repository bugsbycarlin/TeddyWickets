/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Textures class manages a set of textures.
*/

#ifndef TEDDY_WICKETS_TEXTURES_H_
#define TEDDY_WICKETS_TEXTURES_H_

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include <stdio.h>
#include <cstdlib>
#include <map>
#include <string>

#include "globals.h"
#include "graphics.h"

class Textures {
 public:
  Textures();

  std::map<std::string, int*> texture_map;
  std::string texture_root;

  void addTexture(std::string texture_name, std::string texture_file);

  void setTexture(std::string texture_name);
};

extern Textures* textures;

#endif
