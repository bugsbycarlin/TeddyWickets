/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Bumper class models an in-game bumper.
*/

#ifndef TEDDY_WICKETS_BUMPER_H_
#define TEDDY_WICKETS_BUMPER_H_

#include <OpenGL/GLU.h>

#include "globals.h"
#include "physics.h"
#include "point.h"
#include "textures.h"

class Bumper {
 public:
    Point* start;
    Point* end;
    Point* extrusion;

    Physics* physics;
    Textures* textures;

    int identity;

    int last_bumped;

    bool use_grey;

    Bumper(Physics* physics, Textures* textures, Point* start, Point* end, Point* extrusion);

    void bump();

    void update();

    void render();
};

#endif
