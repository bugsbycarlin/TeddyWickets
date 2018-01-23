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
  if (e.key.keysym.sym == SDLK_DOWN) {
    selection += 1;
    if (selection > 2) {
      selection = 0;
    }
  } else if (e.key.keysym.sym == SDLK_UP) {
    selection -= 1;
    if (selection < 0) {
      selection = 2;
    }
  }

  if (e.key.keysym.sym == SDLK_p) {
    quit = true;
  }

  if (e.key.keysym.sym == SDLK_RETURN) {
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

  glColor3f(1.0f, 1.0f, 1.0f);

  Start2DDraw();

  // title screen render
  textures->setTexture("title_screen");
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
  glTexCoord2d(0.0, 1.0); glVertex2d(0.0, k_screen_height);
  glTexCoord2d(1.0, 1.0); glVertex2d(k_screen_width, k_screen_height);
  glTexCoord2d(1.0, 0.0); glVertex2d(k_screen_width, 0.0);
  glEnd();

  // title text render
  title->render();

  if (selection == 0) {
    one_player_selected->render();
  } else {
    one_player_unselected->render();
  }

  if (selection == 1) {
    two_player_selected->render();
  } else {
    two_player_unselected->render();
  }

  if (selection == 2) {
    control_setup_selected->render();
  } else {
    control_setup_unselected->render();
  }

  End2DDraw();
}

bool Title::initialize() {
  glEnable(GL_TEXTURE_2D);

  textures = new Textures();

  textures->addTexture("title_screen", "title_screen_draft_3.png");

  title = new TextBox("cartoon_blocks.ttf", 180, "Teddy Wickets", 135, 206, 235, 200, 100);

  one_player_selected = new TextBox("cartoon_blocks.ttf", 90, "1P Tutorial", 135, 206, 235, 800, 450);
  one_player_unselected = new TextBox("cartoon_blocks.ttf", 90, "1P Tutorial", 206, 206, 206, 800, 450);
  two_player_selected = new TextBox("cartoon_blocks.ttf", 90, "2P Game", 135, 206, 235, 800, 550);
  two_player_unselected = new TextBox("cartoon_blocks.ttf", 90, "2P Game", 206, 206, 206, 800, 550);
  control_setup_selected = new TextBox("cartoon_blocks.ttf", 90, "Controls", 135, 206, 235, 800, 650);
  control_setup_unselected = new TextBox("cartoon_blocks.ttf", 90, "Controls", 206, 206, 206, 800, 650);

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

  title->shutdown();

  one_player_selected->shutdown();
  one_player_unselected->shutdown();
  two_player_selected->shutdown();
  two_player_unselected->shutdown();
  control_setup_selected->shutdown();
  control_setup_unselected->shutdown();
}

