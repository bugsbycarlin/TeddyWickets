/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Subclass of hazard which models free standing stars, including hit detection, display, and scoring.
*/

#ifndef TEDDY_WICKETS_FREE_STAR_H_
#define TEDDY_WICKETS_FREE_STAR_H_

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

class FreeStar: public Hazard {
 public:

    TextBox* free_star_value_text;

    int value;

    float bpm;

    bool rotated;
    bool active;

    float radius_expander;

    int player_owner;

    FreeStar(std::string object_type, Physics* physics, Point* position, float rotation);

    void render();

    bool flipWicket(Point* current_point, float radius, int player_owner);

    void setRenderInfo(float time_step);
    void renderInfo();
};

#endif
