/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Currently working on V0.2 - One Minute Demo

  This is a short playable demo of the game. Changes are:

  +- Support for multiple modes (ie different screens)
  +- Title screen
  +- Model loader (models come from MayaLT)
  +- Model viewer (separate program)
  +- Add font support with SDL2_ttf
  +- Controller support
  +- Keymapping / controller mapping
  +- Game controller / keyboard setup screen
  +- Bear selection mode in the 2p game
  +- Improved lighting model
  +- Abstract away OpenGL in wrappers
  +- Upgrade from OpenGL 2.1 to OpenGL 4.1 and rewrite graphics to fit
  +- Cel shading
  +- Improved color palette
  +- Model caching
  +- Level editor (save, load, make levels)
  
  - Refined cel shading
  - cel shade outlining
  
  - improve color palette even more


  - Remove mouse support
  - Fully realized game mechanic
  - Two single player levels (?)
  - One multiplayer (2 player) level

  - Use autoptr
  - Do basic memory profiling to check for leaks
  
  - Comic patter inside the game (ie, can put speech bubbles on things)
  - Comic screen
  - Game stuff (to be added as I design the game mechanic)


  See ../../README.md for project details.
*/

#include <string>

#include "fmod.hpp"
#include "fmod_errors.h"

#include "hot_config.h"
#include "globals.h"
#include "graphics.h"
#include "game.h"
#include "title.h"
#include "editor.h"
#include "control_setup.h"

// The window
SDL_Window* window;

// The OpenGL context
SDL_GLContext context;

// Music
FMOD_RESULT result;
FMOD::Sound *audio_stream;
FMOD::System *sound_system = NULL;

bool initialize() {

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER) < 0) {
    printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
    return false;
  }

  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  
  graphics->initializeOpenGLVersion();

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

  // Initialize SDL2_ttf for fonts
  if (TTF_Init() < 0) {
    printf("Warning: Unable to initialize SDL2_ttf for fonts: %s\n", TTF_GetError());
    return false;
  }

  // Use Vsync
  if (SDL_GL_SetSwapInterval(1) < 0) {
    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
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

  //  sound_system->createStream("Sound/Bias_Groove.mp3", FMOD_DEFAULT, 0, &audio_stream);

  // Sound disabled for the moment.
  // sound_system->playSound(audio_stream, NULL, false, 0);

  // Enable text input
  SDL_StartTextInput();

  std::string arg2;

  int current_screen;
  if (argc > 1 &&
    (std::string(args[1]) == "title" ||
    std::string(args[1]) == "Title" ||
    std::string(args[1]) == "T")) {
    current_screen = k_title_screen;
  } else if (argc > 1 &&
    (std::string(args[1]) == "controllers" ||
    std::string(args[1]) == "Controllers" ||
    std::string(args[1]) == "C")) {
    current_screen = k_control_setup_screen;
  } else if (argc > 1 &&
    (std::string(args[1]) == "editor" ||
    std::string(args[1]) == "Editor" ||
    std::string(args[1]) == "E")) {
    current_screen = k_editor_screen;
    arg2 = std::string(args[2]);
  } else {
    current_screen = k_1p_game_screen;
  }

  Screen* screen = NULL;

  bool quit = false;
  while (!quit) {
    if (screen == NULL || screen->current_screen != current_screen) {
      if (screen != NULL) {
        current_screen = screen->current_screen;
        screen->shutdown();
      }

      if (current_screen == k_title_screen) {
        screen = new Title();
      } else if (current_screen == k_1p_game_screen) {
        screen = new Game();
      } else if (current_screen == k_editor_screen) {
        screen = new Editor(arg2);
        screen->sound_system = sound_system;
      } else if (current_screen == k_control_setup_screen) {
        screen = new ControlSetup();
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
