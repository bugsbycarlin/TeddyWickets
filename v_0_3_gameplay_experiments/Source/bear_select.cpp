/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "bear_select.h"


// Editor constructor
BearSelect::BearSelect() {
  current_screen = k_bear_select_screen;
  game_mode = k_bear_select_mode;

  music = "";

  available_bear_choices = {
    {0, 1},
    {1, 1},
    {2, 1},
    {3, 1},
    {4, 1},
    {5, 1},
    {6, 1},
    {7, 1},
    {8, 1}
  };

  player_1_bears = {};
  player_2_bears = {};
  bear_choice = 0;

  // Player 1 goes first. Later I'll change this to random
  first = 0;
}

// Editor loop. Main.cpp is running this on a loop until it's time to switch to a different part of the game.
void BearSelect::loop(SDL_Window* window, FMOD::System *sound_system) {
  // Event handler
  SDL_Event e;

  // Handle events on queue
  while (SDL_PollEvent(&e) != 0) {
    // User requests quit
    if (e.type == SDL_QUIT) {
      quit = true;
    } else if (e.type == SDL_KEYDOWN) {
      handleKeys(e);
    } else if (e.type == SDL_JOYBUTTONDOWN ||
        e.type == SDL_JOYAXISMOTION ||
        e.type == SDL_JOYHATMOTION ||
        e.type == SDL_JOYBALLMOTION) {
      handleController(e);
    }
  }

  update();

  render();

  // Update screen
  SDL_GL_SwapWindow(window);
}

void BearSelect::update() {
  unsigned long current_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  last_time = current_time;

  if (current_time - framerate_time > 1000.0f) {
    printf("Framerate: %d\n", frames_since_last);
    framerate_time = current_time;
    frames_since_last = 0;
  } else {
    frames_since_last++;
  }

  if (game_mode == k_lets_go_mode) {
    if (current_time - mark_time > 1500.0f) {
      current_screen = k_2p_game_screen;
    } else {
      // go_text->setColor(rand() % static_cast<int>(250 + 1),
      //   rand() % static_cast<int>(250 + 1),
      //   rand() % static_cast<int>(250 + 1));
    }
  }
}

// Handle keyboard input
void BearSelect::handleKeys(SDL_Event e) {

  if (e.key.keysym.sym == SDLK_ESCAPE) {
    quit = true;
  }

  std::string value = control_map->translateKeyEvent(e);

  for (auto item = control_map->ordered_controls.begin(); item != control_map->ordered_controls.end(); ++item) {
    if (control_map->control_map[*item] == value) {
      handleAction(*item);
    }
  }
}

// Handle controller input
void BearSelect::handleController(SDL_Event e) {
  std::string value = control_map->translateControllerEvent(e);

  for (auto item = control_map->ordered_controls.begin(); item != control_map->ordered_controls.end(); ++item) {
    if (control_map->control_map[*item] == value) {
      handleAction(*item);
    }
  }
}


