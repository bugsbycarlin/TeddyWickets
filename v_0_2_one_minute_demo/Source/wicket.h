/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Subclass of hazard which models wickets, including hit detection, display, and scoring.
*/

#ifndef TEDDY_WICKETS_WICKET_H_
#define TEDDY_WICKETS_WICKET_H_

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

class Wicket: public Hazard {
 public:

    TextBox* wicket_value_text;

    int value;

    float bpm;

    bool rotated;
    bool active;

    int player_owner;

    Wicket(std::string object_type, Physics* physics, Point* position, float rotation);

    void render();

    bool flipWicket(Point* last_point, Point* current_point, int player_owner);

    void setRenderInfo(float time_step);
    void renderInfo();
    void renderLastWicketInfo();
};

#endif
