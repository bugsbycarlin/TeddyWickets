/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  CelShader class loads GLSL cel shader and can turn it on for renders.
*/

#ifndef TEDDY_WICKETS_CEL_SHADER_H_
#define TEDDY_WICKETS_CEL_SHADER_H_

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <map>
#include <string>

#include "globals.h"
#include "teddy_gl.h"

class CelShader {
 public:
  CelShader();

  GLint program;

  void setShader();
  void unsetShader();
};

#endif