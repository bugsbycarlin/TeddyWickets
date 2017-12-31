/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "bumper.h"

Bumper::Bumper(Physics* physics, Textures* textures, Point* start, Point* end, Point* extrusion) {
  this->start = start;
  this->end = end;
  this->extrusion = extrusion;

  last_bumped = 0;

  this->physics = physics;
  this->textures = textures;

  identity = physics->addBumper(start, end, extrusion);
}

// If the bumper has been bumped, set a bump counter which will cause the bumper
// to light up for a few frames.
void Bumper::bump() {
  last_bumped = 2;
}

// reduce the bump counter on update
void Bumper::update() {
  if (last_bumped > 0) {
    last_bumped--;
  }
}

void Bumper::render() {
  // if the bumper has been bumped, light it up
  if (last_bumped > 0) {
    textures->setTexture("lit_bumper");
  } else {
    textures->setTexture("bumper");
  }

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  // bumpers have no opengl lighting for now
  glDisable(GL_LIGHTING);

  glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex3f(start->x, start->y, start->z);
    glTexCoord2f(4, 0);
    glVertex3f(end->x, end->y, end->z);
    glTexCoord2f(4, 1);
    glVertex3f(end->x, end->y, end->z + k_bumper_height);
    glTexCoord2f(0, 1);
    glVertex3f(start->x, start->y, start->z + k_bumper_height);

    glTexCoord2f(0, 0);
    glVertex3f(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z);
    glTexCoord2f(4, 0);
    glVertex3f(end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z);
    glTexCoord2f(4, 1);
    glVertex3f(end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z + k_bumper_height);
    glTexCoord2f(0, 1);
    glVertex3f(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z + k_bumper_height);

    glTexCoord2f(0, 0);
    glVertex3f(start->x, start->y, start->z + k_bumper_height);
    glTexCoord2f(4, 0);
    glVertex3f(end->x, end->y, end->z + k_bumper_height);
    glTexCoord2f(4, 1);
    glVertex3f(end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z + k_bumper_height);
    glTexCoord2f(0, 1);
    glVertex3f(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z + k_bumper_height);

    glTexCoord2f(0.5, 0);
    glVertex3f(start->x, start->y, start->z);
    glTexCoord2f(1, 0);
    glVertex3f(start->x, start->y, start->z + k_bumper_height);
    glTexCoord2f(1, 1);
    glVertex3f(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z + k_bumper_height);
    glTexCoord2f(0.5, 1);
    glVertex3f(start->x + extrusion->x, start->y + extrusion->y, start->z + extrusion->z);

    glTexCoord2f(0.5, 0);
    glVertex3f(end->x, end->y, end->z);
    glTexCoord2f(1, 0);
    glVertex3f(end->x, end->y, end->z + k_bumper_height);
    glTexCoord2f(1, 1);
    glVertex3f(end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z + k_bumper_height);
    glTexCoord2f(0.5, 1);
    glVertex3f(end->x + extrusion->x, end->y + extrusion->y, end->z + extrusion->z);

  glEnd();

  glEnable(GL_LIGHTING);
}
