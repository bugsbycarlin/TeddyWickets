/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Textures class manages a set of textures.
*/

#ifndef TEDDY_WICKETS_TEXTURES_H_
#define TEDDY_WICKETS_TEXTURES_H_

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
//#include <SDL2/SDL_opengl.h>

//#include <OpenGL/gl3.h>
//#include <OpenGL/GLU.h>

#include <stdio.h>
#include <cstdlib>
#include <map>
#include <string>

#include "globals.h"

class Textures {
 public:
  Textures();

  std::map<std::string, GLuint*> texture_map;
  std::string texture_root;

  void addTexture(std::string texture_name, std::string texture_file);

  void setTexture(std::string texture_name);
};

#endif
