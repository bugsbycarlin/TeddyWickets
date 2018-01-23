#include "globals.h"

// This method sets up the screen for a 2D drawing phase
void Start2DDraw() {
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0f, k_screen_width, k_screen_height, 0.0f, 0.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}

// This method ends the 2D drawing phase
void End2DDraw() {
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}


// The fade functions contain extremely useful and generic code
// to fade the screen to and from black based on time inputs.
void fadeInOut(float start, float finish, float timeDiff) {
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  float alpha = 1.0f - sin((timeDiff - start) * M_PI / (finish - start));
  glDisable(GL_TEXTURE_2D);
  glColor4f(0, 0, 0, alpha);
  glBegin(GL_QUADS);
  glVertex3f(0, k_screen_height, 0);
  glVertex3f(k_screen_width, k_screen_height, 0);
  glVertex3f(k_screen_width, 0, 0);
  glVertex3f(0, 0, 0);
  glEnd();
  glPopAttrib();
}
  
void fadeIn(float start, float finish, float timeDiff) {
  if (timeDiff < finish) {
    fadeInOut(start, finish + finish - start, timeDiff);
  }
}
  
void fadeOut(float start, float finish, float timeDiff) {
  if (timeDiff < finish - start and 0 < timeDiff) {
    fadeInOut(0, 1, (timeDiff / (2.0 * (finish - start)) + 0.5));
  }
}

void blackout() {
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glColor4f(0, 0, 0, 1);
  glBegin(GL_QUADS);
  glVertex3f(0, k_screen_height, 0);
  glVertex3f(k_screen_width, k_screen_height, 0);
  glVertex3f(k_screen_width, 0, 0);
  glVertex3f(0, 0, 0);
  glEnd();
  glPopAttrib();
}
