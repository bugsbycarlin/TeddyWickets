/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Textures class manages a set of textures.
*/

#ifndef TEDDY_WICKETS_TEXTURES_H_
#define TEDDY_WICKETS_TEXTURES_H_

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_opengl.h>

#include <OpenGL/GLU.h>

#include <stdio.h>
#include <cstdlib>
#include <map>
#include <string>

class Textures {
 public:
  static Textures& initialize() {
    static Textures instance;
    return instance;
  }

  static std::map<std::string, GLuint*> texture_map;
  static std::string texture_root;

  static void addTexture(std::string texture_name, std::string texture_file);

  static void setTexture(std::string texture_name);

 private:
  Textures();
};

#endif
