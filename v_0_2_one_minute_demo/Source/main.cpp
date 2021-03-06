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
  +- HotConfig
  +- Remove mouse support
  +- Music loops, multiple music tracks
  +- Player base colors
  +- Show scores
  +- Fix the 2d rotation problem
  +- Clean HUD first draft
  +- Fix the major graphics memory leak problem (by translate/rotate)
  +- Wicket crossing detection and action
  +- Redo physics/state transitions
  +- Bear to bear collisions (should not screw up with state transitions; it should not reset the bear to drop mode)
  +- Front Bumpers
  +- Ramp colors
  +- Start spot color
  +- Shot predictions
  +- Move the baddy
  +- Take out velocity lines
  +- Final wicket and simple celebration


  Demo day remainder:
  - Grey out bears that are not the current bear somehow (or indicate the selected bear more)
  - Make sure shot powers and level size work together
  - Control glyphs
  - Filigree on the level sides
  - more squish and bounce!
  - Topspin and backspin 
  - Tutorial comic bubbles
  - Fix alternating starts issue (see test level 2)


  

  - Refined cel shading
  - cel shade outlining
  
  - improve color palette even more
  - Splash effects
  - Fix water

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
#include <list>

#include "fmod.hpp"
#include "fmod_errors.h"

#include "hot_config.h"
#include "globals.h"
#include "graphics.h"
#include "title.h"
#include "bear_select.h"
#include "game.h"
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

std::string music = "";

static std::vector<std::string> hot_loads;
int hot_loads_counter = 0;
int loop_counter = 0;

bool initialize() {

  hot_loads = {
    // "model", "teddy_2.obj",
    // "model", "tile_flat.obj",
    // "model", "wicket.obj",
    // "model", "player_1_start.obj",
    // "model", "teddy_bear_draft_3.obj",
    // "model", "arrow.obj",
    // "model", "player_2_start.obj",
    // "texture", "coordinates",  
    // "texture", "clouds",
    // "texture", "water",
    // "texture", "player_1_HUD_background",
    // "texture", "player_2_HUD_background",
    // "texture", "large_salmon_star",
    // "texture", "large_purple_star",
    // "texture", "bear_selection_box",
    // "texture", "lil_jon_box",
    // "texture", "mortimer_box",
    // "texture", "gluke_box",
    // "texture", "mags_box",
    // "texture", "bob_smith_box",
    // "texture", "lord_lonsdale_box",
    // "texture", "hpf_swinnerton_dyer_box",
    // "texture", "jeff_bridges_box",
    // "texture", "grim_box",
    // "texture", "flat_shot_glyph",
    // "texture", "up_shot_glyph",
    // "texture", "mallet_glyph",
    // "texture", "mallet_background_glyph",
    // "texture", "special_power",
    // "texture", "taunt",
  };

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

  // Enable text input
  SDL_StartTextInput();

  std::string arg2;
  std::vector<std::string> player_1_bears;
  std::vector<std::string> player_2_bears;
  int last_screen;
  int current_screen;

  //defaults
  player_1_bears = {"lil_jon", "mortimer", "gluke"};
  player_2_bears = {"mags", "bob_smith", "lord_lonsdale"};

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
  } else if (argc > 1 &&
    (std::string(args[1]) == "game" ||
    std::string(args[1]) == "Game" ||
    std::string(args[1]) == "G")) {
    current_screen = k_2p_game_screen;
  } else {
    //current_screen = k_2p_game_screen;
    current_screen = k_title_screen;
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
      } else if (current_screen == k_bear_select_screen) {
        screen = new BearSelect();
      } else if (current_screen == k_2p_game_screen) {
        screen = new Game(player_1_bears, player_2_bears);
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

    last_screen = current_screen;

    if (hot_config->getInt("use_music") == 1) {
      if (screen->music != "" && screen->music != music) {
        if (audio_stream != NULL) {
          sound_system->close();
          sound_system->init(512, FMOD_INIT_NORMAL, 0);
        }
        music = screen->music;
        sound_system->createStream(music.c_str(), FMOD_LOOP_NORMAL, 0, &audio_stream);
        sound_system->playSound(audio_stream, NULL, false, 0);
      }
    }

    loop_counter += 1;

    screen->loop(window, sound_system);

    if (loop_counter % 30 == 0 && hot_loads_counter + 1 < hot_loads.size()) {
      if (hot_loads[hot_loads_counter] == "model") {
        Model* model = model_cache->getModel(hot_loads[hot_loads_counter + 1]);
        //model->render();
      } else if (hot_loads[hot_loads_counter] == "texture") {
        textures->addTexture(hot_loads[hot_loads_counter + 1], hot_loads[hot_loads_counter + 1] + ".png");
      }

      hot_loads_counter += 2;
    }

    if (last_screen == k_bear_select_screen && current_screen == k_2p_game_screen) {
      player_1_bears = ((BearSelect*) screen)->player_1_bears;
      player_2_bears = ((BearSelect*) screen)->player_2_bears;
    }

    if (screen != NULL && screen->quit == true) {
      screen->shutdown();
      quit = true;
    }
  }

  // Disable text input
  SDL_StopTextInput();

  shutdown();
}
