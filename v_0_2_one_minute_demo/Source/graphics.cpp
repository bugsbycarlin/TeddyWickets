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

  rectangle_cache = {};

  initialized_rectangle_buffers = false;
}

// This method sets up the screen for a 2D drawing phase
void Graphics::start2DDraw() {
  glDisable(GL_DEPTH_TEST);
  disableLights();
  projection = glm::ortho(0.0f, (float) k_screen_width, (float) k_screen_height, 0.0f, 0.0f, 1.0f);
  glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, glm::value_ptr(projection));
  color(1.0f, 1.0f, 1.0f, 1.0f);
}

// This method ends the 2D drawing phase
void Graphics::end2DDraw() {
  glEnable(GL_DEPTH_TEST);
  enableLights();
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
  } else if (timeDiff > finish - start) {
    // all black
    setTexture(black_texture_id);
    color(1.0f, 1.0f, 1.0f, 1.0f);
    rectangle(0, 0, k_screen_width, k_screen_height);
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
  // startNormalShading();
  glUseProgram(shader_program);
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

  global_color_id = glGetUniformLocation(shader_program, "global_color_vector");
  mvp_matrix_id = glGetUniformLocation(shader_program, "mvp_matrix");
  v_matrix_id = glGetUniformLocation(shader_program, "v_matrix");
  m_matrix_id = glGetUniformLocation(shader_program, "m_matrix");
  texture_sampler_id = glGetUniformLocation(shader_program, "myTextureSampler");
  bool_cel_shading_id = glGetUniformLocation(shader_program, "bool_cel_shading");
  bool_lighting_id = glGetUniformLocation(shader_program, "bool_lighting");
  light_position_worldspace_id = glGetUniformLocation(shader_program, "light_position_worldspace");

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
  view = glm::mat4(1.0);
  projection = glm::ortho(-zoom * k_aspect_ratio, zoom * k_aspect_ratio, -zoom, zoom, -10 * zoom, 10 * zoom);
  glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
  glUniformMatrix4fv(m_matrix_id, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(v_matrix_id, 1, GL_FALSE, glm::value_ptr(view));
}

void Graphics::standardCamera(float cam_x, float cam_y, float cam_z, float target_x, float target_y, float target_z) {
  // // Set the camera to look down at the character.
  // // For fun, change the z-value to change the viewing angle of the game.
  view = glm::lookAt(glm::vec3(cam_x, cam_y, cam_z),
    glm::vec3(target_x, target_y, target_z),
    glm::vec3(0, 0, 1));
  glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
  glUniformMatrix4fv(v_matrix_id, 1, GL_FALSE, glm::value_ptr(view));
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

  //glUniform3fv(light_position_worldspace_id, 1, glm::value_ptr(glm::vec3(2, 2, 5)));

  glUniform3fv(light_position_worldspace_id, 1, glm::value_ptr(glm::vec3(2, 0, 5)));
}

void Graphics::setLightPosition(float x, float y, float z) {
  glUniform3fv(light_position_worldspace_id, 1, glm::value_ptr(glm::vec3(x, y, z)));
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
  // graphics->initializeCelShading();

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing lighting! %d\n", error);
    exit(1);
  }
}

void Graphics::initializeNormalShading() {
  shader_program = k_shader_id;

  std::string line;

  std::string vertex_shader_path = k_shader_root_path + "shader_vertex.glsl";
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

  std::string fragment_shader_path = k_shader_root_path + "shader_fragment.glsl";
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

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);

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

  glLinkProgram(shader_program);

  glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
  if (!status) {
    printf("Error: failed to link the shader program object.\n");
    exit(1);
  }
}

void Graphics::enableCelShading() {
  glUniform1f(bool_cel_shading_id, 1.0);
}

void Graphics::disableCelShading() {
  glUniform1f(bool_cel_shading_id, 0.0);
}

void Graphics::enableLights() {
  glUniform1f(bool_lighting_id, 1.0);
}

void Graphics::disableLights() {
  glUniform1f(bool_lighting_id, 0.0);
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

int Graphics::cacheMesh(int size, float vertex_data[], float normal_data[], float texture_data[]) {
  int cache_id = this->next_mesh_cache_id;

  buffer_sizes[cache_id] = size;

  glGenBuffers(1, &vertex_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) * size * 3, vertex_data, GL_STATIC_DRAW);

  glGenBuffers(1, &normal_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normal_data) * size * 3, normal_data, GL_STATIC_DRAW);

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

  // 2nd attribute buffer : normals
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffers[cache_id]);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // 3rd attribute buffer : colors
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, primitive_color_buffer);
  glVertexAttribPointer(
    2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
    4,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // 4th attribute buffer : textures
  glEnableVertexAttribArray(3);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffers[cache_id]);
  glVertexAttribPointer(
    3,                                // attribute. No particular reason for 3, but must match the layout in the shader.
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
  glDisableVertexAttribArray(3);
}