// Handle actions as translated from the control map
void BearSelect::handleAction(std::string action) {
  if (game_mode == k_lets_go_mode) {
    return;
  }

  // Whose turn?
  // Player 1
  if (player_1_bears.size() + first == player_2_bears.size()) {
    if (action == "player_1_down") {
      bear_choice += 3;
      if (bear_choice > 8) bear_choice -= 9;
    } else if (action == "player_1_up") {
      bear_choice -= 3;
      if (bear_choice < 0) bear_choice += 9;
    } else if (action == "player_1_left") {
      bear_choice -= 1;
      if ((bear_choice + 3) % 3 == 2) bear_choice += 3;
    } else if (action == "player_1_right") {
      bear_choice += 1;
      if (bear_choice % 3 == 0) bear_choice -= 3;
    } else if (action == "player_1_shoot_accept") {
      if (available_bear_choices[bear_choice] == 1) {
        available_bear_choices[bear_choice]--;
        player_1_bears.push_back(bear_choices[bear_choice]);
        player_1_choose_text->setColor(53, 62, 89);
        player_2_choose_text->setColor(140, 98, 57);
      }
    }
  } else { // Player 2
    if (action == "player_2_down") {
      bear_choice += 3;
      if (bear_choice > 8) bear_choice -= 9;
    } else if (action == "player_2_up") {
      bear_choice -= 3;
      if (bear_choice < 0) bear_choice += 9;
    } else if (action == "player_2_left") {
      bear_choice -= 1;
      if ((bear_choice + 3) % 3 == 2) bear_choice += 3;
    } else if (action == "player_2_right") {
      bear_choice += 1;
      if (bear_choice % 3 == 0) bear_choice -= 3;
    } else if (action == "player_2_shoot_accept") {
      if (available_bear_choices[bear_choice] == 1) {
        available_bear_choices[bear_choice]--;
        player_2_bears.push_back(bear_choices[bear_choice]);
        player_1_choose_text->setColor(140, 98, 57);
        player_2_choose_text->setColor(53, 62, 89);
      }
    }
  }

  if (player_1_bears.size() == 3 && player_2_bears.size() == 3) {
    // to do: this should be starting animation instead
    game_mode = k_lets_go_mode;
    mark_time = last_time;
    player_1_choose_text->setColor(53, 62, 89);
    player_2_choose_text->setColor(53, 62, 89);
  }

  bear_name_text->setText(bear_pretty_names[bear_choices[bear_choice]]);
  bear_description_text->setText(bear_descriptions[bear_choices[bear_choice]]);
}

void BearSelect::render() {
  // Clear color buffer
  graphics->clearScreenWithColor(1.0f, 1.0f, 1.0f, 1.0f);

  graphics->start2DDraw();

  textures->setTexture("bear_select_background");
  graphics->rectangle(0, 0, k_screen_width, k_screen_height);

  player_1_choose_text->render();
  player_2_choose_text->render();

  if (game_mode == k_bear_select_mode) {
    choose_your_bears_text->render();
    bear_name_text->render();
    bear_description_text->render();

    textures->setTexture("selection_2");
    float m = k_bear_choice_x + k_bear_choice_margin * (bear_choice % 3);
    float n = k_bear_choice_y + k_bear_choice_margin * (bear_choice / 3);
    graphics->rectangle(m, n, k_selection_box_size, k_selection_box_size);

    for (int i = 0; i < bear_choices.size(); i++) {
      textures->setTexture(bear_choices[i] + "_box");
      float m = k_bear_choice_x + k_bear_choice_margin * (i % 3);
      float n = k_bear_choice_y + k_bear_choice_margin * (i / 3);
      graphics->rectangle(m, n, k_selection_box_size, k_selection_box_size);

      if (available_bear_choices[i] != 1) {
        textures->setTexture("unavailable_bear_selection_box");
        graphics->rectangle(m, n, k_selection_box_size, k_selection_box_size);
      }
    }

    textures->setTexture("bear_selection_box");
    m = k_bear_choice_x + k_bear_choice_margin * (bear_choice % 3);
    n = k_bear_choice_y + k_bear_choice_margin * (bear_choice / 3);
    graphics->rectangle(m, n, k_selection_box_size, k_selection_box_size);
  } else if (game_mode == k_lets_go_mode) {
    int max = (int) ((last_time - mark_time - 100.0f) / 100.0f);
    if (max > 9) max = 9;
    // if (last_time - mark_time > 200.0f) {
    //   go_text_1->render();
    // }
    // if (last_time - mark_time > 300.0f) {
    //   go_text_2->render();
    // }
    // if (last_time - mark_time > 400.0f) {
    //   go_text_3->render();
    // }
    for (int i = 0; i < max; i++) {
      int num = i * 7 % 9;
      go_text_1->x = k_bear_choice_x + k_bear_choice_margin * (num % 3);
      go_text_1->y = k_bear_choice_y + k_bear_choice_margin * (num / 3);
      go_text_1->render();
    }
  }

  for (int j = 0; j < 3; j++) {
    if (player_1_bears.size() < j + 1) {
      textures->setTexture("unfilled_bear_selection_box");
    } else {
      textures->setTexture(player_1_bears[j] + "_box");
    }
    float n = k_bear_choice_y + k_bear_choice_margin * j;
    graphics->rectangle(k_player_1_choices_x, n, k_selection_box_size, k_selection_box_size);
  }

  for (int j = 0; j < 3; j++) {
    if (player_2_bears.size() < j + 1) {
      textures->setTexture("unfilled_bear_selection_box");
    } else {
      textures->setTexture(player_2_bears[j] + "_box");
    }
    float n = k_bear_choice_y + k_bear_choice_margin * j;
    graphics->rectangle(k_player_2_choices_x, n, k_selection_box_size, k_selection_box_size);
  }

  if (game_mode == k_lets_go_mode) {
    graphics->fadeOut(0.5f, 1.5f, ((last_time - mark_time) / 1000.0f) - 0.5f);
  }

  graphics->end2DDraw();
}


