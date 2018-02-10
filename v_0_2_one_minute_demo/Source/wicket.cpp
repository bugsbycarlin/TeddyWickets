/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "wicket.h"

Wicket::Wicket(Physics* physics, Textures* textures, Point* position, float rotation) {
  this->position = position;
  this->rotation = rotation;

  this->physics = physics;
  this->textures = textures;

  this->model = new Model(textures, "wicket.obj");

  this->identity = physics->addMesh(this->model->getMeshAsTriangles(), this->position, rotation);
}

void Wicket::render() {
  textures->setTexture("wicket");

  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);

  graphics->pushMatrix();
  graphics->translate(position->x, position->y, position->z);
  graphics->rotate(-90.0f, 0.0f, 0.0f, 1.0f);
  graphics->rotate(rotation * 180.0 / M_PI, 0, 0, 1);
  model->render();
  graphics->popMatrix();
}
