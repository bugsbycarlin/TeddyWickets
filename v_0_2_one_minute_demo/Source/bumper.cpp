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

  this->cache_id = -1;

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
  // if (last_bumped > 0) {
  //   textures->setTexture("lit_bumper");
  // } else {
  //   textures->setTexture("bumper");
  // }
  // graphics->color(1.0f, 1.0f, 1.0f, 1.0f);

  textures->setTexture("pastel_rainbow");
  if (last_bumped > 0) {
    graphics->color(0.5f, 0.5f, 1.0f, 1.0f);
  } else {
    graphics->color(1.0f, 1.0f, 1.0f, 1.0f);
  }

  if (cache_id == -1) {

    std::vector<float> vertex_data = {};
    std::vector<float> normal_data = {};
    std::vector<float> texture_data = {};
    std::vector<float> color_data = {};

    vertex_data.push_back(start->x);
    vertex_data.push_back(start->y);
    vertex_data.push_back(start->z + 0.01f);
    vertex_data.push_back(end->x);
    vertex_data.push_back(end->y);
    vertex_data.push_back(end->z + 0.01f);
    vertex_data.push_back(end->x + extrusion->x);
    vertex_data.push_back(end->y + extrusion->y);
    vertex_data.push_back(end->z + extrusion->z + 0.01f);
    vertex_data.push_back(start->x + extrusion->x);
    vertex_data.push_back(start->y + extrusion->y);
    vertex_data.push_back(start->z + extrusion->z + 0.01f);

    texture_data.push_back(0);
    texture_data.push_back(0);
    texture_data.push_back(4);
    texture_data.push_back(0);
    texture_data.push_back(4);
    texture_data.push_back(1);
    texture_data.push_back(0);
    texture_data.push_back(1);

    Point* normal = physics->normalVectorRelative(start, end, new Point(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z));
    float flip = 1.0f;
    if (normal->z < 0) {
      flip = -1.0f;
    }
    normal_data.push_back(normal->x * flip);
    normal_data.push_back(normal->y * flip);
    normal_data.push_back(normal->z * flip);
    normal_data.push_back(normal->x * flip);
    normal_data.push_back(normal->y * flip);
    normal_data.push_back(normal->z * flip);
    normal_data.push_back(normal->x * flip);
    normal_data.push_back(normal->y * flip);
    normal_data.push_back(normal->z * flip);
    normal_data.push_back(normal->x * flip);
    normal_data.push_back(normal->y * flip);
    normal_data.push_back(normal->z * flip);

    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);
    color_data.push_back(1.0f);

    cache_id = graphics->cacheFullMesh(
      vertex_data,
      normal_data,
      texture_data,
      color_data);
  } else {
    graphics->drawMesh(cache_id);
  }
}
