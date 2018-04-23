/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "control_setup.h"

// ControlSetup constructor
ControlSetup::ControlSetup() {

  printf("In here now three\n");
  quit = false;
  current_screen = k_control_setup_screen;
  music = "";
  mode = k_control_view_mode;
  printf("In here now four\n");
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
    } else if (e.type == SDL_JOYBUTTONDOWN ||
        e.type == SDL_JOYAXISMOTION ||
        e.type == SDL_JOYHATMOTION ||
        e.type == SDL_JOYBALLMOTION) {
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
  std::string value = control_map->translateKeyEvent(e);
  printf("%s\n", value.c_str());

  if (mode == k_control_view_mode && e.key.keysym.sym == SDLK_RETURN) {
    mode = k_control_config_mode;
    setEmptyTextToMap();
    control_map->new_control_map = {};
    control_iterator = control_map->ordered_controls.begin();
    action_map[*control_iterator]->setText("Press Key");
  } else if (mode == k_control_view_mode && e.key.keysym.sym == SDLK_ESCAPE) {
    current_screen = k_title_screen;
  } else if (mode == k_control_config_mode && e.key.keysym.sym == SDLK_ESCAPE) {
    mode = k_control_view_mode;
    setTextToMap();
  } else if (e.key.keysym.sym == SDLK_p) {
    quit = true;
  } else if (mode == k_control_config_mode && value != "Key Unknown") {
    action_map[*control_iterator]->setText(value);
    control_map->new_control_map[*control_iterator] = value;
    ++control_iterator;
    if (control_iterator == control_map->ordered_controls.end()) {
      control_map->swapMapsAndSave();
      mode = k_control_view_mode;
    } else {
      action_map[*control_iterator]->setText("Press Key");
    }
  } else if (mode == k_control_view_mode) {
    for (auto item = control_map->ordered_controls.begin(); item != control_map->ordered_controls.end(); ++item) {
      if (control_map->control_map[*item] == value) {
        action_map[*item]->setTemporaryColor(237, 28, 36);
      }
    }
  }
}

// Handle controller input
void ControlSetup::handleController(SDL_Event e) {
  std::string value = control_map->translateControllerEvent(e);
  printf("%s\n", value.c_str());

  if (mode == k_control_config_mode && value != "Unknown") {
    action_map[*control_iterator]->setText(value);
    control_map->new_control_map[*control_iterator] = value;
    ++control_iterator;
    if (control_iterator == control_map->ordered_controls.end()) {
      control_map->swapMapsAndSave();
      mode = k_control_view_mode;
    } else {
      action_map[*control_iterator]->setText("Press Key");
    }
  } else if (mode == k_control_view_mode) {
    for (auto item = control_map->ordered_controls.begin(); item != control_map->ordered_controls.end(); ++item) {
      if (control_map->control_map[*item] == value) {
        action_map[*item]->setTemporaryColor(237, 28, 36);
      }
    }
  }
}

// Handle mouse input
void ControlSetup::handleMouse(SDL_Event e) {
}

void ControlSetup::render() {
  graphics->clearScreenWithColor(0.97f, 0.97f, 1.0f, 1.0f);

  graphics->start2DDraw();

  textures->setTexture("control_setup_screen");
  graphics->rectangle(0, 0, k_screen_width, k_screen_height);

  if (mode == k_control_view_mode) {
    view_mode_instruction->render();
    return_to_title_instruction->render();
  } else if (mode == k_control_config_mode) {
    configure_mode_instruction->render();
  }

  player_1_up->render();
  player_1_down->render();
  player_1_left->render();
  player_1_right->render();
  player_1_shoot_accept->render();
  player_1_cancel_switch->render();
  player_1_special->render();
  player_1_view_taunt->render();
  player_2_up->render();
  player_2_down->render();
  player_2_left->render();
  player_2_right->render();
  player_2_shoot_accept->render();
  player_2_cancel_switch->render();
  player_2_special->render();
  player_2_view_taunt->render();

  graphics->end2DDraw();
}

