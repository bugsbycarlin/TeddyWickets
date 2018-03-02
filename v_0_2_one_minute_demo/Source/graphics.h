/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Graphics class abstracts away OpenGL.
*/

#ifndef TEDDY_WICKETS_graphics_H_
#define TEDDY_WICKETS_graphics_H_

#define GL3_PROTOTYPES 1

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

// Various forms of OpenGL
// #include <SDL2/SDL_opengl.h>
// #include <OpenGL/gl3.h>
#include <OpenGL/gl3.h>

// DO NOT
// #include <OpenGL/GLU.h>

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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
  int normal_shader_program;
  int current_program;
  int next_display_list_index;

  int* white_texture_id;
  int* black_texture_id;

  GLuint rectangle_vertex_buffer;
  GLuint rectangle_color_buffer;
  GLuint rectangle_texture_buffer;

  // Um. Ignoring normals for now.
  GLuint primitive_vertex_buffer;
  GLuint primitive_normal_buffer;
  GLuint primitive_color_buffer;
  GLuint primitive_texture_buffer;

  GLuint vertex_array_id;
  GLuint matrix_id;
  GLuint global_color_id;
  GLuint texture_sampler_id;

  glm::mat4 projection;
  glm::mat4 view;
  glm::mat4 model;
  glm::vec4 global_color_vector;

  void start2DDraw();
  void end2DDraw();

  void fadeInOut(float start, float finish, float timeDiff);
  void fadeIn(float start, float finish, float timeDiff);
  void fadeOut(float start, float finish, float timeDiff);
  void blackout();

  void initialize();
  void initializeBasic();
  void initializeShadersAndLighting();
  void initializeBuffersAndGeometry();
  
  void initializeCelShading();
  void startCelShading();
  void initializeNormalShading();
  void startNormalShading();
  void clearShading();

  void initializeOpenGLVersion();

  void clearScreen();
  void clearScreenWithColor(float r, float g, float b, float a);
  void set3d(float zoom);
  void standardCamera(float cam_x, float cam_y, float cam_z, float target_x, float target_y, float target_z);
  void standardLightPosition();

  int* makeTexture(int w, int h, const GLvoid * pixels, bool soften);
  void setTexture(int* texture);

  void matteMaterial();

  //void texVert(float t1, float t2, float v1, float v2, float v3);
  //void texNormVert(float t1, float t2, float n1, float n2, float n3, float v1, float v2, float v3);

  void primitive(int size, float vertex_data[], float normal_data[], float texture_data[]);
  void rectangle(float x, float y, float w, float h);
  void rectangleWithTexture(float vertex_data[], float texture_data[]);

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

  //void crossProduct(float x1, float y1, float z1, float x2, float y2, float z2, float res[3]);
  //void lookAt(float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ);
};

extern Graphics* graphics;

#endif

