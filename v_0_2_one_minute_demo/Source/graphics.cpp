/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "graphics.h"

Graphics* graphics = new Graphics();

Graphics::Graphics() {
  this->next_display_list_index = 1;
  this->next_mesh_cache_id = 1;

  model_stack = {};

  // ball = gluNewQuadric();
  // gluQuadricTexture(ball, GL_TRUE);
  // gluQuadricNormals(ball, GLU_SMOOTH);
}

// This method sets up the screen for a 2D drawing phase
void Graphics::start2DDraw() {
  glDisable(GL_DEPTH_TEST);
  projection = glm::ortho(0.0f, (float) k_screen_width, (float) k_screen_height, 0.0f, 0.0f, 1.0f);
  glUniformMatrix4fv(matrix_id, 1, GL_FALSE, glm::value_ptr(projection));
  color(1.0f, 1.0f, 1.0f, 1.0f);
}

// This method ends the 2D drawing phase
void Graphics::end2DDraw() {
  glEnable(GL_DEPTH_TEST);
}


// The fade functions contain extremely useful and generic code
// to fade the screen to and from black based on time inputs.
void Graphics::fadeInOut(float start, float finish, float timeDiff) {
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  float alpha = 1.0f - sin((timeDiff - start) * M_PI / (finish - start));
  setTexture(black_texture_id);
  color(1.0f, 1.0f, 1.0f, alpha);
  rectangle(0, 0, k_screen_width, k_screen_height);
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
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  setTexture(black_texture_id);
  color(0.0f, 0.0f, 0.0f, 1.0f);
  rectangle(0, 0, k_screen_width, k_screen_height);
}

void Graphics::initialize() {
  initializeBasic();
  initializeShadersAndLighting();
  startNormalShading();
  initializeBuffersAndGeometry();
}

