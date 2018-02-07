/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  CelShader class loads GLSL cel shader and can turn it on for renders.
*/

#ifndef TEDDY_WICKETS_CEL_SHADER_H_
#define TEDDY_WICKETS_CEL_SHADER_H_

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
//#include <SDL2/SDL_opengl.h>

//#include <OpenGL/gl3.h>
//#include <OpenGL/GLU.h>


#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <map>
#include <string>

#include "globals.h"

class CelShader {
 public:
  CelShader();

  GLint program;

  void setShader();
  void unsetShader();
};

#endif
