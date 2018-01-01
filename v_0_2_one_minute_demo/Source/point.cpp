/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "point.h"

Point::Point() {
}

Point::Point(float x, float y, float z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

void Point::vertex() {
  glVertex3f(x, y, z);
}

void Point::add(Point* p2) {
  this->x += p2->x;
  this->y += p2->y;
  this->z += p2->z;
}
