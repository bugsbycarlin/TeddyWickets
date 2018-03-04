/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "game.h"

// Game constructor
Game::Game() {
  quit = false;
  mousedown = false;
  zoom = k_default_zoom;
  drag_x = 0;
  drag_y = 0;
  default_speed_ramping = k_default_speed_ramping;
  simulation_speed = k_default_minimum_speed;
  current_screen = k_1p_game_screen;

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

  bool choose = false;

  if (choose) {
    // choose bears
    game_mode = k_bear_select_mode;
    player_1_bears = {};
    player_2_bears = {};
    bear_choice = 0;
  } else {
    // or default while programming
    game_mode = k_drop_mode;
    bear_choice = 0;
    player_1_bears = {"lil_jon", "mortimer", "gluke"};
    player_2_bears = {"mags", "bob_smith", "lord_lonsdale"};
  }
}

// Game loop. Main.cpp is running this on a loop until it's time to switch to a different part of the game.
void Game::loop(SDL_Window* window, FMOD::System *sound_system) {
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

  afterUpdate();
}

void Game::update() {
  // Set time and perform physics update
  unsigned long current_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  physics->update(simulation_speed * (current_time - last_time) / 1000.0f);
  last_time = current_time;
  if (simulation_speed > k_default_minimum_speed) {
    simulation_speed *= 0.98f;
  }

  if (current_time - framerate_time > 1000.0f) {
    printf("Framerate: %d\n", frames_since_last);
    printf("Ball: %0.2f, %0.2f, %0.2f\n", character->position->x, character->position->y, character->position->z);
    framerate_time = current_time;
    frames_since_last = 0;
  } else {
    frames_since_last++;
  }

  if (game_mode == k_lets_go_mode) {
    if (current_time - mark_time > 2000.0f) {
      game_mode = k_drop_mode;
    } else {
      go_text->setColor(rand() % static_cast<int>(250 + 1),
        rand() % static_cast<int>(250 + 1),
        rand() % static_cast<int>(250 + 1));
    }
  }

  if (game_mode == k_bear_select_mode) {
    return;
  }

  // Update bumpers to see if they need lighting up
  for (auto bumper = bumpers.begin(); bumper != bumpers.end(); ++bumper) {
    (*bumper)->update();
    if (physics->hasCollision((*bumper)->identity, character->identity)) {
      (*bumper)->bump();
    }
  }

  // Check whether action has finished, and if so, switch to drop mode
  if (!physics->checkActive(character->identity) && game_mode == k_action_mode) {
    game_mode = k_drop_mode;
    physics->stop(character->identity);
    character->shot_rotation = character->default_shot_rotation;
    physics->setPositionAndRotation(character->identity,
      new Point(character->position->x, character->position->y, character->position->z + k_character_drop_height),
      0, 0, character->default_shot_rotation);
    physics->activate(character->identity);
  }

  // Check whether in prep mode, and if so, manually set the position and keep the ball active
  // in the physics environment.
  if (!physics->checkActive(character->identity) && game_mode == k_prep_mode) {
    physics->setPositionAndRotation(character->identity,
      new Point(character->position->x, character->position->y, character->position->z),
      0, 0, character->shot_rotation);
    physics->activate(character->identity);
  }

  // In power mode, set the shot power gauge to fluctuate up and down
  if (game_mode == k_power_mode) {
    if (shot_rising) {
      character->shot_power += 0.25f;
    } else {
      character->shot_power -= 0.25f;
    }

    if (character->shot_power > character->default_shot_power) {
      shot_rising = false;
    }

    if (character->shot_power < 0.1f) {
      shot_rising = true;
    }

    // uncomment this to force a weak shot if the player doesn't activate the power gauge.
    // if (character->shot_power < 0.1f) {
    //   character->shot_power = k_default_shot_power / 10.0f;
    //   shoot();
    // }
  }

  // Update the character position (for rendering and game logic) from physics
  character->updateFromPhysics();

  // If the character has slowed to a crawl, stop the character
  if (physics->getVelocity(character->identity) < 0.02f) {
    physics->stop(character->identity);
  }

  // If the character has fallen far enough, quit the game
  if (character->position->z < -50) {
    quit = true;
  }
}

