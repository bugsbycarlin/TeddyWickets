/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Surface class models an in-game surface, such as a wall or a floor.
*/

#ifndef TEDDY_WICKETS_SURFACE_H_
#define TEDDY_WICKETS_SURFACE_H_

#include <stdio.h>
#include <cstdlib>

#include "physics.h"
#include "point.h"
#include "textures.h"

class Surface {
 public:
    Point* p1;
    Point* p2;
    Point* p3;
    Point* p4;
    Point* normal;

    Physics* physics;

    float color_r;
    float color_g;
    float color_b;

    bool use_grey;

    Surface(Physics* physics, bool use_grey, Point* p1, Point* p2, Point* p3, Point* p4);

    void render();
};

#endif