int Graphics::cacheFullMesh(std::vector<float> vertex_data, std::vector<float> normal_data, std::vector<float> texture_data, std::vector<float> color_data) {
  int cache_id = this->next_mesh_cache_id;

  buffer_sizes[cache_id] = vertex_data.size() / 3;
  //printf("Buffer size is %d\n", buffer_sizes[cache_id]);
  //printf("Given to buffer data function: %d\n", vertex_data.size() * sizeof(float));

  glGenBuffers(1, &vertex_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), &vertex_data[0], GL_STATIC_DRAW);

  glGenBuffers(1, &normal_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, normal_data.size() * sizeof(float), &normal_data[0], GL_STATIC_DRAW);

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

  // 2nd attribute buffer : normals
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffers[cache_id]);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // 3rd attribute buffer : colors
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffers[cache_id]);
  glVertexAttribPointer(
    2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    4,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // 4th attribute buffer : textures
  glEnableVertexAttribArray(3);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffers[cache_id]);
  glVertexAttribPointer(
    3,                                // attribute. No particular reason for 2, but must match the layout in the shader.
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
  glDisableVertexAttribArray(3);
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

  GLfloat normal_data[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
  };

  glGenBuffers(1, &vertex_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) * 4 * 3, vertex_data, GL_STATIC_DRAW);

  glGenBuffers(1, &normal_buffers[cache_id]);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffers[cache_id]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normal_data) * 4 * 3, normal_data, GL_STATIC_DRAW);

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

// This method draws a rectangle
void Graphics::rectangle(float x, float y, float w, float h) {
  std::string id = std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(w) + "," + std::to_string(h);

  if (rectangle_cache.find(id) == rectangle_cache.end()) {
    rectangle_cache[id] = cacheRectangle(x, y, w, h);
  } else {
    drawMesh(rectangle_cache[id]);
  }
}

void Graphics::rectangleWithTexture(float vertex_data[], float texture_data[]) {
  if (!initialized_rectangle_buffers) {
    GLfloat normal_data[] = { 
      0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f
    };

    glGenBuffers(1, &rectangle_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, rectangle_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) * 4 * 3, vertex_data, GL_STATIC_DRAW);

    glGenBuffers(1, &rectangle_normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, rectangle_normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal_data) * 4 * 3, normal_data, GL_STATIC_DRAW);

    glGenBuffers(1, &rectangle_texture_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, rectangle_texture_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_data) * 4 * 2, texture_data, GL_STATIC_DRAW);
  } else {
    glBufferSubData(rectangle_vertex_buffer, 0, sizeof(vertex_data) * 4 * 3, vertex_data);
    glBufferSubData(rectangle_texture_buffer, 0, sizeof(texture_data) * 4 * 2, texture_data);
  }

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
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_normal_buffer);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // 3rd attribute buffer : colors
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_color_buffer);
  glVertexAttribPointer(
    2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    4,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // 4th attribute buffer : textures
  glEnableVertexAttribArray(3);
  glBindBuffer(GL_ARRAY_BUFFER, rectangle_texture_buffer);
  glVertexAttribPointer(
    3,                                // attribute. No particular reason for 2, but must match the layout in the shader.
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
  glDisableVertexAttribArray(3);
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

void Graphics::color(float r, float g, float b, float a) {
  global_color_vector = glm::vec4(r, g, b, a);
  glUniform4fv(global_color_id, 1, glm::value_ptr(global_color_vector));
}

void Graphics::rotate(float angle, float x, float y, float z) {
  model = model * glm::rotate(glm::radians(angle), glm::vec3(x, y, z));
  glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
  glUniformMatrix4fv(m_matrix_id, 1, GL_FALSE, glm::value_ptr(model));
}

void Graphics::translate(float x, float y, float z) {
  model = model * glm::translate(glm::vec3(x, y, z));
  glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
  glUniformMatrix4fv(m_matrix_id, 1, GL_FALSE, glm::value_ptr(model));
}

void Graphics::scale(float x, float y, float z) {
  model = model * glm::scale(glm::vec3(x, y, z));
  glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
  glUniformMatrix4fv(m_matrix_id, 1, GL_FALSE, glm::value_ptr(model));
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
  glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, glm::value_ptr(projection * view * model));
  glUniformMatrix4fv(m_matrix_id, 1, GL_FALSE, glm::value_ptr(model));
}

glm::vec3 Graphics::get2dCoords(float x, float y, float z) {
  glm::vec3 coords = glm::project(glm::vec3(x, y, z), model, projection, glm::vec4(0, 0, k_screen_width, k_screen_height));
  //printf("Here are coords: %0.2f, %0.2f, %0.2f\n", coords.x, coords.y, coords.z);
  return coords;
}