void Game::afterUpdate() {
  // To do: fix this so it doesn't break the drop action, but also doesn't
  // go back to long, awkward drops.
  if (game_mode == k_drop_mode && (!physics->checkActive(character->identity) ||
    physics->getVelocity(character->identity) < 0.00001)) {
    game_mode = k_prep_mode;
    character->up_shot = false;
    character->setShotRotation(character->default_shot_rotation, true);
  }
}

// Convenience method to do everything necessary for one "golf" shot.
void Game::shoot() {
  game_mode = k_action_mode;
  simulation_speed = default_speed_ramping;
  character->future_positions.clear();
  if (character->up_shot) {
    // an angled shot

    character->impulse(cos(k_up_shot_angle) * character->shot_power * sin(character->shot_rotation),
      cos(k_up_shot_angle) * -character->shot_power * cos(character->shot_rotation),
      sin(k_up_shot_angle) * character->shot_power);
  } else {
    // a flat shot
    character->impulse(character->shot_power * sin(character->shot_rotation),
      -character->shot_power * cos(character->shot_rotation),
      0.5);
  }
  physics->activate(character->identity);
}

// Handle keyboard input
void Game::handleKeys(SDL_Event e) {
  if (e.key.keysym.sym == SDLK_p) {
    quit = true;
  }

  if (game_mode == k_bear_select_mode) {
    if (e.key.keysym.sym == SDLK_DOWN) {
      bear_choice += 3;
      if (bear_choice > 8) bear_choice -= 9;
    } else if (e.key.keysym.sym == SDLK_UP) {
      bear_choice -= 3;
      if (bear_choice < 0) bear_choice += 9;
    } else if (e.key.keysym.sym == SDLK_LEFT) {
      bear_choice -= 1;
      if ((bear_choice + 3) % 3 == 2) bear_choice += 3;
    } else if (e.key.keysym.sym == SDLK_RIGHT) {
      bear_choice += 1;
      if (bear_choice % 3 == 0) bear_choice -= 3;
    } else if (e.key.keysym.sym == SDLK_RETURN) {
      if (available_bear_choices[bear_choice] == 1) {
        available_bear_choices[bear_choice]--;
        if (player_1_bears.size() == player_2_bears.size()) {
          player_1_bears.push_back(bear_choices[bear_choice]);
          player_1_choose_text->setColor(53, 62, 89);
          player_2_choose_text->setColor(140, 98, 57);
        } else {
          player_2_bears.push_back(bear_choices[bear_choice]);
          player_1_choose_text->setColor(140, 98, 57);
          player_2_choose_text->setColor(53, 62, 89);
        }

        if (player_1_bears.size() == 3 && player_2_bears.size() == 3) {
          // to do: this should be starting animation instead
          game_mode = k_lets_go_mode;
          mark_time = last_time;
          player_2_choose_text->setColor(53, 62, 89);
        }
      }
    }

    bear_name_text->setText(bear_pretty_names[bear_choices[bear_choice]]);
    bear_description_text->setText(bear_descriptions[bear_choices[bear_choice]]);

    return;
  }

  if (e.key.keysym.sym == SDLK_z) {
    zoom += 1.0f;
  } else if (e.key.keysym.sym == SDLK_x) {
    zoom -= 1.0f;
  }

  if (game_mode == k_prep_mode) {
    if (e.key.keysym.sym == SDLK_a) {
      character->setShotRotation(character->shot_rotation + M_PI / 50, true);
    }

    if (e.key.keysym.sym == SDLK_d) {
      character->setShotRotation(character->shot_rotation - M_PI / 50, true);
    }

    if (e.key.keysym.sym == SDLK_w) {
      character->up_shot = true;
      character->setShotRotation(character->shot_rotation, true);
    }

    if (e.key.keysym.sym == SDLK_s) {
      character->up_shot = false;
      character->setShotRotation(character->shot_rotation, true);
    }

    if (e.key.keysym.sym == SDLK_m) {
      game_mode = k_power_mode;
      character->shot_power = 0;
      shot_rising = true;
      return;
    }
  }

  if (game_mode == k_power_mode) {
    if (e.key.keysym.sym == SDLK_m) {
      shoot();
    }

    if (e.key.keysym.sym == SDLK_c) {
      game_mode = k_prep_mode;
    }
  }
}

