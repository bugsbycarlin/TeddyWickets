
#include "point.h"
#include "physics.h"
#include "textures.h"
#include <OpenGL/GLU.h>
#include <cstdlib>
#include <stdio.h>

class Surface {
  public:
    Point* p1;
    Point* p2;
    Point* p3;
    Point* p4;
    Point* normal;
    Physics* physics;

    float color_R;
    float color_G;
    float color_B;

    bool use_grey;

    Surface(Physics* physics, bool use_grey, Point* p1, Point* p2, Point* p3, Point* p4);

    void render();
};