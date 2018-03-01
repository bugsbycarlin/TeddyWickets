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

  this->model = new Model(object_type + ".obj");

  this->identity = physics->addMesh(this->model->getMeshAsTriangles(), this->position, rotation);
}

void Hazard::render() {
  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);

  graphics->pushMatrix();
  graphics->translate(position->x, position->y, position->z);
  graphics->rotate(-90.0f, 0.0f, 0.0f, 1.0f);
  graphics->rotate(-rotation * 180.0 / M_PI, 0, 0, 1);
  model->render();
  graphics->popMatrix();
}
