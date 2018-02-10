/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "surface.h"

Surface::Surface(Physics* physics, Textures* textures, bool use_grey, Point* p1, Point* p2, Point* p3, Point* p4) {
  this->p1 = p1;
  this->p2 = p2;
  this->p3 = p3;
  this->p4 = p4;

  float grey_value = ((double) rand() / (RAND_MAX)) / 5.0 + 0.65;
  color_r = grey_value;
  color_g = grey_value;
  color_b = grey_value;

  this->use_grey = use_grey;

  this->physics = physics;
  this->textures = textures;

  this->normal = physics->normalVectorRelative(p1, p3, p4);

  physics->addSurface(p1, p2, p3, p4);
}

void Surface::render() {
  textures->setTexture("tiles");

  if (use_grey) {
    graphics->color(color_r, color_g, color_b, 1.0f);
  } else {
    graphics->color(1.0f, 1.0f, 1.0f, 1.0f);
  }

  graphics->matteMaterial();

  float data[32] = {
    0, 0, normal->x, normal->y, normal->z, p1->x, p1->y, p1->z,
    1, 0, normal->x, normal->y, normal->z, p2->x, p2->y, p2->z,
    1, 1, normal->x, normal->y, normal->z, p3->x, p3->y, p3->z,
    0, 1, normal->x, normal->y, normal->z, p4->x, p4->y, p4->z,
  };
  graphics->face(4, data);
}
