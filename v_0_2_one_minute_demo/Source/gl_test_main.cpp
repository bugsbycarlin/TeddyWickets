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

GLuint textureID;

GLuint gVAO = 0;
GLuint gVBO = 0;

GLuint VertexArrayID;
GLuint MatrixID;
GLuint vertexbuffer;
GLuint colorbuffer;
GLuint uvbuffer;
glm::mat4 MVP;
GLuint texture_location;

float screen_x = 1440;
float screen_y = 900;
int test_shader_program;
GLenum error;
GLint transform_location;

void loadTexture() {
  std::string path = "Art/title_screen_draft_5.png";
  //std::string path = "Art/tiles4.png";

  // Use the SDL to load the image
  SDL_Surface* image = IMG_Load(path.c_str());
  if (image == nullptr) {
      printf("IMG_Load failed for %s with error: %s\n", path.c_str(), IMG_GetError());
  }

  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  
  // // // deprecated,
  // // //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  // // // use this intead
  // glGenerateMipmap(GL_TEXTURE_2D);

  // WOW MIPMAPS BREAK ME VERY BADLY.

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  printf("Bytes per pixel: %d\n", image->format->BytesPerPixel);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image->pixels);

  Uint32 *pixels = (Uint32 *)image->pixels;
  for (int i = 0; i < 16; i++) {
    printf("Pixels %d: %u\n", i, pixels[i]);
  }
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
  //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

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

  std::string vertex_shader_path = "Shaders/gl_test_vertex_4.glsl";
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

  std::string fragment_shader_path = "Shaders/gl_test_fragment_4.glsl";
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

void loadGeometry2() {
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  MatrixID = glGetUniformLocation(test_shader_program, "MVP");

  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
  // Camera matrix
  glm::mat4 View       = glm::lookAt(
                glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
                glm::vec3(0,0,0), // and looks at the origin
                glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
               );
  // Model matrix : an identity matrix (model will be at the origin)
  glm::mat4 Model      = glm::mat4(1.0f);
  // Our ModelViewProjection : multiplication of our 3 matrices
  MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

  // Get a handle for our "myTextureSampler" uniform
  texture_location  = glGetUniformLocation(test_shader_program, "myTextureSampler");
  printf("Texture location is %d\n", texture_location);

  // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
  // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
  static const GLfloat g_vertex_buffer_data[] = { 
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f
  };

  // One color for each vertex. They were generated randomly.
  static const GLfloat g_color_buffer_data[] = { 
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
  };

  // Two UV coordinatesfor each vertex. They were created with Blender.
  static const GLfloat g_uv_buffer_data[] = { 
    0.000059f, 1.0f-0.000004f, 
    0.000103f, 1.0f-0.336048f, 
    0.335973f, 1.0f-0.335903f, 
    1.000023f, 1.0f-0.000013f, 
    0.667979f, 1.0f-0.335851f, 
    0.999958f, 1.0f-0.336064f, 
    0.667979f, 1.0f-0.335851f, 
    0.336024f, 1.0f-0.671877f, 
    0.667969f, 1.0f-0.671889f, 
    1.000023f, 1.0f-0.000013f, 
    0.668104f, 1.0f-0.000013f, 
    0.667979f, 1.0f-0.335851f, 
    0.000059f, 1.0f-0.000004f, 
    0.335973f, 1.0f-0.335903f, 
    0.336098f, 1.0f-0.000071f, 
    0.667979f, 1.0f-0.335851f, 
    0.335973f, 1.0f-0.335903f, 
    0.336024f, 1.0f-0.671877f, 
    1.000004f, 1.0f-0.671847f, 
    0.999958f, 1.0f-0.336064f, 
    0.667979f, 1.0f-0.335851f, 
    0.668104f, 1.0f-0.000013f, 
    0.335973f, 1.0f-0.335903f, 
    0.667979f, 1.0f-0.335851f, 
    0.335973f, 1.0f-0.335903f, 
    0.668104f, 1.0f-0.000013f, 
    0.336098f, 1.0f-0.000071f, 
    0.000103f, 1.0f-0.336048f, 
    0.000004f, 1.0f-0.671870f, 
    0.336024f, 1.0f-0.671877f, 
    0.000103f, 1.0f-0.336048f, 
    0.336024f, 1.0f-0.671877f, 
    0.335973f, 1.0f-0.335903f, 
    0.667969f, 1.0f-0.671889f, 
    1.000004f, 1.0f-0.671847f, 
    0.667979f, 1.0f-0.335851f
  };

  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

}

