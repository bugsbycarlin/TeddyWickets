/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "wicket.h"

Wicket::Wicket(Physics* physics, Point* pole_1_position, Point* pole_2_position, float height) {
  this->pole_1_position = pole_1_position;
  this->pole_2_position = pole_2_position;
  this->height = height;

  pole = gluNewQuadric();
  gluQuadricTexture(pole, GL_TRUE);    // Create Texture Coords
  gluQuadricNormals(pole, GLU_SMOOTH); // Create Smooth Normals

  pole_cap = gluNewQuadric();
  gluQuadricTexture(pole_cap, GL_TRUE);    // Create Texture Coords
  gluQuadricNormals(pole_cap, GLU_SMOOTH); // Create Smooth Normals

  physics->addWicket(pole_1_position, pole_2_position, height);
}

void Wicket::render() {
  Textures::setTexture("wicket");

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  // pole 1
  glTranslatef(pole_1_position->x, pole_1_position->y, pole_1_position->z);
  gluCylinder(pole, 0.2, 0.2, height, 20, 20);
  glTranslatef(-pole_1_position->x, -pole_1_position->y, -pole_1_position->z);

  // pole 2
  glTranslatef(pole_2_position->x, pole_2_position->y, pole_2_position->z);
  gluCylinder(pole, 0.2, 0.2, height, 20, 20);
  glTranslatef(-pole_2_position->x, -pole_2_position->y, -pole_2_position->z);

  Textures::setTexture("plain");

  // pole cap 1
  glTranslatef(pole_1_position->x, pole_1_position->y, pole_1_position->z + height);
  gluSphere(pole_cap, 0.2, 20, 20);
  glTranslatef(-pole_1_position->x, -pole_1_position->y, -pole_1_position->z - height);

  // pole cap 1
  glTranslatef(pole_2_position->x, pole_2_position->y, pole_2_position->z + height);
  gluSphere(pole_cap, 0.2, 20, 20);
  glTranslatef(-pole_2_position->x, -pole_2_position->y, -pole_2_position->z - height);


  float base_height = 0.1f;

  // pole base 1
  glTranslatef(pole_1_position->x, pole_1_position->y, pole_1_position->z + 0.01f);
  gluCylinder(pole, 0.4, 0.4, base_height, 20, 20);
  Textures::setTexture("black");
  glTranslatef(0,0,base_height);
  Textures::setTexture("plain");
  gluDisk(pole, 0.0, 0.4, 20, 20);
  glTranslatef(0,0,-base_height);
  glTranslatef(-pole_1_position->x, -pole_1_position->y, -pole_1_position->z - 0.01f);

  // pole 2
  glTranslatef(pole_2_position->x, pole_2_position->y, pole_2_position->z + 0.01f);
  gluCylinder(pole, 0.4, 0.4, base_height, 20, 20);
  Textures::setTexture("black");
  glTranslatef(0,0,base_height);
  Textures::setTexture("plain");
  gluDisk(pole, 0.0, 0.4, 20, 20);
  glTranslatef(0,0,-base_height);
  glTranslatef(-pole_2_position->x, -pole_2_position->y, -pole_2_position->z - 0.01f);


  // glBegin(GL_QUADS);

  //   glNormal3f(normal->x, normal->y, normal->z);

  //   glTexCoord2f(0, 0); glVertex3f(p1->x, p1->y,p1->z);
  //   glTexCoord2f(1, 0); glVertex3f(p2->x, p2->y,p2->z);
  //   glTexCoord2f(1, 1); glVertex3f(p3->x, p3->y,p3->z);
  //   glTexCoord2f(0, 1); glVertex3f(p4->x, p4->y,p4->z);

  // glEnd();
}
