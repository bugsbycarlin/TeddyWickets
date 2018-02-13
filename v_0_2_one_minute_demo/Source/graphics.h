/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Graphics class abstracts away OpenGL.
*/

#ifndef TEDDY_WICKETS_graphics_H_
#define TEDDY_WICKETS_graphics_H_

#define GL3_PROTOTYPES 1

// Various forms of OpenGL
// #include <SDL2/SDL_opengl.h>
#include <OpenGL/gl3.h>
// #include <OpenGL/gl.h>

// DO NOT
// #include <OpenGL/GLU.h>

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <list>
#include <string>
#include <vector>

#include "globals.h"

class Graphics {
 public:
  Graphics();

  int cel_shader_program;
  int next_display_list_index;

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

  void initializeOpenGLVersion();

  void clearScreen();
  void clearScreenWithColor(float r, float g, float b, float a);
  void set3d(float zoom);
  void standardCamera(float cam_x, float cam_y, float cam_z, float target_x, float target_y, float target_z);
  void standardLightPosition();

  int* makeTexture(int w, int h, const GLvoid * pixels, bool soften);
  void setTexture(int* texture);

  void matteMaterial();

  void texVert(float t1, float t2, float v1, float v2, float v3);
  void texNormVert(float t1, float t2, float n1, float n2, float n3, float v1, float v2, float v3);
  void face(int size, float data[]);
  void face2d(double data[]);

  void lineWidth(int line_width);
  void lineStrip(std::vector<float> line_data);

  void sphere(float radius);

  void color(float r, float g, float b, float a);
  void rotate(float angle, float x, float y, float z);
  void scale(float x, float y, float z);
  void translate(float x, float y, float z);

  void pushMatrix();
  void popMatrix();
  void multMatrix(const float* m);

  int cacheProgram();
  void endCacheProgram();
  void runCacheProgram(int id);

  void crossProduct(float x1, float y1, float z1, float x2, float y2, float z2, float res[3]);
  void lookAt(float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ);
};

extern Graphics* graphics;

#endif