// Handle mouse input
void Game::handleMouse(SDL_Event e) {
  // In prep mode, I use mousemotion, mousedown and mouseup to track mouse dragging.
  // In a drag event, I set the rotation of the character.
  if (game_mode == k_prep_mode) {
    if (mousedown && e.type == SDL_MOUSEMOTION) {
      int x = 0, y = 0;
      SDL_GetMouseState(&x, &y);
      character->setShotRotation(pre_drag_rotation - (drag_x - x) / 50.0f, false);
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
      mousedown = true;
      SDL_GetMouseState(&drag_x, &drag_y);
      pre_drag_rotation = character->shot_rotation;
    } else if (e.type == SDL_MOUSEBUTTONUP) {
      mousedown = false;
      int x = 0, y = 0;
      SDL_GetMouseState(&x, &y);
      character->setShotRotation(pre_drag_rotation - (drag_x - x) / 50.0f, true);
      // this is a cheap hack; if the player has released a click
      // and there's no change in the mouse position, count it as a regular click
      // and switch to power mode.
      if (x == drag_x && y == drag_y) {
        game_mode = k_power_mode;
        character->shot_power = 0;
        shot_rising = true;
      }
    }
  } else if (game_mode == k_power_mode) {
    // In power mode, clicking just makes you take the "golf" shot.
    if (e.type == SDL_MOUSEBUTTONUP) {
      shoot();
    }
  }
}

void Game::render() {
  graphics->clearScreen();
  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);

  if (game_mode == k_bear_select_mode || game_mode == k_lets_go_mode) {
    renderBearSelectMode();
    return;
  }

  renderBackground();

  graphics->set3d(zoom);

  // Set the camera to look down at the character.
  // For fun, change the z-value to change the viewing angle of the game.
  graphics->standardCamera(character->position->x + 15, character->position->y + 15, 10,
    character->position->x, character->position->y, 0);

  graphics->standardLightPosition();

  // render surfaces (walls and floors)
  for (auto surface = surfaces.begin(); surface != surfaces.end(); ++surface) {
    (*surface)->render();
  }

  // render bumpers
  for (auto bumper = bumpers.begin(); bumper != bumpers.end(); ++bumper) {
    (*bumper)->render();
  }

  // render hazards
  for (auto hazard = hazards.begin(); hazard != hazards.end(); ++hazard) {
    (*hazard)->render();
  }

  // render the character
  character->render(game_mode);

  // render 2D overlay
  graphics->start2DDraw();

  int info_x = 324;
  int info_y = 20;

  // render shot prep infographic
  if (game_mode == k_prep_mode) {
    textures->setTexture("m_prep_info");
    graphics->rectangle(info_x, info_y, 400, 400);
  }

  // render power mode
  if (game_mode == k_power_mode) {
    // render power mode infographic
    textures->setTexture("m_shot_info");
    graphics->rectangle(info_x, info_y, 400, 400);

    // render power gauge outline
    int power_x = 974;
    int power_y = 20;
    textures->setTexture("shot_power_outline");
    graphics->rectangle(power_x, power_y, 30, 150);

    // render power gauge fill
    float portion = character->shot_power / character->default_shot_power;
    textures->setTexture("shot_power_fill");
    // custom texture scaling for the fill
    float vertex_data[12] = {
      power_x + 0.0f, power_y + 150.0f - 140.0f * portion, 0.0f,
      power_x + 0.0f, power_y + 150.0f, 0.0f,
      power_x + 30.0f, power_y + 150.0f, 0.0f,
      power_x + 30.0f, power_y + 150.0f - 140.0f * portion, 0.0f
    };
    float texture_data[8] = {
      0.0f, 1.0f - 140.0f / 150.0f * portion,
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 1.0f - 140.0f / 150.0f * portion
    };
    graphics->rectangleWithTexture(vertex_data, texture_data);
  }

  // render coordinates
  int coord_x = 40;
  int coord_y = k_screen_height - 79 - 40;
  textures->setTexture("coordinates");
  graphics->rectangle(coord_x, coord_y, 93, 79);

  graphics->end2DDraw();
}

