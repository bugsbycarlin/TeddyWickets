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

  void clearScreen();
  void set3d(float zoom);
  void standardCamera(float cam_x, float cam_y, float cam_z, float target_x, float target_y, float target_z);
  void standardLightPosition();

  int* makeTexture(int w, int h, const GLvoid * pixels, bool soften);
  void setTexture(int* texture);

  void texVert(float t1, float t2, float v1, float v2, float v3);
  void texNormVert(float t1, float t2, float n1, float n2, float n3, float v1, float v2, float v3);
  void face(int size, float data[]);
  void face2d(double data[]);
};

extern TeddyGL* teddy_gl;

#endif

