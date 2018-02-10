/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Model Viewer looks at models up close.

*/

#include <string>

#include "model_viewer.h"
#include "globals.h"
#include "graphics.h"

// The window
SDL_Window* window;

// The OpenGL context
SDL_GLContext context;

bool initialize() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  graphics->initializeOpenGLVersion();

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

  while (!screen->quit) {
    screen->loop(window, NULL);
  }

  if (screen != NULL) {
    screen->shutdown();
  }

  // Disable text input
  SDL_StopTextInput();

  shutdown();
}
