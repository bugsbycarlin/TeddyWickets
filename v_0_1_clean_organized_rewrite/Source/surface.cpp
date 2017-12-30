#include "surface.h"

Surface::Surface(Physics* physics, bool use_grey, Point* p1, Point* p2, Point* p3, Point* p4) {
  this->p1 = p1;
  this->p2 = p2;
  this->p3 = p3;
  this->p4 = p4;

  float grey_value = ((double) rand() / (RAND_MAX)) / 5.0 + 0.65;
  color_R = grey_value;
  color_G = grey_value;
  color_B = grey_value;

  this->use_grey = use_grey;

  this->physics = physics;

  this->normal = physics->normalVectorRelative(p1, p3, p4);

  physics->addWall(p1, p2, p3, p4);
}

void Surface::render() {
  Textures::setTexture("tiles");

  if (use_grey) {
    glColor4f(color_R, color_G, color_B, 1.0f);
  } else {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  }

  GLfloat mat_a[] = {0.5, 0.5, 0.5, 1.0};
  GLfloat mat_d[] = {0.5, 0.5, 0.5, 1.0};
  GLfloat mat_s[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat low_sh[] = {5.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_a);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_d);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_s);
  glMaterialfv(GL_FRONT, GL_SHININESS, low_sh);

  glBegin(GL_QUADS);

    glNormal3f(normal->x, normal->y, normal->z);

    glTexCoord2f(0, 0); glVertex3f(p1->x, p1->y,p1->z);
    glTexCoord2f(1, 0); glVertex3f(p2->x, p2->y,p2->z);
    glTexCoord2f(1, 1); glVertex3f(p3->x, p3->y,p3->z);
    glTexCoord2f(0, 1); glVertex3f(p4->x, p4->y,p4->z);

  glEnd();
}
