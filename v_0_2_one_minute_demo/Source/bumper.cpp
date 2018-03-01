/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "bumper.h"

Bumper::Bumper(Physics* physics, Point* start, Point* end, Point* extrusion) {
  this->start = start;
  this->end = end;
  this->extrusion = extrusion;

  last_bumped = 0;

  this->physics = physics;

  identity = physics->addBumper(start, end, extrusion);
}

// If the bumper has been bumped, set a bump counter which will cause the bumper
// to light up for a few frames.
void Bumper::bump() {
  last_bumped = 2;
}

// reduce the bump counter on update
void Bumper::update() {
  if (last_bumped > 0) {
    last_bumped--;
  }
}

void Bumper::render() {
  // if the bumper has been bumped, light it up
  if (last_bumped > 0) {
    textures->setTexture("lit_bumper");
  } else {
    textures->setTexture("bumper");
  }

  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);

  // GROSS

  float data1[] = {
    0, 0, 0, 0, 1, start->x, start->y, start->z,
    4, 0, 0, 0, 1, end->x, end->y, end->z,
    4, 1, 0, 0, 1, end->x, end->y, end->z + k_bumper_height,
    0, 1, 0, 0, 1, start->x, start->y, start->z + k_bumper_height
  };
  graphics->face(4, data1);

  float data2[] = {
    0, 0, 0, 0, 1, start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z,
    4, 0, 0, 0, 1, end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z,
    4, 1, 0, 0, 1, end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z + k_bumper_height,
    0, 1, 0, 0, 1, start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z + k_bumper_height,
  };
  graphics->face(4, data2);

  float data3[] = {
    0, 0, 0, 0, 1, start->x, start->y, start->z + k_bumper_height,
    4, 0, 0, 0, 1, end->x, end->y, end->z + k_bumper_height,
    4, 1, 0, 0, 1, end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z + k_bumper_height,
    0, 1, 0, 0, 1, start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z + k_bumper_height,
  };
  graphics->face(4, data3);

  float data4[] = {
    0.5, 0, 0, 0, 1, start->x, start->y, start->z,
    1, 0, 0, 0, 1, start->x, start->y, start->z + k_bumper_height,
    1, 1, 0, 0, 1, start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z + k_bumper_height,
    0.5, 1, 0, 0, 1, start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z,
  };
  graphics->face(4, data4);

  float data5[] = {
    0.5, 0, 0, 0, 1, end->x, end->y, end->z,
    1, 0, 0, 0, 1, end->x, end->y, end->z + k_bumper_height,
    1, 1, 0, 0, 1, end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z + k_bumper_height,
    0.5, 1, 0, 0, 1, end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z,
  };
  graphics->face(4, data5);
}
