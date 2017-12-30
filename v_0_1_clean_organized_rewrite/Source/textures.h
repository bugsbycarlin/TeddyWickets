#ifndef TEXTURES_H
#define TEXTURES_H

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_opengl.h>

#include <OpenGL/GLU.h>

#include <cstdlib>
#include <map>
#include <string>
#include <stdio.h>

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