void Game::renderBackground() {
  graphics->start2DDraw();

  // background render
  textures->setTexture("clouds");
  graphics->rectangle(0, 0, k_screen_width, k_screen_height);

  graphics->end2DDraw();
}

void Game::renderBearSelectMode() {
  // Clear color buffer
  graphics->clearScreenWithColor(1.0f, 1.0f, 1.0f, 1.0f);

  graphics->start2DDraw();

  player_1_choose_text->render();
  player_2_choose_text->render();

  if (game_mode == k_bear_select_mode) {
    choose_your_bears_text->render();
    bear_name_text->render();
    bear_description_text->render();

    // textures->setTexture("bear_selection_box");
    // float m = k_bear_choice_x + k_bear_choice_margin * (bear_choice % 3);
    // float n = k_bear_choice_y + k_bear_choice_margin * (bear_choice / 3);
    // graphics->rectangle(m, n, k_selection_box_size, k_selection_box_size);

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
    float m = k_bear_choice_x + k_bear_choice_margin * (bear_choice % 3);
    float n = k_bear_choice_y + k_bear_choice_margin * (bear_choice / 3);
    graphics->rectangle(m, n, k_selection_box_size, k_selection_box_size);
  } else if (game_mode == k_lets_go_mode) {
    go_text->render();
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
    graphics->fadeOut(1.0f, 2.0f, ((last_time - mark_time) / 1000.0f) - 1.0f);
  }

  graphics->end2DDraw();
}

