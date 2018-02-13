/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "graphics.h"

Graphics* graphics = new Graphics();

Graphics::Graphics() {
  this->next_display_list_index = 1;

  // ball = gluNewQuadric();
  // gluQuadricTexture(ball, GL_TRUE);
  // gluQuadricNormals(ball, GLU_SMOOTH);
}

// This method draws a rectangle
void Graphics::drawRectangle(float x, float y, float w, float h) {
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(x, y);
  glTexCoord2d(0.0, 1.0); glVertex2d(x, y + h);
  glTexCoord2d(1.0, 1.0); glVertex2d(x + w, y + h);
  glTexCoord2d(1.0, 0.0); glVertex2d(x + w, y);
  glEnd();
}

// This method sets up the screen for a 2D drawing phase
void Graphics::start2DDraw() {
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
void Graphics::end2DDraw() {
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}


// The fade functions contain extremely useful and generic code
// to fade the screen to and from black based on time inputs.
void Graphics::fadeInOut(float start, float finish, float timeDiff) {
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

void Graphics::fadeIn(float start, float finish, float timeDiff) {
  if (timeDiff < finish) {
    fadeInOut(start, finish + finish - start, timeDiff);
  }
}

void Graphics::fadeOut(float start, float finish, float timeDiff) {
  if (timeDiff < finish - start && 0 < timeDiff) {
    fadeInOut(0, 1, (timeDiff / (2.0 * (finish - start)) + 0.5));
  }
}

void Graphics::blackout() {
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

void Graphics::initializeBasic() {

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
    printf("Error initializing GL config! %d\n", error);
    exit(1);
  }

  // Debug info for now
  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
  printf("OpenGL Shading Language Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void Graphics::initializeOpenGLVersion() {
  // I don't understand setting the version yet. Leaving all of these attempts in here for now.
  // Use OpenGL 4.1
  // SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  // SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  // SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
}

void Graphics::clearScreenWithColor(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  clearScreen();
}

void Graphics::clearScreen() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::set3d(float zoom) {
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

void Graphics::standardCamera(float cam_x, float cam_y, float cam_z, float target_x, float target_y, float target_z) {
  // Set the camera to look down at the character.
  // For fun, change the z-value to change the viewing angle of the game.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  lookAt(cam_x, cam_y, cam_z,
    target_x, target_y, target_z,
    0, 0, 1);  
}

void Graphics::standardLightPosition() {
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

void Graphics::initializeLighting() {
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
  graphics->initializeCelShading();

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing lighting! %d\n", error);
    exit(1);
  }
}

void Graphics::initializeCelShading() {
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

void Graphics::startCelShading() {
  glUseProgram(cel_shader_program);
}

void Graphics::stopCelShading() {
  glUseProgram(0);
}

int* Graphics::makeTexture(int w, int h, const GLvoid * pixels, bool soften) {
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
  return (int*) texture;
}

void Graphics::setTexture(int* texture) {
  glBindTexture(GL_TEXTURE_2D, (GLuint) *texture);
}

void Graphics::matteMaterial() {
  GLfloat material_ambient[] = {0.5, 0.5, 0.5, 1.0};
  GLfloat material_diffuse[] = {0.5, 0.5, 0.5, 1.0};
  GLfloat material_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat shininess[] = {5.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void Graphics::texVert(float t1, float t2, float v1, float v2, float v3) {
  glTexCoord2f(t1, t2);
  glVertex3f(v1, v2, v3);
}

void Graphics::texNormVert(float t1, float t2, float n1, float n2, float n3, float v1, float v2, float v3) {
  glTexCoord2f(t1, t2);
  glNormal3f(n1, n2, n3);
  glVertex3f(v1, v2, v3);
}

void Graphics::face(int size, float data[]) {
  if (size == 4) {
    glBegin(GL_QUADS);
  } else if (size == 3) {
    glBegin(GL_TRIANGLES);
  }

  for (int i = 0; i < size; i++) {
    glTexCoord2f(data[8 * i], data[8 * i + 1]);
    glNormal3f(data[8 * i + 2], data[8 * i + 3], data[8 * i + 4]);
    glVertex3f(data[8 * i + 5], data[8 * i + 6], data[8 * i + 7]);
  }

  glEnd();
}

void Graphics::face2d(double data[]) {
  glBegin(GL_QUADS);

  for (int i = 0; i < 4; i++) {
    glTexCoord2d(data[4 * i], data[4 * i + 1]);
    glVertex2d(data[4 * i + 2], data[4 * i + 3]);
  }

  glEnd();
}

void Graphics::lineWidth(int lineWidth) {
  glLineWidth(lineWidth);
}

void Graphics::lineStrip(std::vector<float> line_data) {
  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < line_data.size() / 3.0; i++) {
    glVertex3f(line_data[3 * i], line_data[3 * i + 1], line_data[3 * i + 2]);
  }
  glEnd();
}

void Graphics::sphere(float radius) {
  // TODO: reimplement if necessary
}

void Graphics::color(float r, float g, float b, float a) {
  glColor4f(r, g, b, a);
}

void Graphics::rotate(float angle, float x, float y, float z) {
  glRotatef(angle, x, y, z);
}

void Graphics::translate(float x, float y, float z) {
  glTranslatef(x, y, z);
}

void Graphics::scale(float x, float y, float z) {
  glScalef(x, y, z);
}

void Graphics::pushMatrix() {
  glPushMatrix();
}

void Graphics::popMatrix() {
  glPopMatrix();
}

void Graphics::multMatrix(const float* m) {
  glMultMatrixf((GLfloat*)m);
}

int Graphics::cacheProgram() {
  int id = this->next_display_list_index;
  this->next_display_list_index++;

  glNewList(id, GL_COMPILE);
  return id;
}

void Graphics::endCacheProgram() {
  glEndList();
}

void Graphics::runCacheProgram(int id) {
  glCallList(id);
}

// https://forums.khronos.org/showthread.php/4991-The-Solution-for-gluLookAt()-Function!!!!
void Graphics::crossProduct(float x1, float y1, float z1, float x2, float y2, float z2, float res[3])
{
  res[0] = y1*z2 - y2*z1;
  res[1] = x2*z1 - x1*z2;
  res[2] = x1*y2 - x2*y1;
}

//https://forums.khronos.org/showthread.php/4991-The-Solution-for-gluLookAt()-Function!!!!
void Graphics::lookAt(float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ)
{
  float f[3];

  // calculating the viewing vector
  f[0] = lookAtX - eyeX;
  f[1] = lookAtY - eyeY;
  f[2] = lookAtZ - eyeZ;

  float fMag = sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
  float upMag = sqrt(upX*upX + upY*upY + upZ*upZ);

  // normalizing the viewing vector
  if( fMag != 0)
  {
    f[0] = f[0]/fMag;
    f[1] = f[1]/fMag;
    f[2] = f[2]/fMag;
  }

  // normalising the up vector
  if( upMag != 0 )
  {
    upX = upX/upMag;
    upY = upY/upMag;
    upZ = upZ/upMag;
  }

  float s[3], u[3];

  crossProduct(f[0], f[1], f[2], upX, upY, upZ, s);
  crossProduct(s[0], s[1], s[2], f[0], f[1], f[2], u);

  float M[]=
  {
    s[0], u[0], -f[0], 0,
    s[1], u[1], -f[1], 0,
    s[2], u[2], -f[2], 0,
    0, 0, 0, 1
  };

  glMultMatrixf(M);
  glTranslatef(-eyeX, -eyeY, -eyeZ); 
}
