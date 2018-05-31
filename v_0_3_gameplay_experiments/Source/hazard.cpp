/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "hazard.h"

Hazard::Hazard(std::string object_type, Physics* physics, Point* position, float rotation) {
  this->position = position;
  this->rotation = rotation;

  this->physics = physics;

  this->object_type = object_type;

  this->model = model_cache->getModel(object_type + ".obj");

  float physics_rotation = -rotation - M_PI;
  // I do not know or remember why this is
  // and I need to fix it for bumpers too. they vary by rotation
  // if (object_type == "wicket" || object_type == "ramp") {
  //   physics_rotation = rotation;
  // }

  this->identity = physics->addMesh(this->model->getMeshAsTriangles(), this->position, physics_rotation);
}

// No physics
Hazard::Hazard(std::string object_type, Point* position, float rotation) {
  this->position = position;
  this->rotation = rotation;

  this->object_type = object_type;

  this->model = model_cache->getModel(object_type + ".obj");
}

void Hazard::render() {
  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);

  graphics->pushModelMatrix();
  graphics->translate(position->x, position->y, position->z);
  graphics->rotate(-90.0f, 0.0f, 0.0f, 1.0f);
  graphics->rotate(-rotation * 180.0 / M_PI, 0, 0, 1);
  model->render();
  graphics->popModelMatrix();
}

void Hazard::update(float time_step) {
  
}