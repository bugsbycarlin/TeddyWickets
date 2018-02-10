/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Wicket class models in-game wickets.
*/

#ifndef TEDDY_WICKETS_WICKET_H_
#define TEDDY_WICKETS_WICKET_H_

#include <math.h>

#include "physics.h"
#include "point.h"
#include "model.h"
#include "textures.h"
#include "triangle.h"

class Wicket {
 public:
    int identity;
    Point* position;
    float rotation;

    Physics* physics;
    Textures* textures;
    Model* model;

    Wicket(Physics* physics, Textures* textures, Point* position, float rotation);

    void render();
};

#endif
