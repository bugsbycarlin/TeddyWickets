/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "surface.h"

Surface::Surface(Physics* physics, bool use_grey, Point* p1, Point* p2, Point* p3, Point* p4) {
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

  this->normal = physics->normalVectorRelative(p1, p3, p4);

  physics->addSurface(p1, p2, p3, p4);

  this->cache_id = -1;
}

void Surface::render() {
  textures->setTexture("tiles");

  if (use_grey) {
    graphics->color(color_r, color_g, color_b, 1.0f);
  } else {
    graphics->color(1.0f, 1.0f, 1.0f, 1.0f);
  }

  graphics->matteMaterial();

  if(cache_id == -1) {

    float vertex_data[12] = {
      p1->x, p1->y, p1->z,
      p2->x, p2->y, p2->z,
      p3->x, p3->y, p3->z,
      p4->x, p4->y, p4->z
    };

    float normal_data[12] = {
      normal->x, normal->y, normal->z,
      normal->x, normal->y, normal->z,
      normal->x, normal->y, normal->z,
      normal->x, normal->y, normal->z
    };

    float texture_data[8] = {
      0.0f, 0.0f,
      1.0f, 0.0f,
      1.0f, 1.0f,
      0.0f, 1.0f
    };

    //graphics->primitive(4, vertex_data, normal_data, texture_data);
    cache_id = graphics->cacheMesh(4, vertex_data, normal_data, texture_data);
  } else {
    graphics->drawMesh(cache_id);
  }
  
}
