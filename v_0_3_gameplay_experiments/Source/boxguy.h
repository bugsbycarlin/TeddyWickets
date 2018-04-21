/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Subclass of hazard which models a boxguy, including motion and squishy rendering.
*/

#ifndef TEDDY_WICKETS_BOXGUY_H_
#define TEDDY_WICKETS_BOXGUY_H_

#include <math.h>
#include <string>

#include "glm/glm.hpp"

#include "hot_config.h"
#include "physics.h"
#include "point.h"
#include "model_cache.h"
#include "textures.h"
#include "textbox.h"
#include "triangle.h"
#include "hazard.h"

class BoxGuy: public Hazard {
 public:

    Point* original_position;

    bool rotated;

    float squish;
    float rise;
    float bpm;

    BoxGuy(std::string object_type, Physics* physics, Point* position, float rotation);

    virtual void update(float time_step);
    virtual void render();
};

#endif
