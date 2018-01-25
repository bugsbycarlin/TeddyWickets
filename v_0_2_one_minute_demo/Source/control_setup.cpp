/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "control_setup.h"

// ControlSetup constructor
ControlSetup::ControlSetup() {
  quit = false;
  current_screen = k_control_setup_screen;
  mode = k_control_view_mode;
}

// ControlSetup loop. Main.cpp is running this on a loop until it's time to switch to a different part of the game.
void ControlSetup::loop(SDL_Window* window, FMOD::System *sound_system) {
  // Event handler
  SDL_Event e;

  // Handle events on queue
  while (SDL_PollEvent(&e) != 0) {
    // User requests quit
    if (e.type == SDL_QUIT) {
      quit = true;
    } else if (e.type == SDL_KEYDOWN) {
      handleKeys(e);
    } else if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN) {
      handleMouse(e);
    } else if (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYAXISMOTION) {
      handleController(e);
    }
  }

  update();

  render();

  // Update sound
  sound_system->update();

  // Update screen
  SDL_GL_SwapWindow(window);
}

void ControlSetup::update() {
}

// Handle keyboard input
void ControlSetup::handleKeys(SDL_Event e) {
  // if (mode == k_title_title_mode) {
  //   if (e.key.keysym.sym == SDLK_DOWN) {
  //     selection += 1;
  //     if (selection > 2) {
  //       selection = 0;
  //     }
  //   } else if (e.key.keysym.sym == SDLK_UP) {
  //     selection -= 1;
  //     if (selection < 0) {
  //       selection = 2;
  //     }
  //   }

  //   if (e.key.keysym.sym == SDLK_RETURN) {
  //     current_screen = k_1p_game_screen;
  //   }
  // }

  if (e.key.keysym.sym == SDLK_p) {
    quit = true;
  }

}

// Handle controller input
void ControlSetup::handleController(SDL_Event e) {
  if (e.type == SDL_JOYBUTTONDOWN) {
    printf("Button %d\n", e.jbutton.button);
  } else if (e.type == SDL_JOYAXISMOTION && (e.jaxis.value < -3200 || e.jaxis.value > 3200)) {
    printf("Axis %d\n", e.jaxis.axis);
  }
}

// Handle mouse input
void ControlSetup::handleMouse(SDL_Event e) {
}

void ControlSetup::render() {
  // Simple viewport.
  glViewport(0, 0, k_screen_width, k_screen_height);

  // Clear color buffer
  glClearColor(0.97f, 0.97f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Start2DDraw();

  // if (mode == k_title_presents_mode) {
  //   //glEnable(GL_LIGHTING);
  //   //glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
  //   bugsby_presents->render();
  //   fadeIn(0.0f, 2.0f, ((last_time - start_time) / 1000.0f));
  //   fadeOut(3.0f, 4.0f, ((last_time - start_time) / 1000.0f) - 3.0f);
  // } else if (mode == k_title_title_mode) {
  //   // ControlSetup screen render
  //   textures->setTexture("title_screen");
  //   glBegin(GL_QUADS);
  //   glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
  //   glTexCoord2d(0.0, 1.0); glVertex2d(0.0, k_screen_height);
  //   glTexCoord2d(1.0, 1.0); glVertex2d(k_screen_width, k_screen_height);
  //   glTexCoord2d(1.0, 0.0); glVertex2d(k_screen_width, 0.0);
  //   glEnd();

  //   // ControlSetup text render
  //   title->render();

  //   if (selection == 0) {
  //     one_player_selected->render();
  //   } else {
  //     one_player_unselected->render();
  //   }

  //   if (selection == 1) {
  //     two_player_selected->render();
  //   } else {
  //     two_player_unselected->render();
  //   }

  //   if (selection == 2) {
  //     control_setup_selected->render();
  //   } else {
  //     control_setup_unselected->render();
  //   }

  //   fadeIn(0.0f, 1.0f, ((last_time - start_time) / 1000.0f) - 4.0f);
  // }

  End2DDraw();
}

bool ControlSetup::initialize() {
  glEnable(GL_TEXTURE_2D);

  textures = new Textures();

  //textures->addTexture("title_screen", "title_screen_draft_5.png");

  // glFrontFace(GL_CW);
  // glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing textures! %s\n", gluErrorString(error));
    return false;
  }

  printf("Initializing controllers.\n");

  SDL_GameController *controller = NULL;
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
    }
  }

  return true;
}

void ControlSetup::shutdown() {
  delete textures;
}

