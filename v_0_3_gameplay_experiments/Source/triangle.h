/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Triangle class models a triangle.
*/

#ifndef TEDDY_WICKETS_TRIANGLE_H_
#define TEDDY_WICKETS_TRIANGLE_H_

#include "globals.h"
#include "graphics.h"
#include "point.h"

class Triangle {
 public:
  Point* p1;
  Point* p2;
  Point* p3;

  Triangle();

  Triangle(Point* p1, Point* p2, Point* p3);
};

#endif
