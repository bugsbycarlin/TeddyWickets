/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "character.h"

Character::Character(Physics* physics, Point* position) {
  this->position = position;

  ball = gluNewQuadric();
  gluQuadricTexture(ball, GL_TRUE);
  gluQuadricNormals(ball, GLU_SMOOTH);

  this->physics = physics;

  default_shot_rotation = k_default_shot_rotation;
  shot_rotation = default_shot_rotation;
  default_shot_power = k_default_shot_power;
  shot_power = 0;
  up_shot = false;

  radius = 0.75;

  identity = physics->addBall(radius, position->x, position->y, position->z);
}

void Character::updateFromPhysics() {
  physics->updatePoint(position, identity);
}

void Character::render(int game_mode) {
  GLfloat material_ambient[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat material_diffuse[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat material_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat shininess[] = {5.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

  Textures::setTexture("bear_face");
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  btScalar transform_matrix[16];
  btTransform transform = physics->getTransform(identity);
  transform.getOpenGLMatrix(transform_matrix);
  glPushMatrix();
  glMultMatrixf((GLfloat*)transform_matrix);
  gluSphere(ball, radius, 20, 20);

  // at the moment, all of the renders below are fixed to a parent ball radius of 0.75.
  // rather than change this now, I'll wait for the future, where all of this will be
  // replaced by a mesh loading system and meshes made in Maya.

  // nose
  Textures::setTexture("bear_nose");

  glTranslatef(0, -0.62, 0.15);
  gluSphere(ball, 0.25, 20, 20);
  glTranslatef(0, 0.62, -0.15);

  // parts
  Textures::setTexture("bear_arms");

  // arms
  glTranslatef(-0.6, -0.25, 0);
  gluSphere(ball, 0.25, 20, 20);
  glTranslatef(0.6, 0.25, 0);

  glTranslatef(0.6, -0.25, 0);
  gluSphere(ball, 0.25, 20, 20);
  glTranslatef(-0.6, 0.25, 0);

  // ears
  glTranslatef(-0.4, 0, 0.6);
  gluSphere(ball, 0.17, 20, 20);
  glTranslatef(0.4, 0, -0.6);

  glTranslatef(0.4, 0, 0.6);
  gluSphere(ball, 0.17, 20, 20);
  glTranslatef(-0.4, 0, -0.6);

  // stubby lil' feet
  glTranslatef(-0.4, 0, -0.55);
  gluSphere(ball, 0.25, 20, 20);
  glTranslatef(0.4, 0, 0.55);

  glTranslatef(0.4, 0, -0.55);
  gluSphere(ball, 0.25, 20, 20);
  glTranslatef(-0.4, 0, 0.55);

  // tail
  glTranslatef(0, 0.62, -0.15);
  gluSphere(ball, 0.17, 20, 20);
  glTranslatef(0, -0.62, 0.15);

  if (game_mode == k_prep_mode && shot_rotation == default_shot_rotation) {
    // Tutorial info
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    Textures::setTexture("left_turn_info");

    glBegin(GL_QUADS);
      glTexCoord2f(0, 1); glVertex3f(3.2, 3.2, -6);
      glTexCoord2f(0, 0); glVertex3f(3.2, 3.2, 6);
      glTexCoord2f(1, 0); glVertex3f(11.6, -5.3, 6);
      glTexCoord2f(1, 1); glVertex3f(11.6, -5.3, -6);
    glEnd();

    Textures::setTexture("right_turn_info");

    glBegin(GL_QUADS);
      glTexCoord2f(1, 1); glVertex3f(-1, 5.3, -6);
      glTexCoord2f(1, 0); glVertex3f(-1, 5.3, 6);
      glTexCoord2f(0, 0); glVertex3f(-9.5, -3.2, 6);
      glTexCoord2f(0, 1); glVertex3f(-9.5, -3.2, -6);
    glEnd();
  }

  glPopMatrix();

  if (game_mode == k_prep_mode) {
    // render future positions
    Textures::setTexture("plain");

    // balls marking future positions
    for (auto position = future_positions.begin(); position != future_positions.end(); ++position) {
      btScalar transform_matrix[16];
      position->getOpenGLMatrix(transform_matrix);
      glPushMatrix();
      glMultMatrixf((GLfloat*)transform_matrix);
      gluSphere(ball, radius * 0.15, 10, 10);
      glPopMatrix();
    }

    // lines between balls
    glLineWidth(3);
    glBegin(GL_LINE_STRIP);
    for (auto position = future_positions.begin(); position != future_positions.end(); ++position) {
      btScalar transform_matrix[16];
      position->getOpenGLMatrix(transform_matrix);
      glVertex3f(position->getOrigin().getX(), position->getOrigin().getY(), position->getOrigin().getZ());
    }
    glEnd();
  }
}

void Character::setShotRotation(float value, bool recompute_trajectory) {
  shot_rotation = value;
  physics->setRotation(identity, 0, 0, shot_rotation);

  if (recompute_trajectory) {
    // calculate future points...
    future_positions.clear();
    if (up_shot) {
      // ... using a 45 degree shot
      impulse(0.293 * default_shot_power * sin(shot_rotation),
        0.293 * -default_shot_power * cos(shot_rotation),
        0.707 * default_shot_power);
    } else {
      // ... using a flat shot
      impulse(default_shot_power * sin(shot_rotation), -default_shot_power * cos(shot_rotation), 0.5);
    }
    physics->activate(identity);
    btTransform current_transform = physics->getTransform(identity);
    for (int i = 0; i < 300; i++) {
      physics->update(1.0f / 60.f);
      if (i % 5 == 0) future_positions.push_front(btTransform(physics->getTransform(identity)));
    }
    physics->setTransform(identity, current_transform);
    physics->stop(identity);
    physics->setPositionAndRotation(identity,
        new Point(position->x, position->y, position->z + 0.001f),
        0, 0, shot_rotation);
  }
}

void Character::impulse(float x, float y, float z) {
  physics->impulse(identity, x, y, z);
}
