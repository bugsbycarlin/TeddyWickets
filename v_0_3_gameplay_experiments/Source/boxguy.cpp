/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "boxguy.h"

BoxGuy::BoxGuy(std::string object_type, Physics* physics, Point* position, float rotation) : Hazard(object_type, physics, position, rotation) {
  this->rotated = true;
  // assuming wicket is rotated +- Pi/2, or 0
  if (abs(rotation - M_PI) < 0.2 || abs(rotation - M_PI - M_PI) < 0.2) {
    this->rotated = false;
  }

  squish = 1.0f;
  rise = 0.0f;

  this->original_position = new Point(position->x, position->y, position->z);
}

void BoxGuy::render() {

  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);

  graphics->pushModelMatrix();
  graphics->translate(position->x, position->y, position->z + rise);
  graphics->rotate(-90.0f, 0.0f, 0.0f, 1.0f);
  graphics->scale(1.0f, 1.0f, squish);
  graphics->rotate(-rotation * 180.0 / M_PI, 0, 0, 1);
  model->render();
  graphics->popModelMatrix();


  //pi happens 60.0/bpm seconds, time_step is seconds since the beginning. every 1 timestep should be bpm/60 pis.
}

void BoxGuy::update(float time_step) {

  // Abusing timestep to be total time.

  if (rotated) {
    position->x = original_position->x + 9 * sin(time_step / 1.0f);
  } else {
    position->y = original_position->y + 9 * sin(time_step / 1.0f);
  }
  physics->setPosition(identity, position);

  squish = 1.0f + hot_config->getFloat("boxguy_squish") * sin(time_step * M_PI * bpm/60.0f);
  rise = hot_config->getFloat("boxguy_rise") * sin(time_step * M_PI * bpm/60.0f);
}