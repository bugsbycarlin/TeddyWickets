/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Hazard class models in-game physical objects, including the physical parts of
  active objects such as wickets and moving platforms.
*/

#ifndef TEDDY_WICKETS_HAZARD_H_
#define TEDDY_WICKETS_HAZARD_H_

#include <math.h>
#include <string>

#include "physics.h"
#include "point.h"
#include "model_cache.h"
#include "textures.h"
#include "triangle.h"

class Hazard {
 public:
    int identity;
    Point* position;
    float rotation;

    std::string object_type;

    Physics* physics;
    Model* model;

    Hazard(std::string object_type, Physics* physics, Point* position, float rotation);

    void render();
};

#endif
