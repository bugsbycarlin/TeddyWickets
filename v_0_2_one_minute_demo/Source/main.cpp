/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Currently working on V0.2 - One Minute Demo

  This is a one minute playable demo of the game. Changes are:

  +- Support for multiple modes (ie different screens)
  +- Title screen
  - Fully realized game mechanic
  - Two single player levels (?)
  - One multiplayer (2 player) level
  - Game controller / keyboard / mouse setup screen
  - Controller support
  - Keymap support(?)
  - Level editor (separate program)
  - Level saver
  - Level loader
  - Model loader (models come from MayaLT)
  - Model viewer (separate program)
  - Abstract away OpenGL in wrappers
  - More fully understand lighting model
  - Cel shading
  - Comic patter inside the game (ie, can put speech bubbles on things)
  - Comic screen
  - Game stuff (to be added as I design the game mechanic)


  See ../../README.md for project details.
*/

#include <string>

#include "fmod.hpp"
#include "fmod_errors.h"

#include "game.h"
#include "title.h"

// The window
SDL_Window* window;

// The OpenGL context
SDL_GLContext context;

// Music
FMOD_RESULT result;
FMOD::Sound *audio_stream;
FMOD::System *sound_system = NULL;

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
  window = SDL_CreateWindow("Teddy Wickets",
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

  // Initialize Audio
  result = FMOD::System_Create(&sound_system);
  if (result != FMOD_OK) {
      printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
      return false;
  }

  result = sound_system->init(512, FMOD_INIT_NORMAL, 0);
  if (result != FMOD_OK) {
      printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
      return false;
  }
}

void shutdown() {
  // Destroy window
  SDL_DestroyWindow(window);
  window = NULL;

  sound_system->close();

  // Quit SDL subsystems
  SDL_Quit();
}

int main(int argc, char* args[]) {
  initialize();

  sound_system->createStream("Sound/Bias_Groove.mp3", FMOD_DEFAULT, 0, &audio_stream);

  // Sound disabled for the moment.
  // sound_system->playSound(audio_stream, NULL, false, 0);

  // Enable text input
  SDL_StartTextInput();

  int current_screen;
  if (argc > 1 && (std::string(args[1]) == "title" || std::string(args[1]) == "Title")) {
    current_screen = k_title_screen;
  } else {
    current_screen = k_1p_game_screen;
  }

  Screen* screen = NULL;

  bool quit = false;
  while(!quit) {
    if (screen == NULL || screen->current_screen != current_screen) {
      if (screen != NULL) {
        current_screen = screen->current_screen;
        screen->shutdown();
      }

      if (current_screen == k_title_screen) {
        screen = new Title();
      } else if (current_screen == k_1p_game_screen) {
        screen = new Game();
      }

      if (!screen->initialize()) {
        quit = true;
        break;
      }
    }

    screen->loop(window, sound_system);

    if (screen != NULL && screen->quit == true) {
      screen->shutdown();
      quit = true;
    }
  }

  // Disable text input
  SDL_StopTextInput();

  shutdown();
}