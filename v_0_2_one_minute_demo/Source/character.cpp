/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "character.h"

Character::Character(Physics* physics, Point* position) {
  this->position = position;

  this->physics = physics;

  this->model = new Model("teddy_bear_draft_3.obj");

  default_shot_rotation = k_default_shot_rotation;
  shot_rotation = default_shot_rotation;
  default_shot_power = k_default_shot_power;
  shot_power = 0;
  up_shot = false;

  radius = 0.75;

  identity = physics->addBall(radius, position->x, position->y, position->z);
}

// this gets the position of the ball for game logic and rendering. I believe it's currently unused,
// since rendering is done with the transform directly from physics. Keeping this method for now
// for potential future use.
void Character::updateFromPhysics() {
  physics->updatePoint(position, identity);
}

void Character::render(int game_mode) {
  btScalar transform_matrix[16];
  btTransform transform = physics->getTransform(identity);
  graphics->pushMatrix();
    transform.getOpenGLMatrix(transform_matrix);
    graphics->multMatrix(transform_matrix);
    graphics->pushMatrix();
      graphics->scale(k_model_scale, k_model_scale, k_model_scale);
      graphics->rotate(-90.0f, 0.0f, 0.0f, 1.0f);
      model->render();
    graphics->popMatrix();
  graphics->popMatrix();

  // future positions (to help the player predict the shot)
  if (game_mode == k_prep_mode) {
    textures->setTexture("plain");

    // balls marking future positions
    for (auto position = future_positions.begin(); position != future_positions.end(); ++position) {
      btScalar transform_matrix[16];
      position->getOpenGLMatrix(transform_matrix);
      graphics->pushMatrix();
      graphics->multMatrix(transform_matrix);
      graphics->sphere(radius * 0.15);
      graphics->popMatrix();
    }

    // lines between balls

    graphics->lineWidth(3);
    //float line_data[3 * future_positions.size()];
    std::vector<float> line_data = {};
    for (auto position = future_positions.begin(); position != future_positions.end(); ++position) {
      btScalar transform_matrix[16];
      position->getOpenGLMatrix(transform_matrix);
      line_data.push_back(position->getOrigin().getX());
      line_data.push_back(position->getOrigin().getY());
      line_data.push_back(position->getOrigin().getZ());
    }
    graphics->lineStrip(line_data);
  }
}

// This is sort of a hack. Force the physics of the ball to a particular angle.
// Then, if recompute_trajectory is true, save the physics state and do some forward simulation
// to predict the trajectory of the ball, saving the values. Finally, restore the old physics state.
// The prediction values are rendered as part of shot preparation.
void Character::setShotRotation(float value, bool recompute_trajectory) {
  shot_rotation = value;
  physics->setRotation(identity, 0, 0, shot_rotation);

  if (recompute_trajectory) {
    // calculate future points...
    future_positions.clear();
    if (up_shot) {
      // ... using an angled degree shot
      impulse(cos(k_up_shot_angle) * default_shot_power * sin(shot_rotation),
        cos(k_up_shot_angle) * -default_shot_power * cos(shot_rotation),
        sin(k_up_shot_angle) * default_shot_power);
    } else {
      // ... using a flat shot
      impulse(default_shot_power * sin(shot_rotation), -default_shot_power * cos(shot_rotation), 0.5);
    }
    physics->activate(identity);
    btTransform current_transform = physics->getTransform(identity);
    for (int i = 0; i < 300; i++) {
      physics->update(1.0f / 60.f);
      if (i > 0 && i % 5 == 0) future_positions.push_front(btTransform(physics->getTransform(identity)));
    }
    physics->setTransform(identity, current_transform);
    physics->stop(identity);
    physics->setPositionAndRotation(identity,
        new Point(position->x, position->y, position->z + 0.001f),
        0, 0, shot_rotation);
  }
}


// Put an impulse force on the ball.
void Character::impulse(float x, float y, float z) {
  physics->impulse(identity, x, y, z);
}
