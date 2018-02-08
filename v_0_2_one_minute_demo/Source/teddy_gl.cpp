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

void TeddyGL::initializeBasic() {

  // glFrontFace(GL_CW);
  // glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_TEXTURE_2D);

  // Simple viewport.
  glViewport(0, 0, k_screen_width, k_screen_height);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing GL config! %s\n", gluErrorString(error));
    exit(1);
  }
}

void TeddyGL::clearScreen() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor4f(1.0, 1.0, 1.0, 1.0);
}

void TeddyGL::set3d(float zoom) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Set one of these perspectives:

  // 1. this is cool, super zoomed out
  // glOrtho(-50, 50, -50, 50, -50, 50);

  // 2. weirdly reasonable ortho
  glOrtho(-zoom * k_aspect_ratio, zoom * k_aspect_ratio, -zoom, zoom, -10 * zoom, 10 * zoom);

  // 3. normal perspective
  // gluPerspective(45.0f,k_screen_width/(1.0 * k_screen_height),0.1f,1000.0f);
}

void TeddyGL::standardCamera(float cam_x, float cam_y, float cam_z, float target_x, float target_y, float target_z) {
  // Set the camera to look down at the character.
  // For fun, change the z-value to change the viewing angle of the game.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(cam_x, cam_y, cam_z,
            target_x, target_y, target_z,
            0, 0, 1);  
}

void TeddyGL::standardLightPosition() {
  // this code makes a sun
  // but it requires passing in time fraction
  // float fraction = (int(last_time - start_time) % k_sun_period) / (float)k_sun_period;
  // printf("Sun %0.2f\n", fraction);
  // GLfloat light_position[] = {1.0f * (float)cos(fraction * 2 * M_PI),
  //   -1.0f * (float)cos(fraction * 2 * M_PI), 1.0f * (float)sin(fraction * 2 * M_PI), 0.0};
  // glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  // if (fraction > 0.5f) {
  //   GLfloat light_diffuse[] = {0.25, 0.25, 0.25, 1.0};
  //   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  // } else {
  //   GLfloat light_diffuse[] = {0.8, 0.8, 0.8, 1.0};
  //   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  // }

  // from fraction of the way around the sky, starting on the left and moving clockwise.
  // A good value is 0.08. This is sort of like 9AM sun if the left is the east.
  // float fraction = 0.08f;
  // GLfloat light_position[] = {3.0f + 1.0f * (float)cos(fraction * 2 * M_PI),
  //   -1.0f * (float)cos(fraction * 2 * M_PI), 1.0f * (float)sin(fraction * 2 * M_PI), 0.0};
  // glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // from fraction of the way around the sky, starting on the left and moving clockwise,
  // but pulled toward me, shining from kind of behind me.
  float fraction = 0.18f;
  GLfloat light_position[] = {0.5f + 1.0f * (float)cos(fraction * 2 * M_PI),
    -1.0f * (float)cos(fraction * 2 * M_PI), 1.0f * (float)sin(fraction * 2 * M_PI), 0.0};
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // straight from the left
  // GLfloat light_position[] = {-1.0, 1.0, 0.0, 0.0};
  // glLightfv(GL_LIGHT0, GL_POSITION, light_position);
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

GLuint* TeddyGL::makeTexture(int w, int h, const GLvoid * pixels, bool soften) {
  GLuint* texture = new GLuint[1];
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  if (soften) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexImage2D(GL_TEXTURE_2D, 0, 4, text_surface->w, text_surface->h, 0,
    //   GL_BGRA, GL_UNSIGNED_BYTE, text_surface->pixels);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
  return texture;
}