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

#include "globals.h"

class TeddyGL {
 public:
  TeddyGL();

  void drawRectangle(float x, float y, float w, float h) const;

  void start2DDraw() const;
  void end2DDraw() const;

  void fadeInOut(float start, float finish, float timeDiff) const;
  void fadeIn(float start, float finish, float timeDiff) const;
  void fadeOut(float start, float finish, float timeDiff) const;
  void blackout() const;
};

extern const TeddyGL* teddy_gl;

#endif

