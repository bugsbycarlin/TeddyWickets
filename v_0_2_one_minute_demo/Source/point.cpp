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

Point::Point(Point* p) {
  this->x = p->x;
  this->y = p->y;
  this->z = p->z;
}

void Point::add(Point* p2) {
  this->x += p2->x;
  this->y += p2->y;
  this->z += p2->z;
}