// This huge method puts all the crap on the game board.
bool Game::initializeGamePieces() {
  character = new Character(physics, new Point(0, 0, k_character_drop_height));
  physics->setRotation(character->identity, 0, 0, character->default_shot_rotation);

  // // Ramp
  // hazards.push_front(new Hazard("ramp", physics,
  //   new Point(4, 0, 0), -M_PI / 2.0));

  // // test wicket
  // hazards.push_front(new Hazard("wicket", physics,
  //   new Point(21, 1.5, -4), 0));

  // // test wicket
  // hazards.push_front(new Hazard("wicket", physics,
  //   new Point(54, 1.5, -8), 0));

  // // test wicket
  // hazards.push_front(new Hazard("wicket", physics,
  //   new Point(49.5, 9, -8), M_PI / 2.0));

  // // test wicket
  // hazards.push_front(new Hazard("wicket", physics,
  //   new Point(45, -46.5, 0), 0));

  // surfaces segment 1 (slide down)
  int height_array[] = {0, 0, 0, -2, -4, -4, -4, -6, -8};
  for (int k = 0; k < 8; k++) {
    for (int y = 0; y < 3; y++) {
      surfaces.push_front(new Surface(physics,
        false,
        new Point(0 + 6 * k, 0 + 6 * y, height_array[k+1]),
        new Point(-6 + 6 * k, 0 + 6 * y, height_array[k]),
        new Point(-6 + 6 * k, -6 + 6 * y, height_array[k]),
        new Point(0 + 6 * k, -6 + 6 * y, height_array[k+1])));
    }
  }

  // bumper segment 1
  bumpers.push_front(new Bumper(physics,
    new Point(-6, 12, 0),
    new Point(6, 12, 0),
    new Point(0, -k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(6, 12, 0),
    new Point(18, 12, -4),
    new Point(0, -k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(18, 12, -4),
    new Point(30, 12, -4),
    new Point(0, -k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(30, 12, -4),
    new Point(42, 12, -8),
    new Point(0, -k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-6, -6, 0),
    new Point(6, -6, 0),
    new Point(0, k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(6, -6, 0),
    new Point(18, -6, -4),
    new Point(0, k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(18, -6, -4),
    new Point(30, -6, -4),
    new Point(0, k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(30, -6, -4),
    new Point(42, -6, -8),
    new Point(0, k_bumper_width, 0)));

  // mountain segment 1
  int first_x = 10 + rand() % static_cast<int>(20 + 1);
  int last_y = 25 + rand() % static_cast<int>(25 + 1);
  for (int k = 0; k < 8; k++) {
    int new_y = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics,
      true,
      new Point(0 + 6 * k, 0 + (6 * 2) + new_y, height_array[k+1] - 70),
      new Point(-6 + 6 * k - first_x, 0 + (6 * 2) + last_y, height_array[k] - 70),
      new Point(-6 + 6 * k, 0 + (6 * 2), height_array[k]),
      new Point(0 + 6 * k, 0 + (6 * 2), height_array[k+1])));
    last_y = new_y;
    first_x = 0;
  }

  // surfaces segment 2 (corner)
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      surfaces.push_front(new Surface(physics,
        false,
        new Point(48 + 6 * x, 0 + 6 * y, -8),
        new Point(42 + 6 * x, 0 + 6 * y, -8),
        new Point(42 + 6 * x, -6 + 6 * y, -8),
        new Point(48 + 6 * x, -6 + 6 * y, -8)));
    }
  }

  // bumper segment 2 (corner)
  bumpers.push_front(new Bumper(physics,
    new Point(60, 12, -8),
    new Point(42, 12, -8),
    new Point(0, -k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(60, 12, -8),
    new Point(60, -6, -8),
    new Point(-k_bumper_width, 0.0, 0)));

  // mountain segment 2
  for (int k = 0; k < 3; k++) {
    int new_y = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics,
      true,
      new Point(48 + 6 * k, 0 + (6 * 2) + new_y, -8 - 70),
      new Point(42 + 6 * k, 0 + (6 * 2) + last_y, -8 - 70),
      new Point(42 + 6 * k, 0 + (6 * 2), -8),
      new Point(48 + 6 * k, 0 + (6 * 2), -8)));
    last_y = new_y;
  }

  // mountain corner
  int last_x = 25 + rand() % static_cast<int>(25 + 1);
  surfaces.push_front(new Surface(physics,
    true,
    new Point(60 + last_x, 0 + (6 * 2) + last_y, -8 - 70),
    new Point(60, 0 + (6 * 2) + last_y, -8 - 70),
    new Point(60, 0 + (6 * 2), -8),
    new Point(60, 0 + (6 * 2), -8)));

  // mountain segment 3
  for (int k = 0; k < 3; k++) {
    int new_x = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics,
      true,
      new Point(48 + 6 * 2 + new_x, 6 - 6 * k, -8 - 70),
      new Point(48 + 6 * 2 + last_x, 12 - 6 * k + last_y, -8 - 70),
      new Point(48 + 6 * 2, 12 - 6 * k, -8),
      new Point(48 + 6 * 2, 6 - 6 * k, -8)));
    last_x = new_x;
    last_y = 0;
  }

  // surfaces segment 3 (slide back up)
  int height_array2[] = {-8, -6, -4, -4, -4, -2, 0, 0, 0};
  for (int k = 0; k < 8; k++) {
    for (int x = 0; x < 3; x++) {
      surfaces.push_front(new Surface(physics,
        false,
        new Point(48 + 6 * x, -6 - 6 * k, height_array2[k]),
        new Point(42 + 6 * x, -6 - 6 * k, height_array2[k]),
        new Point(42 + 6 * x, -12 - 6 * k, height_array2[k+1]),
        new Point(48 + 6 * x, -12 - 6 * k, height_array2[k+1])));
    }
  }

  // bumper segment 3
  bumpers.push_front(new Bumper(physics,
    new Point(60, -6, -8),
    new Point(60, -18, -4),
    new Point(-k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(60, -18, -4),
    new Point(60, -30, -4),
    new Point(-k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(60, -30, -4),
    new Point(60, -42, 0),
    new Point(-k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(60, -42, 0),
    new Point(60, -54, 0),
    new Point(-k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(42, -6, -8),
    new Point(42, -18, -4),
    new Point(k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(42, -18, -4),
    new Point(42, -30, -4),
    new Point(k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(42, -30, -4),
    new Point(42, -42, 0),
    new Point(k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(42, -42, 0),
    new Point(42, -54, 0),
    new Point(k_bumper_width, 0.0, 0)));

  // mountain segment 4
  for (int k = 0; k < 8; k++) {
    int new_x = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics,
      true,
      new Point(48 + 6 * 2 + new_x, -12 - 6 * k, height_array2[k+1] - 70),
      new Point(48 + 6 * 2 + last_x, -6 - 6 * k, height_array2[k] - 70),
      new Point(48 + 6 * 2, -6 - 6 * k, height_array2[k]),
      new Point(48 + 6 * 2, -12 - 6 * k, height_array2[k+1])));
    last_x = new_x;
  }

  return true;
}

bool Game::initialize() {
  graphics->initialize();

  // Initialize Textures
  if (!initializeTextures()) {
    printf("Unable to initialize textures!\n");
    return false;
  }

  // Initialize Bullet Physics
  physics = new Physics();
  if (!physics->initializePhysics()) {
    printf("Unable to initialize Bullet Physics!\n");
    return false;
  }

  // Initialize Game
  if (!initializeGamePieces()) {
    printf("Unable to initialize game logic!\n");
    return false;
  }

  start_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  last_time = start_time;
  framerate_time = start_time;

  return true;
}

bool Game::initializeTextures() {
  textures->addTexture("coordinates", "coordinates.png");

  textures->addTexture("tiles", "tiles4.png");

  textures->addTexture("bumper", "bumper2.png");
  textures->addTexture("lit_bumper", "bumper4.png");

  textures->addTexture("bear_face", "bear_test2.png");
  textures->addTexture("bear_nose", "nose.png");
  textures->addTexture("bear_arms", "parts.png");

  textures->addTexture("plain", "plain.png");
  textures->addTexture("black", "black.png");

  textures->addTexture("shot_power_outline", "ShotPowerOutline.png");
  textures->addTexture("shot_power_fill", "ShotPowerFill.png");

  textures->addTexture("clouds", "clouds.png");

  textures->addTexture("left_turn_info", "A_Key_Turn_Left.png");
  textures->addTexture("right_turn_info", "D_Key_Turn_Right.png");
  textures->addTexture("m_prep_info", "M_Key_Prep_Your_Shot.png");
  textures->addTexture("m_shot_info", "M_Again_Set_Power_And_Take_Shot.png");

  //textures->addTexture("wicket", "barber_pole.png");

  //////

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

  ///////

  choose_your_bears_text = new TextBox("cartoon_blocks.ttf", 60,
    "Choose your bears", 53, 62, 89, 44, 22);
  player_1_choose_text = new TextBox("cartoon_blocks.ttf", 60,
    "Player 1", 140, 98, 57, 44, 128);
  player_2_choose_text = new TextBox("cartoon_blocks.ttf", 60,
    "Player 2", 53, 62, 89, 1147, 128);
  bear_name_text = new TextBox("cartoon_blocks.ttf", 50,
    bear_pretty_names[bear_choices[bear_choice]], 53, 62, 89, 362, 93);
  bear_description_text = new TextBox("cartoon_blocks.ttf", 40,
    bear_descriptions[bear_choices[bear_choice]], 53, 62, 89, 362, 149);
  go_text = new TextBox("cartoon_blocks.ttf", 200,
    "let's go!", 53, 62, 89, k_bear_choice_x, k_bear_choice_y);

  ///////

  return true;
}

void Game::shutdown() {
  physics->shutdown();
  delete physics;
}
