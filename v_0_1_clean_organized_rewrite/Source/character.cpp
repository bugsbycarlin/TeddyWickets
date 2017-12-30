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

  default_shot_rotation = -0.78;
  shot_rotation = default_shot_rotation;
  default_shot_power = 20.0;
  shot_power = 0;
  up_shot = false;

  radius = 0.75;

  identity = physics->addBall(radius, position->x, position->y, position->z);
}

void Character::updateFromPhysics() {
  physics->updatePoint(position, identity);
}

void Character::render(int game_mode) {
  GLfloat mat_a[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat mat_d[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat mat_s[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat low_sh[] = {5.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_a);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_d);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_s);
  glMaterialfv(GL_FRONT, GL_SHININESS, low_sh);

  Textures::setTexture("bear_face");
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  btScalar m[16];
  btTransform trans = physics->getTransform(identity);
  trans.getOpenGLMatrix(m);
  glPushMatrix();
  glMultMatrixf((GLfloat*)m);
  gluSphere(ball, radius, 20, 20);

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

  if (game_mode == PREP_MODE && shot_rotation == default_shot_rotation) {
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

  if (game_mode == PREP_MODE) {
    // future positions
    Textures::setTexture("plain");
    glLineWidth(3);
    glBegin(GL_LINE_STRIP);
    for (auto position = futurePositions.begin(); position != futurePositions.end(); ++position) {
      btScalar m[16];
      position->getOpenGLMatrix(m);
      glVertex3f(position->getOrigin().getX(), position->getOrigin().getY(), position->getOrigin().getZ());
    }
    glEnd();
    for (auto position = futurePositions.begin(); position != futurePositions.end(); ++position) {
      btScalar m[16];
      position->getOpenGLMatrix(m);
      glPushMatrix();
      glMultMatrixf((GLfloat*)m);
      gluSphere(ball, radius * 0.15, 10, 10);
      glPopMatrix();
    }
  }
}

void Character::setShotRotation(float value, bool recompute_trajectory) {
  shot_rotation = value;
  physics->setRotation(identity, 0, 0, shot_rotation);

  if (recompute_trajectory) {
    // calculate future points
    futurePositions.clear();
    if (up_shot) {
      // a 45 degree shot
      impulse(0.293 * default_shot_power * sin(shot_rotation),
        0.293 * -default_shot_power * cos(shot_rotation),
        0.707 * default_shot_power);
    } else {
      impulse(default_shot_power * sin(shot_rotation), -default_shot_power * cos(shot_rotation), 0.5);
    }
    physics->activate(identity);
    btTransform currentTrans = physics->getTransform(identity);
    for (int i = 0; i < 300; i++) {
      physics->update(1.0f / 60.f);
      if (i % 5 == 0) futurePositions.push_front(btTransform(physics->getTransform(identity)));
    }
    physics->setTransform(identity, currentTrans);
    physics->stop(identity);
    physics->setPositionAndRotation(identity,
        new Point(position->x, position->y, position->z + 0.001f),
        0, 0, shot_rotation);
  }
}

void Character::impulse(float x, float y, float z) {
  physics->impulse(identity, x, y, z);
}
