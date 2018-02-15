/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Testing OpenGL version.
*/

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>
#include <OpenGL/gl3.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>

// The window
SDL_Window* window;

SDL_GLContext context;

GLuint* texture;
GLuint gVAO = 0;
GLuint gVBO = 0;
float screen_x = 1440;
float screen_y = 900;
int test_shader_program;
GLenum error;
GLint transform_location;
GLint texture_location;

void loadTexture() {
  std::string path = "Art/title_screen_draft_5.png";

  // Use the SDL to load the image
  SDL_Surface* image = IMG_Load(path.c_str());
  if (image == nullptr) {
      printf("IMG_Load failed for %s with error: %s\n", path.c_str(), IMG_GetError());
  }

  texture = new GLuint[1];
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  
  // deprecated,
  //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  // use this intead
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_x, screen_y, 0, GL_BGRA, GL_UNSIGNED_BYTE, image->pixels);
}

bool initialize() {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // With gl3.h:
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // Create window
  window = SDL_CreateWindow("Teddy Wickets",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    screen_x, screen_y,
    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  if (window == NULL) {
    printf("Window could not be created. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Create context
  context = SDL_GL_CreateContext(window);
  if (context == NULL) {
    printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  printf("gl_renderer: %s\n", glGetString(GL_RENDERER));
  printf("gl_context: %s\n", glGetString(GL_VERSION));
  printf("gl_shading_lang_version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  // Use Vsync
  if (SDL_GL_SetSwapInterval(1) < 0) {
    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  loadTexture();
}

void shutdown() {
  // Destroy window
  SDL_DestroyWindow(window);
  window = NULL;

  // Quit SDL subsystems
  SDL_Quit();
}

void loadShaders() {
  std::string line;

  std::string vertex_shader_path = "Shaders/gl_test_vertex_3.glsl";
  printf("Loading vertex shader from %s\n", vertex_shader_path.c_str());

  std::ifstream vertex_shader_file;
  vertex_shader_file.open(vertex_shader_path);

  if (!vertex_shader_file.is_open()) {
    printf("Error: Failed to load vertex shader from %s\n", vertex_shader_path.c_str());
    exit(1);
  }

  std::string vertex_shader_string = "";
  while (getline(vertex_shader_file, line)) {
    vertex_shader_string += line + "\n";
  }

  std::string fragment_shader_path = "Shaders/gl_test_fragment_3.glsl";
  printf("Loading fragment shader from %s\n", fragment_shader_path.c_str());

  std::ifstream fragment_shader_file;
  fragment_shader_file.open(fragment_shader_path);

  if (!fragment_shader_file.is_open()) {
    printf("Error: Failed to load fragment shader from %s\n", fragment_shader_path.c_str());
    exit(1);
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

  test_shader_program = glCreateProgram();
  glAttachShader(test_shader_program, vertex_shader);
  glAttachShader(test_shader_program, fragment_shader);

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

  glLinkProgram(test_shader_program);

  glGetProgramiv(test_shader_program, GL_LINK_STATUS, &status);
  if (!status) {
    printf("Error: failed to link the shader program object.\n");
    exit(1);
  }
}

void loadGeometry() {
  ////////
  //// New method for vertices and stuff

  // make and bind the VAO
  glGenVertexArrays(1, &gVAO);
  glBindVertexArray(gVAO);
  
  // make and bind the VBO
  glGenBuffers(1, &gVBO);
  glBindBuffer(GL_ARRAY_BUFFER, gVBO);

  error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing GL config 3a! %d\n", error);
    exit(1);
  }
  
  // Put the three triangle verticies into the VBO
  GLfloat vertexData[] = {
        //  X         Y     Z     R     G     B     U     V
       0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       0.0f, 400.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
       400.0f,  400.0f,  0.0f, 0.0f, 0.8f, 0.8f, 1.0f, 1.0f,
       400.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

  error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing GL config 4a! %d\n", error);
    exit(1);
  }

  /* Position attribute */
  glVertexAttribPointer(glGetAttribLocation(test_shader_program, "position"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(glGetAttribLocation(test_shader_program, "position"));

  error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing GL config 6a! %d\n", error);
    exit(1);
  }

  /* Color attribute */
  glVertexAttribPointer(glGetAttribLocation(test_shader_program, "color"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(glGetAttribLocation(test_shader_program, "color"));

  error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing GL config 7a! %d\n", error);
    exit(1);
  }

  /* Texture attribute */
  glVertexAttribPointer(glGetAttribLocation(test_shader_program, "vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(glGetAttribLocation(test_shader_program, "vertTexCoord"));

  error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing GL config 8a! %d\n", error);
    exit(1);
  }

  // unbind the VBO and VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

int main(int argc, char* args[]) {
  initialize();

  // Enable text input
  SDL_StartTextInput();

  glEnable(GL_DEPTH_TEST);

  // These are dead
  //glShadeModel(GL_SMOOTH);
  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // This is not dead
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // This is dead
  //glEnable(GL_TEXTURE_2D);

  loadShaders();

  loadGeometry();
  
  // Simple viewport.
  glViewport(0, 0, screen_x, screen_y);

  error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing GL config 7! %d\n", error);
    exit(1);
  }

  // Debug info for now
  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
  printf("OpenGL Shading Language Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


  bool quit = false;
  float t_x = 0.0f;
  float t_y = 0.0f;
  while (!quit) {
    
    // Event handler
    SDL_Event e;

    // Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_KEYDOWN) {
        quit = true;
      }
    }

    glClearColor(0.67, 0.97, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind the program (the shaders)
    glUseProgram(test_shader_program);

    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    texture_location = glGetUniformLocation(test_shader_program, "myTextureSampler");
    glUniform1i(texture_location, 0);

    // http://www.songho.ca/opengl/gl_projectionmatrix.html
    // Ortho matrix
    // right = screen_x
    // left = 0
    // top = 0
    // bottom = screen_y
    // near = 0
    // far = 1
    // tx = -1 * (right + left) / (right - left) = -1 * (screen_x + 0) / (screen_x - 0) = -1
    // ty = -1 * (top + bottom) / (top - bottom) = -1 * (0 + screen_y) / (0 - screen_y) = 1
    // tz = -1 * (far + near) / (far - near) = -1 * (1 + 0) / (1 - 0) = -1
    transform_location = glGetUniformLocation(test_shader_program, "transform");
    glm::mat4 projection = glm::ortho(0.0f, screen_x, screen_y, 0.0f, 0.0f, 1.0f);
    // GLfloat transform[] = {
    //     2.0f / screen_x, 0.0f, 0.0f, 0.0f,
    //     0.0f, -2.0f / screen_y, 0.0f, 0.0f,
    //     0.0f, 0.0f, -2.0f, -1.0f,
    //     0.0f, 0.0f, 0.0f, 1.0f,
    // };
    glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(projection));
        
    // bind the VAO (the triangle)
    glBindVertexArray(gVAO);
    
    // draw the VAO
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    // unbind the VAO
    glBindVertexArray(0);
    
    // unbind the program
    glUseProgram(0);

    // Update screen
    SDL_GL_SwapWindow(window);
  }

  // Disable text input
  SDL_StopTextInput();

  shutdown();
}