bool BearSelect::initialize() {
  graphics->initialize();

  control_map = new ControlMap();

  // Initialize Textures
  if (!initializeTextures()) {
    printf("Unable to initialize textures!\n");
    return false;
  }

  start_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  last_time = start_time;
  framerate_time = start_time;

  return true;
}

bool BearSelect::initializeTextures() {

  textures->addTexture("bear_select_background", "bear_select_background.png");

  textures->addTexture("selection_2", "selection_2.png");

  textures->addTexture("unfilled_bear_selection_box", "unfilled_bear_selection_box.png");
  textures->addTexture("unavailable_bear_selection_box", "unavailable_bear_selection_box.png");
  textures->addTexture("unknown_bear_selection_box", "unknown_bear_selection_box.png");
  textures->addTexture("bear_selection_box", "bear_selection_box.png");

  textures->addTexture("lil_jon_box", "lil_jon_box.png");
  textures->addTexture("mortimer_box", "mortimer_box.png");
  textures->addTexture("gluke_box", "gluke_box.png");
  textures->addTexture("mags_box", "mags_box.png");
  textures->addTexture("bob_smith_box", "bob_smith_box.png");
  textures->addTexture("lord_lonsdale_box", "lord_lonsdale_box.png");
  textures->addTexture("hpf_swinnerton_dyer_box", "hpf_swinnerton_dyer_box.png");
  textures->addTexture("jeff_bridges_box", "jeff_bridges_box.png");
  textures->addTexture("grim_box", "grim_box.png");

  Point* selection_box_color = colors->color("selection_box");
  Point* dark_box_color = colors->darker(selection_box_color);
  printf("Darker color: %d, %d, %d\n", (int) dark_box_color->x, (int) dark_box_color->y, (int) dark_box_color->z);

  choose_your_bears_text = new TextBox("candy_crayon.ttf", 60,
    "Choose your bears", 53, 62, 89, 44, 22);
  player_1_choose_text = new TextBox("candy_crayon.ttf", 60,
    "Player 1", 140, 98, 57, 44, 128);
  player_2_choose_text = new TextBox("candy_crayon.ttf", 60,
    "Player 2", 53, 62, 89, 1147, 128);
  bear_name_text = new TextBox("candy_crayon.ttf", 50,
    bear_pretty_names[bear_choices[bear_choice]], 53, 62, 89, 362, 93);
  bear_description_text = new TextBox("candy_crayon.ttf", 40,
    bear_descriptions[bear_choices[bear_choice]], 53, 62, 89, 362, 149);
   
  go_text_1 = new TextBox("candy_crayon.ttf", 160,
  "Go", 0, 0, 0, k_bear_choice_x, k_bear_choice_y);

  // go_text_2 = new TextBox("candy_crayon.ttf", 160,
  // "Go", 0, 0, 0, k_bear_choice_x + k_bear_choice_margin, k_bear_choice_y + k_bear_choice_margin);

  // go_text_3 = new TextBox("candy_crayon.ttf", 160,
  // "Go!", 0, 0, 0, k_bear_choice_x + 2 * k_bear_choice_margin, k_bear_choice_y + 2 * k_bear_choice_margin);

  return true;
}

void BearSelect::shutdown() {
}
