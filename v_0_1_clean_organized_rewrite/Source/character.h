/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Character class models the player's avatar (ie the bear, or the ball).
*/

#ifndef TEDDY_WICKETS_CHARACTER_H_
#define TEDDY_WICKETS_CHARACTER_H_

#include <OpenGL/GLU.h>

#include <list>

#include "point.h"
#include "physics.h"
#include "textures.h"

class Character {
 public:
    enum  {
        PREP_MODE = 1,
        ACTION_MODE = 2
    };

    // Ball
    GLUquadric* ball;
    int identity;
    Point* position;

    Physics* physics;

    float default_shot_rotation;
    float shot_rotation;
    float default_shot_power;
    float shot_power;
    bool up_shot;

    float radius;

    std::list<btTransform> futurePositions = { };

    Character(Physics* physics, Point* position);

    void updateFromPhysics();

    void setShotRotation(float value, bool recompute_trajectory);

    void impulse(float x, float y, float z);

    void render(int game_mode);
};

#endif
