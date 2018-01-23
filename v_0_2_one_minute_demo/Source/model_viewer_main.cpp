/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Model Viewer looks at models up close.

*/

#include <string>

#include "model_viewer.h"

// The window
SDL_Window* window;

// The OpenGL context
SDL_GLContext context;

bool initializeOpenGL() {
  // Initialize Projection Matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Initialize Modelview Matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Initialize clear color
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

  // Check for error
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing OpenGL! %s\n", gluErrorString(error));
    return false;
  }

  return true;
}

bool initialize() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Use OpenGL 2.1
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  // Create window
  window = SDL_CreateWindow("Teddy Wickets Model Viewer",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    k_screen_width, k_screen_height,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

  // Use Vsync
  if (SDL_GL_SetSwapInterval(1) < 0) {
    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // Initialize OpenGL
  if (!initializeOpenGL()) {
    printf("Unable to initialize OpenGL!\n");
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

int main(int argc, char* args[]) {
  initialize();

  // Enable text input
  SDL_StartTextInput();

  int current_screen = k_model_viewer_screen;

  Screen* screen = new ModelViewer();

  screen->initialize();

  while(!screen->quit) {
    screen->loop(window, NULL);
  }

  if (screen != NULL) {
    screen->shutdown();
  }

  // Disable text input
  SDL_StopTextInput();

  shutdown();
}
