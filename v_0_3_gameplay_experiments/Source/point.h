/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Point class models a point in space, or a vector.
*/

#ifndef TEDDY_WICKETS_POINT_H_
#define TEDDY_WICKETS_POINT_H_

#include <math.h>

#include "globals.h"

class Point {
 public:
  float x, y, z;

  Point();

  Point(float x, float y, float z);

  Point(Point* p);

  float magnitude();

  void add(Point* p2);

  float distance(Point* p2);
};

#endif
