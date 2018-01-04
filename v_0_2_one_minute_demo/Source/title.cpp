/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "title.h"

// Title constructor
Title::Title() {
  quit = false;
  selection = 0;
  current_screen = k_title_screen;
}

// Title loop. Main.cpp is running this on a loop until it's time to switch to a different part of the game.
void Title::loop(SDL_Window* window, FMOD::System *sound_system) {
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
    }
  }

  update();

  render();

  // Update sound
  sound_system->update();

  // Update screen
  SDL_GL_SwapWindow(window);
}

void Title::update() {
  // Set time and perform physics update
  unsigned long current_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  last_time = current_time;
}

// Handle keyboard input
void Title::handleKeys(SDL_Event e) {
  if (e.key.keysym.sym==SDLK_DOWN || e.key.keysym.sym==SDLK_UP) {
    if (selection != 1) {
      selection = 1;
    } else {
      selection = 2;
    }
  }

  if (e.key.keysym.sym==SDLK_p) {
    quit = true;
  }

  if (e.key.keysym.sym==SDLK_RETURN) {
    current_screen = k_1p_game_screen;
  }
}

// Handle mouse input
void Title::handleMouse(SDL_Event e) {
}

void Title::render() {
  // Simple viewport.
  glViewport(0, 0, k_screen_width, k_screen_height);

  // Clear color buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Start2DDraw();

  // title screen render
  textures->setTexture("title_screen");
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
  glTexCoord2d(0.0, 1.0); glVertex2d(0.0, k_screen_height);
  glTexCoord2d(1.0, 1.0); glVertex2d(k_screen_width, k_screen_height);
  glTexCoord2d(1.0, 0.0); glVertex2d(k_screen_width, 0.0);
  glEnd();

  if (selection == 1) {
    textures->setTexture("one_player_selected");
  } else {
    textures->setTexture("one_player_unselected");
  }
  int one_p_x = 530;
  int one_p_y = 500;
  int one_p_width = 420;
  int one_p_height = 100;
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(one_p_x + 0, one_p_y + 0);
  glTexCoord2d(0.0, 1.0); glVertex2d(one_p_x + 0, one_p_y + one_p_height);
  glTexCoord2d(1.0, 1.0); glVertex2d(one_p_x + one_p_width, one_p_y + one_p_height);
  glTexCoord2d(1.0, 0.0); glVertex2d(one_p_x + one_p_width, one_p_y + 0);
  glEnd();

  if (selection == 2) {
    textures->setTexture("two_player_selected");
  } else {
    textures->setTexture("two_player_unselected");
  }
  int two_p_x = 530;
  int two_p_y = 650;
  int two_p_width = 420;
  int two_p_height = 100;
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(two_p_x + 0, two_p_y + 0);
  glTexCoord2d(0.0, 1.0); glVertex2d(two_p_x + 0, two_p_y + two_p_height);
  glTexCoord2d(1.0, 1.0); glVertex2d(two_p_x + two_p_width, two_p_y + two_p_height);
  glTexCoord2d(1.0, 0.0); glVertex2d(two_p_x + two_p_width, two_p_y + 0);
  glEnd();

  End2DDraw();
}

bool Title::initialize() {
  glEnable(GL_TEXTURE_2D);

  textures = new Textures();

  textures->addTexture("title_screen", "title_screen_draft.png");

  textures->addTexture("one_player_unselected", "one_player_unselected.png");
  textures->addTexture("one_player_selected", "one_player_selected.png");
  textures->addTexture("two_player_unselected", "two_player_unselected.png");
  textures->addTexture("two_player_selected", "two_player_selected.png");

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

  start_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  last_time = start_time;

  return true;
}

void Title::shutdown() {
  // To do: release the image files.
  delete textures;
}

