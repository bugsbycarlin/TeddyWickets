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
  mode = k_title_presents_mode;
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

  if (current_time - start_time > 4000.0f) {
    mode = k_title_title_mode;
  }
}

// Handle keyboard input
void Title::handleKeys(SDL_Event e) {
  if (mode == k_title_title_mode) {
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

    if (e.key.keysym.sym == SDLK_RETURN) {
      if (selection == 0) {
        current_screen = k_1p_game_screen;
      } else if (selection == 1) {
        current_screen = k_1p_game_screen;
      } else if (selection == 2) {
        current_screen = k_control_setup_screen;
      }
    }
  }

  if (e.key.keysym.sym == SDLK_p) {
    quit = true;
  }
}

// Handle mouse input
void Title::handleMouse(SDL_Event e) {
}

void Title::render() {
  graphics->clearScreenWithColor(0.0f, 0.0f, 0.0f, 1.0f);

  graphics->start2DDraw();

  if (mode == k_title_presents_mode) {
    bugsby_presents->render();
    graphics->fadeIn(0.0f, 2.0f, ((last_time - start_time) / 1000.0f));
    graphics->fadeOut(3.0f, 4.0f, ((last_time - start_time) / 1000.0f) - 3.0f);
  } else if (mode == k_title_title_mode) {
    // title screen render
    textures->setTexture("title_screen");
    graphics->drawRectangle(0, 0, k_screen_width, k_screen_height);

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

    graphics->fadeIn(0.0f, 1.0f, ((last_time - start_time) / 1000.0f) - 4.0f);
  }

  graphics->end2DDraw();
}

bool Title::initialize() {
  graphics->initializeBasic();

  textures = new Textures();

  textures->addTexture("title_screen", "title_screen_draft_5.png");

  bugsby_presents = new TextBox("cartoon_blocks.ttf", 90, "Bugsby Presents", 255, 255, 255, 780, 770);

  title = new TextBox("cartoon_blocks.ttf", 180, "Teddy Wickets", 53, 62, 89, 200, 100);

  one_player_selected = new TextBox("cartoon_blocks.ttf", 90, "1P Tutorial", 53, 62, 89, 800, 450);
  one_player_unselected = new TextBox("cartoon_blocks.ttf", 90, "1P Tutorial", 206, 206, 206, 800, 450);
  two_player_selected = new TextBox("cartoon_blocks.ttf", 90, "2P Game", 53, 62, 89, 800, 550);
  two_player_unselected = new TextBox("cartoon_blocks.ttf", 90, "2P Game", 206, 206, 206, 800, 550);
  control_setup_selected = new TextBox("cartoon_blocks.ttf", 90, "Controls", 53, 62, 89, 800, 650);
  control_setup_unselected = new TextBox("cartoon_blocks.ttf", 90, "Controls", 206, 206, 206, 800, 650);

  start_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  last_time = start_time;

  return true;
}

void Title::shutdown() {
  delete textures;

  title->shutdown();

  one_player_selected->shutdown();
  one_player_unselected->shutdown();
  two_player_selected->shutdown();
  two_player_unselected->shutdown();
  control_setup_selected->shutdown();
  control_setup_unselected->shutdown();
}

