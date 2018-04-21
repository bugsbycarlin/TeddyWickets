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
#include "glm/gtx/string_cast.hpp"

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <list>
#include <unordered_map>
#include <string>
#include <vector>

#include "globals.h"
#include "colors.h"

class Graphics {
 public:
  Graphics();

  //int cel_shader_program;
  int shader_program;
  //int current_program;

  int next_display_list_index;
  int next_mesh_cache_id;

  std::unordered_map<int, GLuint> vertex_buffers;
  std::unordered_map<int, GLuint> normal_buffers;
  std::unordered_map<int, GLuint> texture_buffers;
  std::unordered_map<int, GLuint> color_buffers;
  std::unordered_map<int, int> buffer_sizes;

  int* white_texture_id;
  int* black_texture_id;

  GLuint rectangle_vertex_buffer;
  GLuint rectangle_normal_buffer;
  GLuint rectangle_color_buffer;
  GLuint rectangle_texture_buffer;

  GLuint primitive_vertex_buffer;
  GLuint primitive_normal_buffer;
  GLuint primitive_color_buffer;
  GLuint primitive_texture_buffer;

  GLuint vertex_array_id;
  GLuint mvp_matrix_id;
  GLuint v_matrix_id;
  GLuint m_matrix_id;
  GLuint global_color_id;
  GLuint texture_sampler_id;
  GLuint bool_cel_shading_id;
  GLuint bool_lighting_id;
  GLuint light_position_worldspace_id;

  glm::mat4 projection;
  glm::mat4 view;
  glm::mat4 model;
  glm::vec4 global_color_vector;

  std::vector<glm::mat4> model_stack;

  std::unordered_map<std::string, int> rectangle_cache;

  bool initialized_rectangle_buffers;

  bool using_2d;

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
  
  void initializeNormalShading();

  void enableCelShading();
  void disableCelShading();
  void enableLights();
  void disableLights();

  void initializeOpenGLVersion();

  void clearScreen();
  void clearScreenWithColor(float r, float g, float b, float a);
  void set3d(float zoom);
  void standardCamera(float cam_x, float cam_y, float cam_z, float target_x, float target_y, float target_z);
  void standardLightPosition();
  void setLightPosition(float x, float y, float z);

  int* makeTexture(int w, int h, const GLvoid * pixels, bool soften);
  void setTexture(int* texture);

  void matteMaterial();

  void rectangle(float x, float y, float w, float h);
  void rectangleWithTexture(float vertex_data[], float texture_data[]);

  int cacheMesh(int size, float vertex_data[], float normal_data[], float texture_data[]);
  void drawMesh(int cache_id);

  int cacheFullMesh(std::vector<float> vertex_data, std::vector<float> normal_data, std::vector<float> texture_data, std::vector<float> color_data);
  void drawFullMesh(int cache_id);

  int cacheRectangle(float w, float h);
  void rectangle(int cache_id);

  void lineWidth(int line_width);
  void lineStrip(std::vector<float> line_data);

  void color(float r, float g, float b, float a);
  void rotate(float angle, float x, float y, float z);
  void scale(float x, float y, float z);
  void translate(float x, float y, float z);

  void pushModelMatrix();
  void popModelMatrix();
  void multMatrix(const float* m);

  glm::vec3 get2dCoords(float x, float y, float z);
  void printModel();
};

extern Graphics* graphics;

#endif

