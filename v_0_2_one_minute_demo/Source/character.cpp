/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "character.h"

Character::Character(Physics* physics, Point* position, std::string model_name) {
  this->position = position;
  this->last_drop_position = new Point(position->x, position->y, position->z);

  this->physics = physics;

  if (model_name != "") {
    this->model = model_cache->getModel(model_name);  
  } else {
    this->model = model_cache->getModel("teddy_bear_draft_3.obj");
  }

  this->status = k_bear_status_sidelined;

  default_shot_rotation = k_default_shot_rotation;
  shot_rotation = default_shot_rotation;
  default_shot_power = k_default_shot_power;
  shot_power = 0;
  up_shot = false;

  radius = 0.75;

  identity = physics->addBall(radius, position->x, position->y, position->z);
  //identity = physics->addSoftball(this->model->getMeshAsTriangles(), this->position, shot_rotation);
}

// this gets the position of the ball for game logic and rendering. I believe it's currently unused,
// since rendering is done with the transform directly from physics. Keeping this method for now
// for potential future use.
void Character::updateFromPhysics() {
  physics->updatePoint(position, identity);
  
  Point* v = physics->getVelocityVector(identity);
  velocity_history.push_front(v);
  if (velocity_history.size() > 10) {
    velocity_history.pop_back();
  }

  position_history.push_front(new Point(position->x, position->y, position->z));
  if (position_history.size() > 10) {
    position_history.pop_back();
  }
}

bool Character::stoppedMoving() {
  if (velocity_history.size() < 10) return false;
  bool stoppedMoving = true;
  for (auto velocity = velocity_history.begin(); velocity != velocity_history.end(); ++velocity) {
    //printf("Velocity magnitude: %0.5f\n", (*velocity)->magnitude());
    if ((*velocity)->magnitude() > 0.0001f) {
      stoppedMoving = false;
    }
  }
  return stoppedMoving;
}

void Character::render(int game_mode) {
  btScalar transform_matrix[16];
  btTransform transform = physics->getTransform(identity);
  graphics->pushModelMatrix();
    transform.getOpenGLMatrix(transform_matrix);
    graphics->multMatrix(transform_matrix);
    graphics->pushModelMatrix();
      graphics->scale(k_model_scale, k_model_scale, k_model_scale);
      graphics->rotate(-90.0f, 0.0f, 0.0f, 1.0f);
      //graphics->scale(1.0f, 1.0f, 0.7f);
      model->render();
    graphics->popModelMatrix();
  graphics->popModelMatrix();

  // future positions (to help the player predict the shot)
  //if (game_mode == k_aim_mode) {
    //textures->setTexture("plain");

    // balls marking future positions
    // for (auto position = future_positions.begin(); position != future_positions.end(); ++position) {
    //   btScalar transform_matrix[16];
    //   position->getOpenGLMatrix(transform_matrix);
    //   graphics->pushModelMatrix();
    //   graphics->multMatrix(transform_matrix);
    //   //graphics->sphere(radius * 0.15);
    //   graphics->scale(1.0f, 1.0f, 1.0f);
    //   graphics->rotate(-90.0f, 0.0f, 0.0f, 1.0f);
    //   shot_arrow->render();
    //   graphics->popModelMatrix();
    // }

    // lines between balls

    // graphics->lineWidth(3);
    // //float line_data[3 * future_positions.size()];
    // std::vector<float> line_data = {};
    // for (auto position = future_positions.begin(); position != future_positions.end(); ++position) {
    //   btScalar transform_matrix[16];
    //   position->getOpenGLMatrix(transform_matrix);
    //   line_data.push_back(position->getOrigin().getX());
    //   line_data.push_back(position->getOrigin().getY());
    //   line_data.push_back(position->getOrigin().getZ());
    // }
    // graphics->lineStrip(line_data);
  //}
}

// This is sort of a hack. Force the physics of the ball to a particular angle.
// Then, if recompute_trajectory is true, save the physics state and do some forward simulation
// to predict the trajectory of the ball, saving the values. Finally, restore the old physics state.
// The prediction values are rendered as part of shot preparation.
void Character::setShotRotation(float value, bool recompute_trajectory) {
  shot_rotation = value;
  physics->setRotation(identity, 0, 0, shot_rotation);
}


// Put an impulse force on the ball.
void Character::impulse(float x, float y, float z) {
  physics->impulse(identity, x, y, z);
}
