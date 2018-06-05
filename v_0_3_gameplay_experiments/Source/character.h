/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Character class models the player's avatar (ie the bear, or the ball).
*/

#ifndef TEDDY_WICKETS_CHARACTER_H_
#define TEDDY_WICKETS_CHARACTER_H_

#include <list>
#include <queue>
#include <deque>

#include "hot_config.h"
#include "globals.h"
#include "graphics.h"
#include "model_cache.h"
#include "physics.h"
#include "point.h"
#include "textures.h"

class Character {
 public:
    int identity;
    Point* position;

    Point* last_drop_position;

    int roster_number;
    int player_number;

    Physics* physics;
    Model* model;

    float default_shot_rotation;
    float shot_rotation;
    float default_shot_power;
    float shot_power;
    bool up_shot;

    btTransform save_transform;

    std::string name;

    int status;

    float radius;

    std::deque<Point*> velocity_history = { };
    std::deque<Point*> position_history = { };

    Character(Physics* physics, Point* position, std::string model_name);

    void updateFromPhysics();

    bool stoppedMoving();

    void setShotRotation(float value, bool recompute_trajectory);

    void impulse(float x, float y, float z);

    void render(int game_mode);
};

#endif
