/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "triangle.h"

Triangle::Triangle() {
}

Triangle::Triangle(Point* p1, Point* p2, Point* p3) {
  this->p1 = new Point(p1);
  this->p2 = new Point(p2);
  this->p3 = new Point(p3);
}