void Graphics::initializeBasic() {

  // glFrontFace(GL_CW);
  // glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  // glShadeModel(GL_SMOOTH);
  // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  // glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Simple viewport.
  glViewport(0, 0, k_screen_width, k_screen_height);

  global_color_vector = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing GL config! %d\n", error);
    exit(1);
  }

  // Make the plain texture to use for effects and stuff
  std::string white_texture_path = k_texture_root_path + "plain.png";
  SDL_Surface* image = IMG_Load(white_texture_path.c_str());
  if (image == nullptr) {
      printf("IMG_Load failed for %s with error: %s\n", white_texture_path.c_str(), IMG_GetError());
  }
  white_texture_id = makeTexture(image->w, image->h, image->pixels, true);

  std::string black_texture_path = k_texture_root_path + "black.png";
  SDL_Surface* image_2 = IMG_Load(black_texture_path.c_str());
  if (image_2 == nullptr) {
      printf("IMG_Load failed for %s with error: %s\n", black_texture_path.c_str(), IMG_GetError());
  }
  black_texture_id = makeTexture(image_2->w, image_2->h, image_2->pixels, true);

  // Debug info for now
  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
  printf("OpenGL Shading Language Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void Graphics::initializeBuffersAndGeometry() {

  global_color_id = glGetUniformLocation(current_program, "global_color_vector");
  matrix_id = glGetUniformLocation(current_program, "mvp_matrix");
  texture_sampler_id = glGetUniformLocation(current_program, "myTextureSampler");

  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  static const GLfloat g_color_buffer_data[] = { 
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
  };

  glGenBuffers(1, &rectangle_color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

  glGenBuffers(1, &primitive_color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, primitive_color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

  model = glm::mat4(1.0);
}

void Graphics::initializeOpenGLVersion() {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
}

void Graphics::clearScreenWithColor(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  clearScreen();
}

void Graphics::clearScreen() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::set3d(float zoom) {
  // // Set one of these perspectives:

  // 1. this is cool, super zoomed out
  // ortho(-50, 50, -50, 50, -50, 50);

  // 2. weirdly reasonable ortho
  // ortho(-zoom * k_aspect_ratio, zoom * k_aspect_ratio, -zoom, zoom, -10 * zoom, 10 * zoom);

  // 3. normal perspective
  // perspective(45.0f,k_screen_width/(1.0 * k_screen_height),0.1f,1000.0f);

  model = glm::mat4(1.0);
  projection = glm::ortho(-zoom * k_aspect_ratio, zoom * k_aspect_ratio, -zoom, zoom, -10 * zoom, 10 * zoom);
  glUniformMatrix4fv(matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
}

void Graphics::standardCamera(float cam_x, float cam_y, float cam_z, float target_x, float target_y, float target_z) {
  // // Set the camera to look down at the character.
  // // For fun, change the z-value to change the viewing angle of the game.
  view = glm::lookAt(glm::vec3(cam_x, cam_y, cam_z),
    glm::vec3(target_x, target_y, target_z),
    glm::vec3(0, 0, 1));
  glUniformMatrix4fv(matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
}

void Graphics::standardLightPosition() {
  // // this code makes a sun
  // // but it requires passing in time fraction
  // // float fraction = (int(last_time - start_time) % k_sun_period) / (float)k_sun_period;
  // // printf("Sun %0.2f\n", fraction);
  // // GLfloat light_position[] = {1.0f * (float)cos(fraction * 2 * M_PI),
  // //   -1.0f * (float)cos(fraction * 2 * M_PI), 1.0f * (float)sin(fraction * 2 * M_PI), 0.0};
  // // glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  // // if (fraction > 0.5f) {
  // //   GLfloat light_diffuse[] = {0.25, 0.25, 0.25, 1.0};
  // //   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  // // } else {
  // //   GLfloat light_diffuse[] = {0.8, 0.8, 0.8, 1.0};
  // //   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  // // }

  // // from fraction of the way around the sky, starting on the left and moving clockwise.
  // // A good value is 0.08. This is sort of like 9AM sun if the left is the east.
  // // float fraction = 0.08f;
  // // GLfloat light_position[] = {3.0f + 1.0f * (float)cos(fraction * 2 * M_PI),
  // //   -1.0f * (float)cos(fraction * 2 * M_PI), 1.0f * (float)sin(fraction * 2 * M_PI), 0.0};
  // // glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // // from fraction of the way around the sky, starting on the left and moving clockwise,
  // // but pulled toward me, shining from kind of behind me.
  // float fraction = 0.18f;
  // GLfloat light_position[] = {0.5f + 1.0f * (float)cos(fraction * 2 * M_PI),
  //   -1.0f * (float)cos(fraction * 2 * M_PI), 1.0f * (float)sin(fraction * 2 * M_PI), 0.0};
  // glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // // straight from the left
  // // GLfloat light_position[] = {-1.0, 1.0, 0.0, 0.0};
  // // glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void Graphics::initializeShadersAndLighting() {
  // // Simple Opengl Lighting
  // glEnable(GL_LIGHTING);
  // glEnable(GL_LIGHT0);
  // GLfloat global_ambient[] = {0.8, 0.8, 0.8, 1.0};
  // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  // glEnable(GL_NORMALIZE);
  // glEnable(GL_RESCALE_NORMAL);

  // GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
  // GLfloat light_diffuse[] = {0.9, 0.9, 0.9, 1.0};
  // GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};

  // glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  // glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  // glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  // // position is defined after the camera, per render.

  // // Common material
  // GLfloat material_ambient[] = {0.8, 0.8, 0.8, 1.0};
  // GLfloat material_diffuse[] = {0.8, 0.8, 0.8, 1.0};
  // GLfloat material_specular[] = {1.0, 1.0, 1.0, 1.0};
  // GLfloat shininess[] = {5.0};
  // glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  // glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
  // glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  // glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

  // Normal Shader
  graphics->initializeNormalShading();

  // Cel Shader
  graphics->initializeCelShading();

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing lighting! %d\n", error);
    exit(1);
  }
}

void Graphics::initializeNormalShading() {
  normal_shader_program = k_normal_shader_id;

  std::string line;

  std::string vertex_shader_path = k_shader_root_path + "normal_shader_vertex.glsl";
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

  std::string fragment_shader_path = k_shader_root_path + "normal_shader_fragment.glsl";
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

  normal_shader_program = glCreateProgram();
  glAttachShader(normal_shader_program, vertex_shader);
  glAttachShader(normal_shader_program, fragment_shader);

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

  glLinkProgram(normal_shader_program);

  glGetProgramiv(normal_shader_program, GL_LINK_STATUS, &status);
  if (!status) {
    printf("Error: failed to link the shader program object.\n");
    exit(1);
  }
}

void Graphics::initializeCelShading() {
  // cel_shader_program = k_cel_shader_id;

  // std::string line;

  // std::string vertex_shader_path = k_shader_root_path + "cel_shader_vertex.glsl";
  // printf("Loading vertex shader from %s\n", vertex_shader_path.c_str());

  // std::ifstream vertex_shader_file;
  // vertex_shader_file.open(vertex_shader_path);

  // if (!vertex_shader_file.is_open()) {
  //   printf("Error: Failed to load vertex shader from %s\n", vertex_shader_path.c_str());
  //   return;
  // }

  // std::string vertex_shader_string = "";
  // while (getline(vertex_shader_file, line)) {
  //   vertex_shader_string += line + "\n";
  // }

  // std::string fragment_shader_path = k_shader_root_path + "cel_shader_fragment.glsl";
  // printf("Loading fragment shader from %s\n", fragment_shader_path.c_str());

  // std::ifstream fragment_shader_file;
  // fragment_shader_file.open(fragment_shader_path);

  // if (!fragment_shader_file.is_open()) {
  //   printf("Error: Failed to load fragment shader from %s\n", fragment_shader_path.c_str());
  //   return;
  // }

  // std::string fragment_shader_string = "";
  // while (getline(fragment_shader_file, line)) {
  //   fragment_shader_string += line + "\n";
  // }

  // const char *vertex_str = vertex_shader_string.c_str();
  // const char *fragment_str = fragment_shader_string.c_str();

  // GLint vertex_shader = 0;
  // GLint fragment_shader = 0;
  // GLint status = 0;

  // vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  // fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  // glShaderSource(vertex_shader, 1, &vertex_str, NULL);
  // glShaderSource(fragment_shader, 1, &fragment_str, NULL);

  // glCompileShader(vertex_shader);
  // glCompileShader(fragment_shader);

  // cel_shader_program = glCreateProgram();
  // glAttachShader(cel_shader_program, vertex_shader);
  // glAttachShader(cel_shader_program, fragment_shader);

  // glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
  // if (!status) {
  //   char log[500];
  //   GLint length;
  //   printf("Error: failed to compile the vertex shader.\n");
  //   glGetShaderInfoLog(vertex_shader, 500, &length, log);
  //   printf("Log: %s\n", log);
  //   exit(1);
  // }

  // glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
  // if (!status) {
  //   char log[500];
  //   GLint length;
  //   printf("Error: failed to compile the fragment shader.\n");
  //   glGetShaderInfoLog(fragment_shader, 500, &length, log);
  //   printf("Log: %s\n", log);
  //   exit(1);
  // }

  // glLinkProgram(cel_shader_program);

  // glGetProgramiv(cel_shader_program, GL_LINK_STATUS, &status);
  // if (!status) {
  //   printf("Error: failed to link the shader program object.\n");
  //   exit(1);
  // }
}

void Graphics::startCelShading() {
  //glUseProgram(cel_shader_program);
  //current_program = cel_shader_program;
}

void Graphics::startNormalShading() {
  glUseProgram(normal_shader_program);
  current_program = normal_shader_program;
}

void Graphics::clearShading() {
  glUseProgram(0);
}

int* Graphics::makeTexture(int w, int h, const GLvoid * pixels, bool soften) {
  GLuint* texture = new GLuint[1];
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  // if (soften) {
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //   glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  // } else {
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //   // glTexImage2D(GL_TEXTURE_2D, 0, 4, text_surface->w, text_surface->h, 0,
  //   //   GL_BGRA, GL_UNSIGNED_BYTE, text_surface->pixels);
  // }

  // Mipmapping currently broken
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
  return (int*) texture;


}

void Graphics::setTexture(int* texture) {
  // glBindTexture(GL_TEXTURE_2D, (GLuint) *texture);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, (GLuint) *texture);
  glUniform1i(texture_sampler_id, 0);
  
}

void Graphics::matteMaterial() {
  // GLfloat material_ambient[] = {0.5, 0.5, 0.5, 1.0};
  // GLfloat material_diffuse[] = {0.5, 0.5, 0.5, 1.0};
  // GLfloat material_specular[] = {1.0, 1.0, 1.0, 1.0};
  // GLfloat shininess[] = {5.0};
  // glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  // glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
  // glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  // glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

// void Graphics::texVert(float t1, float t2, float v1, float v2, float v3) {
//   // glTexCoord2f(t1, t2);
//   // glVertex3f(v1, v2, v3);
// }

// void Graphics::texNormVert(float t1, float t2, float n1, float n2, float n3, float v1, float v2, float v3) {
//   // glTexCoord2f(t1, t2);
//   // glNormal3f(n1, n2, n3);
//   // glVertex3f(v1, v2, v3);
// }


int Graphics::cacheMesh(int size, float vertex_data[], float normal_data[], float texture_data[]) {
  int cache_id = this->next_mesh_cache_id;

  buffer_sizes[cache_id] = size;

  glGenBuffers(1, &vertex_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) * size * 3, vertex_data, GL_STATIC_DRAW);

  glGenBuffers(1, &texture_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texture_data) * size * 2, texture_data, GL_STATIC_DRAW);

  drawMesh(cache_id);
  
  this->next_mesh_cache_id++;
  return cache_id;
}

void Graphics::drawMesh(int cache_id) {
  int size = buffer_sizes[cache_id];

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[cache_id]);
  glVertexAttribPointer(
    0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // 2nd attribute buffer : colors
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, primitive_color_buffer);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    4,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // 3nd attribute buffer : UVs
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffers[cache_id]);
  glVertexAttribPointer(
    2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
    2,                                // size : U+V => 2
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  if (size == 4) {
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

int Graphics::cacheFullMesh(std::vector<float> vertex_data, std::vector<float> normal_data, std::vector<float> texture_data, std::vector<float> color_data) {
  int cache_id = this->next_mesh_cache_id;

  buffer_sizes[cache_id] = vertex_data.size() / 3;
  printf("Buffer size is %d\n", buffer_sizes[cache_id]);
  printf("Given to buffer data function: %d\n", vertex_data.size() * sizeof(float));

  glGenBuffers(1, &vertex_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), &vertex_data[0], GL_STATIC_DRAW);

  glGenBuffers(1, &texture_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, texture_data.size() * sizeof(float), &texture_data[0], GL_STATIC_DRAW);

  glGenBuffers(1, &color_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, color_data.size() * sizeof(float), &color_data[0], GL_STATIC_DRAW);

  drawFullMesh(cache_id);
  
  this->next_mesh_cache_id++;
  return cache_id;
}

void Graphics::drawFullMesh(int cache_id) {
  int size = buffer_sizes[cache_id];

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[cache_id]);
  glVertexAttribPointer(
    0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // 2nd attribute buffer : colors
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffers[cache_id]);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    4,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // 3nd attribute buffer : UVs
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffers[cache_id]);
  glVertexAttribPointer(
    2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
    2,                                // size : U+V => 2
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  glDrawArrays(GL_TRIANGLES, 0, size);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

int Graphics::cacheRectangle(float x, float y, float w, float h) {
  int cache_id = this->next_mesh_cache_id;

  buffer_sizes[cache_id] = 4;

  GLfloat vertex_data[] = { 
    x, y, 0.0f,
    x, y + h, 0.0f,
    x + w, y + h, 0.0f,
    x + w, y, 0.0f,
  };

  GLfloat texture_data[] = { 
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };

  glGenBuffers(1, &vertex_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) * 4 * 3, vertex_data, GL_STATIC_DRAW);

  glGenBuffers(1, &texture_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texture_data) * 4 * 2, texture_data, GL_STATIC_DRAW);

  rectangle(cache_id);
  
  this->next_mesh_cache_id++;
  return cache_id;
}

