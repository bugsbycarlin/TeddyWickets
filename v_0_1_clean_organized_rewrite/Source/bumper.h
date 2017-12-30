#include "point.h"
#include "physics.h"
#include "textures.h"
#include <OpenGL/GLU.h>

class Bumper {
  public:
    Point* start;
    Point* end;
    Point* extrude_direction;
    Point* normal;
    Physics* physics;

    int identity;

    int last_bumped;

    bool use_grey;

    Bumper(Physics* physics, Point* start, Point* end, Point* extrude_direction);

    void bump();

    void update();

    void render();
};