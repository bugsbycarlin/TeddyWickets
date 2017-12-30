#ifndef CHARACTER_H
#define CHARACTER_H

#include "point.h"
#include "physics.h"
#include "textures.h"
#include <OpenGL/GLU.h>

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