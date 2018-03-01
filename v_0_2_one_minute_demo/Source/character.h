/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Character class models the player's avatar (ie the bear, or the ball).
*/

#ifndef TEDDY_WICKETS_CHARACTER_H_
#define TEDDY_WICKETS_CHARACTER_H_

#include <list>

#include "globals.h"
#include "graphics.h"
#include "model.h"
#include "physics.h"
#include "point.h"
#include "textures.h"

class Character {
 public:
    int identity;
    Point* position;

    Physics* physics;
    Model* model;

    float default_shot_rotation;
    float shot_rotation;
    float default_shot_power;
    float shot_power;
    bool up_shot;

    float radius;

    std::list<btTransform> future_positions = { };

    Character(Physics* physics, Point* position);

    void updateFromPhysics();

    void setShotRotation(float value, bool recompute_trajectory);

    void impulse(float x, float y, float z);

    void render(int game_mode);
};

#endif
