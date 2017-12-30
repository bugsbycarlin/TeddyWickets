/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "bumper.h"

Bumper::Bumper(Physics* physics, Point* start, Point* end, Point* extrusion) {
  this->start = start;
  this->end = end;
  this->extrusion = extrusion;

  // wrong
  this->normal = physics->normalVectorRelative(extrusion, end, start);

  last_bumped = 0;

  this->physics = physics;

  identity = physics->addBumper(start, end, extrusion);
}

void Bumper::bump() {
  last_bumped = 2;
}

void Bumper::update() {
  if (last_bumped > 0) {
    last_bumped--;
  }
}

void Bumper::render() {
  if (last_bumped > 0) {
    Textures::setTexture("lit_bumper");
  } else {
    Textures::setTexture("bumper");
  }

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  float bumper_height = 0.5f;

  // bumpers are unlit for now
  glDisable(GL_LIGHTING);

  glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex3f(start->x, start->y, start->z);
    glTexCoord2f(4, 0);
    glVertex3f(end->x, end->y, end->z);
    glTexCoord2f(4, 1);
    glVertex3f(end->x, end->y, end->z + bumper_height);
    glTexCoord2f(0, 1);
    glVertex3f(start->x, start->y, start->z + bumper_height);

    glTexCoord2f(0, 0);
    glVertex3f(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z);
    glTexCoord2f(4, 0);
    glVertex3f(end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z);
    glTexCoord2f(4, 1);
    glVertex3f(end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z + bumper_height);
    glTexCoord2f(0, 1);
    glVertex3f(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z + bumper_height);

    glTexCoord2f(0, 0);
    glVertex3f(start->x, start->y, start->z + bumper_height);
    glTexCoord2f(4, 0);
    glVertex3f(end->x, end->y, end->z + bumper_height);
    glTexCoord2f(4, 1);
    glVertex3f(end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z + bumper_height);
    glTexCoord2f(0, 1);
    glVertex3f(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z + bumper_height);

    glTexCoord2f(0.5, 0);
    glVertex3f(start->x, start->y, start->z);
    glTexCoord2f(1, 0);
    glVertex3f(start->x, start->y, start->z + bumper_height);
    glTexCoord2f(1, 1);
    glVertex3f(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z + bumper_height);
    glTexCoord2f(0.5, 1);
    glVertex3f(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z);

    glTexCoord2f(0.5, 0);
    glVertex3f(end->x, end->y, end->z);
    glTexCoord2f(1, 0);
    glVertex3f(end->x, end->y, end->z + bumper_height);
    glTexCoord2f(1, 1);
    glVertex3f(end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z + bumper_height);
    glTexCoord2f(0.5, 1);
    glVertex3f(end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z);

  glEnd();

  glEnable(GL_LIGHTING);
}
