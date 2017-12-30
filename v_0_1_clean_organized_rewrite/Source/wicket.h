/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Wicket class models in-game wickets.
*/

#ifndef TEDDY_WICKETS_WICKET_H_
#define TEDDY_WICKETS_WICKET_H_

#include <OpenGL/GLU.h>

#include "physics.h"
#include "point.h"
#include "textures.h"

class Wicket {
 public:
    int identity;

    GLUquadric* pole;
    GLUquadric* pole_cap;

    Point* pole_1_position;
    Point* pole_2_position;
    float height;

    Physics* physics;

    Wicket(Physics* physics, Point* pole_1_position, Point* pole_2_position, float height);

    void render();
};

#endif
