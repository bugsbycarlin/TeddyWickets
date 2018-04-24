/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Currently working on V0.3 - Gameplay Experiments

  This version is for refining the rules and mechanics of the game until it is fun.

  Changes:
  +- Add free stars
  +- Fix starting point issue
  +- Filigree on the level sides
  +- Reconfigurable number of bears (and test it on a level)
  +- Free looking around
  +- Slightly better indication of which bear is selected
  +- Control glyphs
  +- Comic patter comic bubbles (taunts and stuff)
  +- Fix font in early screens
  +- put purple tile on select bears screen
  +- Fix free star height detection issue
  +- remove apostrophies from text
  +- Fix bear selection bug (it wasn't going from bear select screen to game screen)
  +- fix physics bumper / rotation issue
  +- Finish blueblue_slopes_2 level

  Some remainder goals from the last phase:
  - Finish blueblue_slopes_1 level
  - Even better indication of which bear is selected (hover on the sides)

  STOP THERE!
  - Refine shot power to fit levels better
  - Boxguys go by facing (true rotation) not just x vs y
  - Make last wicket optional
  - more squish and bounce!
  
  - Topspin and backspin 
  - Tutorial comic bubbles
  
  - Even better indication of which bear is selected


  Tech debt for this phase:
  - Lint for style
  - Refactor a little to make sure things are happening in the right place
  - Make bounce and squish into general things
  - Make scene transitions into general things
  - Make loading screen
  - Use autoptr
  - Do basic memory profiling to check for leaks
  
  
  Saving for later:
  - Refined cel shading
  - cel shade outlining
  - improve color palette even more
  - Splash effects
  - Fix water

  - Comic screen

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


  // TO DO: factor out and move somewhere nice. Control Map maybe.
  printf("Initializing controllers.\n");

  SDL_GameController *controller = NULL;
  SDL_Joystick *joy = NULL;
  printf("This many joysticks %d\n", SDL_NumJoysticks());
  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    printf("Counting %d\n", i);
    if (SDL_IsGameController(i)) {
      controller = SDL_GameControllerOpen(i);
      printf("There is a controller named %s\n", SDL_JoystickNameForIndex(i));
      if (controller) {
        printf("Opened controller %d\n", i);
      } else {
        printf("Could not open gamecontroller %i: %s\n", i, SDL_GetError());
      }
    } else {
      joy = SDL_JoystickOpen(i);
      printf("There is a joystick named %s\n", SDL_JoystickNameForIndex(i));
      if (joy) {
        printf("Opened joystick %d\n", i);
      } else {
        printf("Could not open joystick %i: %s\n", i, SDL_GetError());
      }
    }
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
  player_2_bears = {"hpf_swinnerton_dyer", "bob_smith", "lord_lonsdale"};

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

    // if (loop_counter % 30 == 0 && hot_loads_counter + 1 < hot_loads.size()) {
    //   if (hot_loads[hot_loads_counter] == "model") {
    //     Model* model = model_cache->getModel(hot_loads[hot_loads_counter + 1]);
    //     //model->render();
    //   } else if (hot_loads[hot_loads_counter] == "texture") {
    //     textures->addTexture(hot_loads[hot_loads_counter + 1], hot_loads[hot_loads_counter + 1] + ".png");
    //   }

    //   hot_loads_counter += 2;
    // }

    if (last_screen == k_bear_select_screen && screen->current_screen == k_2p_game_screen) {
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