void loadGeometry() {
  // ////////
  // //// New method for vertices and stuff

  // // make and bind the VAO
  // glGenVertexArrays(1, &gVAO);
  // glBindVertexArray(gVAO);
  
  // // make and bind the VBO
  // glGenBuffers(1, &gVBO);
  // glBindBuffer(GL_ARRAY_BUFFER, gVBO);

  // error = glGetError();
  // if (error != GL_NO_ERROR) {
  //   printf("Error initializing GL config 3a! %d\n", error);
  //   exit(1);
  // }
  
  // // Put the three triangle verticies into the VBO
  // GLfloat vertexData[] = {
  //       //  X         Y     Z     R     G     B     U     V
  //      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
  //      0.0f, 400.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  //      400.0f,  400.0f,  0.0f, 0.0f, 0.8f, 0.8f, 1.0f, 1.0f,
  //      400.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
  // };
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

  // error = glGetError();
  // if (error != GL_NO_ERROR) {
  //   printf("Error initializing GL config 4a! %d\n", error);
  //   exit(1);
  // }

  // /* Position attribute */
  // glVertexAttribPointer(glGetAttribLocation(test_shader_program, "position"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
  // glEnableVertexAttribArray(glGetAttribLocation(test_shader_program, "position"));

  // error = glGetError();
  // if (error != GL_NO_ERROR) {
  //   printf("Error initializing GL config 6a! %d\n", error);
  //   exit(1);
  // }

  // /* Color attribute */
  // glVertexAttribPointer(glGetAttribLocation(test_shader_program, "color"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  // glEnableVertexAttribArray(glGetAttribLocation(test_shader_program, "color"));

  // error = glGetError();
  // if (error != GL_NO_ERROR) {
  //   printf("Error initializing GL config 7a! %d\n", error);
  //   exit(1);
  // }

  // /* Texture attribute */
  // glVertexAttribPointer(glGetAttribLocation(test_shader_program, "vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  // glEnableVertexAttribArray(glGetAttribLocation(test_shader_program, "vertTexCoord"));

  // error = glGetError();
  // if (error != GL_NO_ERROR) {
  //   printf("Error initializing GL config 8a! %d\n", error);
  //   exit(1);
  // }

  // // unbind the VBO and VAO
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindVertexArray(0);
}

void doThings() {
    // //glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture[0]);

    // texture_location = glGetUniformLocation(test_shader_program, "myTextureSampler");
    // glUniform1i(texture_location, 0);

    // // http://www.songho.ca/opengl/gl_projectionmatrix.html
    // // Ortho matrix
    // // right = screen_x
    // // left = 0
    // // top = 0
    // // bottom = screen_y
    // // near = 0
    // // far = 1
    // // tx = -1 * (right + left) / (right - left) = -1 * (screen_x + 0) / (screen_x - 0) = -1
    // // ty = -1 * (top + bottom) / (top - bottom) = -1 * (0 + screen_y) / (0 - screen_y) = 1
    // // tz = -1 * (far + near) / (far - near) = -1 * (1 + 0) / (1 - 0) = -1
    // transform_location = glGetUniformLocation(test_shader_program, "transform");
    // glm::mat4 projection = glm::ortho(0.0f, screen_x, screen_y, 0.0f, 0.0f, 1.0f);
    // // GLfloat transform[] = {
    // //     2.0f / screen_x, 0.0f, 0.0f, 0.0f,
    // //     0.0f, -2.0f / screen_y, 0.0f, 0.0f,
    // //     0.0f, 0.0f, -2.0f, -1.0f,
    // //     0.0f, 0.0f, 0.0f, 1.0f,
    // // };
    // glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(projection));
        
    // // bind the VAO (the triangle)
    // glBindVertexArray(gVAO);
    
    // // draw the VAO
    // glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    // // unbind the VAO
    // glBindVertexArray(0);
}

void doThings2() {

  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
  //try this in a minute
  //glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(MVP));

  texture_location  = glGetUniformLocation(test_shader_program, "myTextureSampler");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glUniform1i(texture_location, 0);
  


  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    3,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // 3nd attribute buffer : UVs
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glVertexAttribPointer(
    2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
    2,                                // size : U+V => 2
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

int main(int argc, char* args[]) {
  initialize();

  // Enable text input
  SDL_StartTextInput();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); 

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

  loadTexture();

  loadGeometry2();
  
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

    doThings2();
    
    // unbind the program
    glUseProgram(0);

    // Update screen
    SDL_GL_SwapWindow(window);
  }

  // Cleanup VBO and shader
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &colorbuffer);
  glDeleteProgram(test_shader_program);
  glDeleteTextures(1, &textureID);
  glDeleteVertexArrays(1, &VertexArrayID);


  // Disable text input
  SDL_StopTextInput();

  shutdown();
}