void Graphics::rectangle(int cache_id) {
  drawMesh(cache_id);
}

// void Graphics::primitive(int size, float vertex_data[], float normal_data[], float texture_data[]) {

//   glGenBuffers(1, &primitive_vertex_buffer);
//   glBindBuffer(GL_ARRAY_BUFFER, primitive_vertex_buffer);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) * size * 3, vertex_data, GL_STATIC_DRAW);

//   glGenBuffers(1, &primitive_texture_buffer);
//   glBindBuffer(GL_ARRAY_BUFFER, primitive_texture_buffer);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(texture_data) * size * 2, texture_data, GL_STATIC_DRAW);

//   // 1rst attribute buffer : vertices
//   glEnableVertexAttribArray(0);
//   glBindBuffer(GL_ARRAY_BUFFER, primitive_vertex_buffer);
//   glVertexAttribPointer(
//     0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
//     3,                  // size
//     GL_FLOAT,           // type
//     GL_FALSE,           // normalized?
//     0,                  // stride
//     (void*)0            // array buffer offset
//   );

//   // 2nd attribute buffer : colors
//   glEnableVertexAttribArray(1);
//   glBindBuffer(GL_ARRAY_BUFFER, primitive_color_buffer);
//   glVertexAttribPointer(
//     1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
//     4,                                // size
//     GL_FLOAT,                         // type
//     GL_FALSE,                         // normalized?
//     0,                                // stride
//     (void*)0                          // array buffer offset
//   );

