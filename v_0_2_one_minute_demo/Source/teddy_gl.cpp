/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "teddy_gl.h"

TeddyGL* teddy_gl = new TeddyGL();

TeddyGL::TeddyGL() {

}

// This method draws a rectangle
void TeddyGL::drawRectangle(float x, float y, float w, float h) {
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(x, y);
  glTexCoord2d(0.0, 1.0); glVertex2d(x, y + h);
  glTexCoord2d(1.0, 1.0); glVertex2d(x + w, y + h);
  glTexCoord2d(1.0, 0.0); glVertex2d(x + w, y);
  glEnd();
}

// This method sets up the screen for a 2D drawing phase
void TeddyGL::start2DDraw() {
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
void TeddyGL::end2DDraw() {
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}


// The fade functions contain extremely useful and generic code
// to fade the screen to and from black based on time inputs.
void TeddyGL::fadeInOut(float start, float finish, float timeDiff) {
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

void TeddyGL::fadeIn(float start, float finish, float timeDiff) {
  if (timeDiff < finish) {
    fadeInOut(start, finish + finish - start, timeDiff);
  }
}

void TeddyGL::fadeOut(float start, float finish, float timeDiff) {
  if (timeDiff < finish - start && 0 < timeDiff) {
    fadeInOut(0, 1, (timeDiff / (2.0 * (finish - start)) + 0.5));
  }
}

void TeddyGL::blackout() {
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

void TeddyGL::initializeLighting() {
  // Simple Opengl Lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat global_ambient[] = {0.8, 0.8, 0.8, 1.0};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_NORMALIZE);
  glEnable(GL_RESCALE_NORMAL);

  GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat light_diffuse[] = {0.9, 0.9, 0.9, 1.0};
  GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  // position is defined after the camera, per render.

  // Common material
  GLfloat material_ambient[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat material_diffuse[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat material_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat shininess[] = {5.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

  // Cel Shader
  teddy_gl->initializeCelShading();

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing lighting! %s\n", gluErrorString(error));
    exit(1);
  }
}

void TeddyGL::initializeCelShading() {
  cel_shader_program = k_cel_shader_id;

  std::string line;

  std::string vertex_shader_path = k_shader_root_path + "cel_shader_vertex.glsl";
  printf("Loading vertex shader from %s\n", vertex_shader_path.c_str());

  std::ifstream vertex_shader_file;
  vertex_shader_file.open(vertex_shader_path);

  if (!vertex_shader_file.is_open()) {
    printf("Error: Failed to load vertex shader from %s\n", vertex_shader_path.c_str());
    return;
  }

  std::string vertex_shader_string = "";
  while (getline(vertex_shader_file, line)) {
    vertex_shader_string += line + "\n";
  }

  std::string fragment_shader_path = k_shader_root_path + "cel_shader_fragment.glsl";
  printf("Loading fragment shader from %s\n", fragment_shader_path.c_str());

  std::ifstream fragment_shader_file;
  fragment_shader_file.open(fragment_shader_path);

  if (!fragment_shader_file.is_open()) {
    printf("Error: Failed to load fragment shader from %s\n", fragment_shader_path.c_str());
    return;
  }

  std::string fragment_shader_string = "";
  while (getline(fragment_shader_file, line)) {
    fragment_shader_string += line + "\n";
  }

  const char *vertex_str = vertex_shader_string.c_str();
  const char *fragment_str = fragment_shader_string.c_str();

  GLint vertex_shader = 0;
  GLint fragment_shader = 0;
  GLint status = 0;

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vertex_shader, 1, &vertex_str, NULL);
  glShaderSource(fragment_shader, 1, &fragment_str, NULL);

  glCompileShader(vertex_shader);
  glCompileShader(fragment_shader);

  cel_shader_program = glCreateProgram();
  glAttachShader(cel_shader_program, vertex_shader);
  glAttachShader(cel_shader_program, fragment_shader);

  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    char log[500];
    GLint length;
    printf("Error: failed to compile the vertex shader.\n");
    glGetShaderInfoLog(vertex_shader, 500, &length, log);
    printf("Log: %s\n", log);
    exit(1);
  }

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    char log[500];
    GLint length;
    printf("Error: failed to compile the fragment shader.\n");
    glGetShaderInfoLog(fragment_shader, 500, &length, log);
    printf("Log: %s\n", log);
    exit(1);
  }

  glLinkProgram(cel_shader_program);

  glGetProgramiv(cel_shader_program, GL_LINK_STATUS, &status);
  if (!status) {
    printf("Error: failed to link the shader program object.\n");
    exit(1);
  }
}

void TeddyGL::startCelShading() {
  glUseProgram(cel_shader_program);
}

void TeddyGL::stopCelShading() {
  glUseProgram(0);
}