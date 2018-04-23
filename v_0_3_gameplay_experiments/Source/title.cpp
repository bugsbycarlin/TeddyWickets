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
  music = "Sound/Nothing_To_Fear.mp3";
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
      if (selection > 1) {
        selection = 0;
      }
    } else if (e.key.keysym.sym == SDLK_UP) {
      selection -= 1;
      if (selection < 0) {
        selection = 1;
      }
    }

    if (e.key.keysym.sym == SDLK_RETURN) {
      // if (selection == 0) {
      //   current_screen = k_1p_game_screen;
      // } else if (selection == 1) {
      //   current_screen = k_bear_select_screen;
      // } else if (selection == 2) {
      //   current_screen = k_control_setup_screen;
      // }
      if (selection == 0) {
        current_screen = k_bear_select_screen;
      } else if (selection == 1) {
        current_screen = k_control_setup_screen;
      }
    }
  }

  if (e.key.keysym.sym == SDLK_ESCAPE) {
    quit = true;
  }
}

void Title::render() {
  graphics->clearScreenWithColor(0.0f, 0.0f, 0.0f, 1.0f);

  graphics->start2DDraw();

  if (mode == k_title_presents_mode) {
    friends_on_mountains_presents->render();
    graphics->fadeIn(0.0f, 2.0f, ((last_time - start_time) / 1000.0f));
    graphics->fadeOut(3.0f, 4.0f, ((last_time - start_time) / 1000.0f) - 3.0f);
  } else if (mode == k_title_title_mode) {
    // title screen render
    textures->setTexture("title_screen");
    graphics->rectangle(0, 0, k_screen_width, k_screen_height);

    // title text render
    title->render();

    // if (selection == 0) {
    //   one_player_selected->render();
    // } else {
    //   one_player_unselected->render();
    // }

    if (selection == 0) {
      two_player_selected->render();
    } else {
      two_player_unselected->render();
    }

    if (selection == 1) {
      control_setup_selected->render();
    } else {
      control_setup_unselected->render();
    }

    graphics->fadeIn(0.0f, 1.0f, ((last_time - start_time) / 1000.0f) - 4.0f);
  }

  graphics->end2DDraw();
}

bool Title::initialize() {
  graphics->initialize();

  textures->addTexture("title_screen", "title_screen_draft_7.png");

  friends_on_mountains_presents = new TextBox(hot_config->getString("title_font"), 70, "Friends on Mountains Presents", 255, 255, 255, 540, 790);

  title = new TextBox(hot_config->getString("title_font"), 180, "Teddy Wickets", 0, 0, 0, 220, 100);

  // one_player_selected = new TextBox(hot_config->getString("title_font"), 90, "1P Tutorial", 0, 0, 0, 720, 510);
  // one_player_unselected = new TextBox(hot_config->getString("title_font"), 90, "1P Tutorial", 160, 160, 160, 720, 510);
  two_player_selected = new TextBox(hot_config->getString("title_font"), 90, "2P Game", 0, 0, 0, 720, 560);
  two_player_unselected = new TextBox(hot_config->getString("title_font"), 90, "2P Game", 160, 160, 160, 720, 560);
  control_setup_selected = new TextBox(hot_config->getString("title_font"), 90, "Controls", 0, 0, 0, 720, 660);
  control_setup_unselected = new TextBox(hot_config->getString("title_font"), 90, "Controls", 160, 160, 160, 720, 660);

  start_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  last_time = start_time;

  return true;
}

void Title::shutdown() {
  title->shutdown();

  // one_player_selected->shutdown();
  // one_player_unselected->shutdown();
  two_player_selected->shutdown();
  two_player_unselected->shutdown();
  control_setup_selected->shutdown();
  control_setup_unselected->shutdown();
}