//   // 3nd attribute buffer : UVs
//   glEnableVertexAttribArray(2);
//   glBindBuffer(GL_ARRAY_BUFFER, primitive_texture_buffer);
//   glVertexAttribPointer(
//     2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
//     2,                                // size : U+V => 2
//     GL_FLOAT,                         // type
//     GL_FALSE,                         // normalized?
//     0,                                // stride
//     (void*)0                          // array buffer offset
//   );

//   if (size == 4) {
//     glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//   } else {
//     glDrawArrays(GL_TRIANGLES, 0, 3);
//   }

//   glDisableVertexAttribArray(0);
//   glDisableVertexAttribArray(1);
//   glDisableVertexAttribArray(2);
// }


// This method draws a rectangle
void Graphics::rectangle(float x, float y, float w, float h) {

  GLfloat vertex_buffer_data[] = { 
    x, y, 0.0f,
    x, y + h, 0.0f,
    x + w, y + h, 0.0f,
    x + w, y, 0.0f,
  };

  GLfloat texture_buffer_data[] = { 
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };

  rectangleWithTexture(vertex_buffer_data, texture_buffer_data);
}

void Graphics::rectangleWithTexture(float vertex_data[], float texture_data[]) {
  glGenBuffers(1, &rectangle_vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) * 4 * 3, vertex_data, GL_STATIC_DRAW);

  glGenBuffers(1, &rectangle_texture_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_texture_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texture_data) * 4 * 2, texture_data, GL_STATIC_DRAW);

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_vertex_buffer);
  glVertexAttribPointer(
    0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // 2nd attribute buffer : colors
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_color_buffer);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    4,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // 3nd attribute buffer : UVs
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_texture_buffer);
  glVertexAttribPointer(
    2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
    2,                                // size : U+V => 2
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

void Graphics::lineWidth(int lineWidth) {
  // glLineWidth(lineWidth);
}

void Graphics::lineStrip(std::vector<float> line_data) {
  // glBegin(GL_LINE_STRIP);
  // for (int i = 0; i < line_data.size() / 3.0; i++) {
  //   glVertex3f(line_data[3 * i], line_data[3 * i + 1], line_data[3 * i + 2]);
  // }
  // glEnd();
}

void Graphics::sphere(float radius) {
  // TODO: reimplement if necessary
}

void Graphics::color(float r, float g, float b, float a) {
  global_color_vector = glm::vec4(r, g, b, a);
  glUniform4fv(global_color_id, 1, glm::value_ptr(global_color_vector));
}

void Graphics::rotate(float angle, float x, float y, float z) {
  model = model * glm::rotate(glm::radians(angle), glm::vec3(x, y, z));
  glUniformMatrix4fv(matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
}

void Graphics::translate(float x, float y, float z) {
  model = model * glm::translate(glm::vec3(x, y, z));
  glUniformMatrix4fv(matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
}

void Graphics::scale(float x, float y, float z) {
  model = model * glm::scale(glm::vec3(x, y, z));
  glUniformMatrix4fv(matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
}

void Graphics::pushModelMatrix() {
  model_stack.push_back(model);
}

void Graphics::popModelMatrix() {
  model = model_stack.back();
  model_stack.pop_back();
}

void Graphics::multMatrix(const float* m) {
  glm::mat4 mult = glm::make_mat4(m);
  model = model * mult;
  // glMultMatrixf((GLfloat*)m);
  glUniformMatrix4fv(matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
}

// int Graphics::cacheProgram() {
//   // int id = this->next_display_list_index;
//   // this->next_display_list_index++;

//   // glNewList(id, GL_COMPILE);
//   // return id;
// }

// void Graphics::endCacheProgram() {
//   // glEndList();
// }

// void Graphics::runCacheProgram(int id) {
//   // glCallList(id);
// }

// // https://forums.khronos.org/showthread.php/4991-The-Solution-for-gluLookAt()-Function!!!!
// void Graphics::crossProduct(float x1, float y1, float z1, float x2, float y2, float z2, float res[3])
// {
//   res[0] = y1*z2 - y2*z1;
//   res[1] = x2*z1 - x1*z2;
//   res[2] = x1*y2 - x2*y1;
// }

// //https://forums.khronos.org/showthread.php/4991-The-Solution-for-gluLookAt()-Function!!!!
// void Graphics::lookAt(float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ)
// {
//   // float f[3];

//   // // calculating the viewing vector
//   // f[0] = lookAtX - eyeX;
//   // f[1] = lookAtY - eyeY;
//   // f[2] = lookAtZ - eyeZ;

//   // float fMag = sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
//   // float upMag = sqrt(upX*upX + upY*upY + upZ*upZ);

//   // // normalizing the viewing vector
//   // if( fMag != 0)
//   // {
//   //   f[0] = f[0]/fMag;
//   //   f[1] = f[1]/fMag;
//   //   f[2] = f[2]/fMag;
//   // }

//   // // normalising the up vector
//   // if( upMag != 0 )
//   // {
//   //   upX = upX/upMag;
//   //   upY = upY/upMag;
//   //   upZ = upZ/upMag;
//   // }

//   // float s[3], u[3];

//   // crossProduct(f[0], f[1], f[2], upX, upY, upZ, s);
//   // crossProduct(s[0], s[1], s[2], f[0], f[1], f[2], u);

//   // float M[]=
//   // {
//   //   s[0], u[0], -f[0], 0,
//   //   s[1], u[1], -f[1], 0,
//   //   s[2], u[2], -f[2], 0,
//   //   0, 0, 0, 1
//   // };

//   // glMultMatrixf(M);
//   // glTranslatef(-eyeX, -eyeY, -eyeZ); 
// }
