#ifndef POINT_H
#define POINT_H

#include <SDL2/SDL_opengl.h>

class Point
{
public:
  float x,y,z;

  Point();
  
  Point(float x,float y,float z);
  
  void v();

  void add(Point* p2);
};

#endif