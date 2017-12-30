#ifndef WICKET_H
#define WICKET_H

#include "point.h"
#include "physics.h"
#include "textures.h"
#include <OpenGL/GLU.h>

class Wicket {
  public:

    int identity;

    GLUquadric* pole;
    GLUquadric* pole_cap;

    Point* pole_1_position;
    Point* pole_2_position;
    float height;

    Physics* physics;

    Wicket(Physics* physics, Point* pole_1_position, Point* pole_2_position, float height);

    void render();
};

#endif