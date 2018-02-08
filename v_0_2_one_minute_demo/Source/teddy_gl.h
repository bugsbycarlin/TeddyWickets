/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  TeddyGL class abstracts away OpenGL.
*/

#ifndef TEDDY_WICKETS_TEDDY_GL_H_
#define TEDDY_WICKETS_TEDDY_GL_H_

// Various forms of OpenGL
// #include <SDL2/SDL_opengl.h>
// #include <OpenGL/gl3.h>
#include <OpenGL/gl.h>
#include <OpenGL/GLU.h>

#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <string>

#include "globals.h"

class TeddyGL {
 public:
  TeddyGL();

  int cel_shader_program;

  void drawRectangle(float x, float y, float w, float h);

  void start2DDraw();
  void end2DDraw();

  void fadeInOut(float start, float finish, float timeDiff);
  void fadeIn(float start, float finish, float timeDiff);
  void fadeOut(float start, float finish, float timeDiff);
  void blackout();

  void initializeBasic();
  void initializeLighting();
  void initializeCelShading();
  void startCelShading();
  void stopCelShading();

  GLuint* makeTexture(int w, int h, const GLvoid * pixels, bool soften);
};

extern TeddyGL* teddy_gl;

#endif