void ControlSetup::setTextToMap() {
  for (auto item = control_map->control_map.begin(); item != control_map->control_map.end(); ++item) {
    printf("%s\n", ((std::string) item->first).c_str());
    printf("%s\n", ((std::string) item->second).c_str());
    action_map[item->first]->setText(item->second);
  }
}

void ControlSetup::setEmptyTextToMap() {
  for (auto item = control_map->control_map.begin(); item != control_map->control_map.end(); ++item) {
    action_map[item->first]->setText(" ");
  }
}

bool ControlSetup::initialize() {
  printf("init d\n");
  graphics->initialize();
  printf("init c\n");

  control_map = new ControlMap();
  printf("init b\n");

  textures->addTexture("control_setup_screen", "control_setup_screen.png");

  printf("init a\n");
  int x_shift = hot_config->getInt("control_map_x");
  int y_shift = hot_config->getInt("control_map_y");

  printf("init 0\n");

  view_mode_instruction = new TextBox(hot_config->getString("title_font"), 32, "Press Return to Configure", 53, 62, 89, 40, 40);
  configure_mode_instruction = new TextBox(hot_config->getString("title_font"), 32, "Press Esc to Cancel", 53, 62, 89, 40, 40);
  return_to_title_instruction = new TextBox(hot_config->getString("title_font"), 32, "Press Esc to Exit to Title", 53, 62, 89, 980, 840);

  printf("init 1\n");
  player_1_up = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 175 + x_shift, 165 + y_shift);
  player_1_down = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 232 + x_shift, 208 + y_shift);
  player_1_left = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 267 + x_shift, 251 + y_shift);
  player_1_right = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 303 + x_shift, 293 + y_shift);
  printf("init 2\n");
  player_1_shoot_accept = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 899 + x_shift, 94 + y_shift);
  player_1_cancel_switch = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 934 + x_shift, 138 + y_shift);
  player_1_special = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 969 + x_shift, 180 + y_shift);
  player_1_view_taunt = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 975 + x_shift, 222 + y_shift);
  printf("init 3\n");
  player_2_up = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 238 + x_shift, 689 + y_shift);
  player_2_down = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 298 + x_shift, 733 + y_shift);
  player_2_left = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 321 + x_shift, 775 + y_shift);
  player_2_right = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 373 + x_shift, 817 + y_shift);
  printf("init 4\n");
  player_2_shoot_accept = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 1136 + x_shift, 350 + y_shift);
  player_2_cancel_switch = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 1177 + x_shift, 394 + y_shift);
  player_2_special = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 1212 + x_shift, 435 + y_shift);
  player_2_view_taunt = new TextBox(hot_config->getString("title_font"), 32, "Key A", 80, 80, 80, 1219 + x_shift, 476 + y_shift);
  printf("init 5\n");

  action_map = {};
  action_map["player_1_up"] = player_1_up;
  action_map["player_1_down"] = player_1_down;
  action_map["player_1_left"] = player_1_left;
  action_map["player_1_right"] = player_1_right;
  action_map["player_1_shoot_accept"] = player_1_shoot_accept;
  action_map["player_1_cancel_switch"] = player_1_cancel_switch;
  action_map["player_1_special"] = player_1_special;
  action_map["player_1_view_taunt"] = player_1_view_taunt;
  action_map["player_2_up"] = player_2_up;
  action_map["player_2_down"] = player_2_down;
  action_map["player_2_left"] = player_2_left;
  action_map["player_2_right"] = player_2_right;
  action_map["player_2_shoot_accept"] = player_2_shoot_accept;
  action_map["player_2_cancel_switch"] = player_2_cancel_switch;
  action_map["player_2_special"] = player_2_special;
  action_map["player_2_view_taunt"] = player_2_view_taunt;

  printf("init 6\n");
  setTextToMap();

  printf("Initializing controllers.\n");

  printf("init 7\n");

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

  printf("init 8\n");

  return true;
}

void ControlSetup::shutdown() {
}

