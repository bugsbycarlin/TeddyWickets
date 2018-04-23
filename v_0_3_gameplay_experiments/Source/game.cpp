/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "game.h"

using namespace tinyxml2;

// The author of tinyxml says,
// "For my own convenience, I also defined a macro for checking error results:"
#ifndef XMLCheckResult
  #define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); return false; }
#endif

// Game constructor
Game::Game(std::vector<std::string> player_1_bears, std::vector<std::string> player_2_bears) {
  srand (time(NULL));

  quit = false;
  current_screen = k_2p_game_screen;

  zoom = k_default_zoom;
  default_speed_ramping = k_default_speed_ramping;
  simulation_speed = k_default_minimum_speed;

  this->player_1_bears = player_1_bears;
  this->player_2_bears = player_2_bears;

  recompute_trajectory = false;

  sway = 0;

  hud_step = hot_config->getInt("hud_step");

  this->shot_marker = model_cache->getModel("marker.obj");

  player_1_score = 0;
  player_1_display_score = 0;
  player_2_score = 0;
  player_2_display_score = 0;

  look = false;
  look_x = 0;
  look_y = 0;

  game_mode = k_setup_mode;

  music = "";

  map_file = hot_config->getString("map_file");

  taunts.push_back("Your mother was a dope dealer!");
  taunts.push_back("The best bears are me and not you!");
  taunts.push_back("Sonny is gonna have a day with you!");
  taunts.push_back("Eggs! Eggs! Eggs! Eggs! Eggs! Wad.");
  taunts.push_back("DABAAA DABAAAA dedoodoodoodoo");
  taunts.push_back("Winnie the Pooh! Doesnt like you!");
  taunts.push_back("It is hard to read an incomplete");
  taunts.push_back("The fundamentals are overrated!");
  taunts.push_back("Once upon a time you sucked.");
  taunts.push_back("What gender am I? Think about it.");
  taunts.push_back("Donut look down. Or do. Or donut.");
  taunts.push_back("This is the song that never ends!");
  taunts.push_back("Weeee are never ever ever getting");
  taunts.push_back("Mmm bop mmm mmm mmm bop doobiedowa");
  taunts.push_back("If you believe in life after love!");
  taunts.push_back("I will mash your potatoes.");
  taunts.push_back("Six plus seven plus three equals?");
  taunts.push_back("You are now out of cool whip.");
  taunts.push_back("Walk slowly, hipster.");
  taunts.push_back("Pump jockey. Works for tips!");
  taunts.push_back("Forget it. I am simply not credible.");
  taunts.push_back("Where are my waffles?!");
  taunts.push_back("Tweet about it, douche!");
  taunt_time = 0;
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

  // Update sound
  sound_system->update();

  // Update screen
  SDL_GL_SwapWindow(window);

  afterUpdate();
}

void Game::update() {
  // Set time and perform physics update
  current_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  physics->update(simulation_speed * (current_time - last_time) / 1000.0f);
  last_time = current_time;
  if (simulation_speed > k_default_minimum_speed) {
    simulation_speed *= 0.98f;
  }

  sway = 0.5 * sin((current_time - start_time) / 1000.0f);

  if (current_time - framerate_time > 1000.0f) {
    printf("Framerate: %d\n", frames_since_last);
    //printf("Ball: %0.2f, %0.2f, %0.2f\n", character->position->x, character->position->y, character->position->z);
    framerate_time = current_time;
    frames_since_last = 0;
  } else {
    frames_since_last++;
  }

  if (taunt_time > 0) {
    taunt_time -= 1;
  }

  // Move the moving hazards
  for (auto hazard = hazards.begin(); hazard != hazards.end(); ++hazard) {
    (*hazard)->update((current_time - start_time) / 1000.0f);
  }

  // In setup mode, activate all inactive bears and return them to their starting positions, plus drop height
  if (game_mode == k_setup_mode) {
    for (auto character = characters.begin(); character != characters.end(); ++character) {
      if ((*character)->status == k_bear_status_sidelined) {
        printf("Restoring character %d to %0.2f, %0.2f, %0.2f\n", (*character)->roster_number, (*character)->last_drop_position->x,
          (*character)->last_drop_position->y, (*character)->last_drop_position->z);
        (*character)->status = k_bear_status_normal;
        (*character)->velocity_history = {};
        (*character)->position_history = {};
        physics->setPositionAndRotation((*character)->identity,
          (*character)->last_drop_position,
          0, 0, (*character)->default_shot_rotation);
        physics->activate((*character)->identity);
      }
    }

    if (bearsAreSetup()) {
      game_mode = k_aim_mode;
      look = false;
      current_character->up_shot = false;
      current_character->setShotRotation(current_character->default_shot_rotation, false);
      recompute_trajectory = true;
    }
  }

  // If we're in aim mode, and we need to recompute the shot trajectory, recompute the shot trajectory
  if (game_mode == k_aim_mode && recompute_trajectory && hot_config->getInt("use_shot_trajectories") == 1) {
    recompute_trajectory = false;

    physics->activate(current_character->identity);

    // Save all the bear positions
    for (auto character = characters.begin(); character != characters.end(); ++character) {
      (*character)->save_transform = physics->getTransform((*character)->identity);
    }

    // Set up the shot
    future_positions.clear();
    if (current_character->up_shot) {
      // ... using an angled degree shot
      current_character->impulse(cos(k_up_shot_angle) * current_character->default_shot_power * sin(current_character->shot_rotation),
        cos(k_up_shot_angle) * -current_character->default_shot_power * cos(current_character->shot_rotation),
        sin(k_up_shot_angle) * current_character->default_shot_power);
    } else {
      // ... using a flat shot
      current_character->impulse(current_character->default_shot_power * sin(current_character->shot_rotation),
        -current_character->default_shot_power * cos(current_character->shot_rotation), 0.5);
    }

    // Calculate all the future positions
    for (int i = 0; i < 300; i++) {
      physics->update(1.0f / 60.f);
      if (i > 0 && i % 8 == 0) future_positions.push_front(btTransform(physics->getTransform(current_character->identity)));
    }

    // Reset the bears
    for (auto character = characters.begin(); character != characters.end(); ++character) {
      physics->setTransform((*character)->identity, (*character)->save_transform);

      if ((*character)->status != k_bear_status_finished) {
        physics->stop((*character)->identity);
        physics->setPositionAndRotation((*character)->identity,
          new Point((*character)->position->x, (*character)->position->y, (*character)->position->z),
          0, 0, (*character)->shot_rotation);
        physics->activate((*character)->identity);
      }
    }

    physics->update(1.0f / 60.f);
  }

  // Check whether in aim mode, and if so, manually set the position and keep the ball active
  // in the physics environment. I believe this is necessary to allow the turns to keep happening.
  if (game_mode == k_aim_mode && !physics->checkActive(current_character->identity)) {
    physics->setPositionAndRotation(current_character->identity,
      new Point(current_character->position->x, current_character->position->y, current_character->position->z),
      0, 0, current_character->shot_rotation);
    physics->activate(current_character->identity);
  }

  // In power mode, set the shot power gauge to fluctuate up and down
  if (game_mode == k_power_mode) {
    if (shot_rising) {
      current_character->shot_power += 0.25f;
    } else {
      current_character->shot_power -= 0.25f;
    }

    if (current_character->shot_power > current_character->default_shot_power) {
      shot_rising = false;
    }

    if (current_character->shot_power < 0.1f) {
      shot_rising = true;
    }
  }

  // Check whether action has finished, and if so, switch to setup mode
  if (game_mode == k_action_mode && bearsAreExhausted()) {
    game_mode = k_setup_mode;

    // update to the next character
    current_character_number += 1;
    if (current_character_number >= characters.size()) {
      current_character_number = 0;
    }
    current_character = characters[current_character_number];

    printf("Changed mode to k_setup_mode!\n");
  }

  if (game_mode == k_setup_mode && current_character->status == k_bear_status_finished) {
    // update to the next character
    current_character_number += 1;
    if (current_character_number >= characters.size()) {
      current_character_number = 0;
    }
    current_character = characters[current_character_number];
  }

  for (auto character = characters.begin(); character != characters.end(); ++character) {
    // Update the character position (for rendering and game logic) from physics
    (*character)->updateFromPhysics();

    // If the character has slowed to a crawl, stop the character
    if (physics->getVelocity((*character)->identity) < 0.02f) {
      physics->stop((*character)->identity);
    }

    // If the character has fallen off the world, reset the character to its starting place
    if (game_mode == k_action_mode && (*character)->status != k_bear_status_finished && (*character)->position->z < -10) {
      
      physics->stop((*character)->identity);
      // Move out of the game somewhere
      physics->setPositionAndRotation((*character)->identity,
        new Point(-1000, -1000, -1000),
        0, 0, (*character)->default_shot_rotation);
      (*character)->status = k_bear_status_sidelined;
    }

    // Test character against wickets and free stars
    if ((*character)->position_history.size() > 1) {
      for (auto wicket = wickets.begin(); wicket != wickets.end(); ++wicket) {
        (*wicket)->flipWicket((*character)->position_history[0], (*character)->position_history[1], (*character)->player_number);
      }

      for (auto free_star = free_stars.begin(); free_star != free_stars.end(); ++free_star) {
        (*free_star)->flipStar((*character)->position_history[0], (*character)->radius, (*character)->player_number);
      }

      // Test character against final wicket
      if (last_wicket->active) {
        bool result = last_wicket->flipWicket((*character)->position_history[0], (*character)->position_history[1], (*character)->player_number);
        if (result) {
          (*character)->status = k_bear_status_finished;
          physics->stop((*character)->identity);
          // Move out of the game somewhere
          physics->setPositionAndRotation((*character)->identity,
            new Point(-1000, -1000, -1000),
            0, 0, (*character)->default_shot_rotation);
        }
      }
    }
  }

  updateScores();

  std::stringstream stream;
  std::string s;

  // stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[0]->identity);
  // bear_velocity_1->setText(stream.str());
  // stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[1]->identity);
  // bear_velocity_2->setText(stream.str());
  // stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[2]->identity);
  // bear_velocity_3->setText(stream.str());
  // stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[3]->identity);
  // bear_velocity_4->setText(stream.str());
  // stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[4]->identity);
  // bear_velocity_5->setText(stream.str());
  // stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[5]->identity);
  // bear_velocity_6->setText(stream.str());
  
}

void Game::updateScores() {

  // Calculate scores based on wickets
  player_1_score = 0;
  player_2_score = 0;
  bool remaining_wickets = false;
  for (auto wicket = wickets.begin(); wicket != wickets.end(); ++wicket) {
    int value = (*wicket)->value;
    int player_owner = (*wicket)->player_owner;
    if (player_owner == 1) {
      player_1_score += value;
    } else if (player_owner == 2) {
      player_2_score += value;
    } else {
      remaining_wickets = true;
    }
  }

  for (auto free_star = free_stars.begin(); free_star != free_stars.end(); ++free_star) {
    int value = (*free_star)->value;
    int player_owner = (*free_star)->player_owner;
    if (player_owner == 1) {
      player_1_score += value;
    } else if (player_owner == 2) {
      player_2_score += value;
    }
  }

  if (!remaining_wickets) {
    last_wicket->active = true;
  }

  bool end_game = false;
  bool player_1_finished = true;
  bool player_2_finished = true;
  for (auto character = characters.begin(); character != characters.end(); ++character) {
    if ((*character)->player_number == 1 && (*character)->status != k_bear_status_finished) {
      player_1_finished = false;
    }
    if ((*character)->player_number == 2 && (*character)->status != k_bear_status_finished) {
      player_2_finished = false;
    }
  }

  // Note it is possible for both sides to get full credit for the last wicket
  if (player_1_finished) {
    player_1_score += last_wicket->value;
    end_game = true;
  }

  if (player_2_finished) {
    player_2_score += last_wicket->value;
    end_game = true;
  }

  if (end_game && game_mode != k_end_mode) {
    game_mode = k_end_mode;

    player_1_display_score = 0;
    player_2_display_score = 0;

    Point* color;
    if (player_1_score > player_2_score) {
      end_mode_box->setText("Player 1 wins!");
      color = colors->color("salmon");
    } else if (player_1_score < player_2_score) {
      end_mode_box->setText("Player 2 wins!");
      color = colors->color("purple");
    } else if (player_1_score == player_2_score) {
      end_mode_box->setText("And ties? You bet!");
      color = colors->color("mint_green");
    }
    end_mode_box->setColor(color->x, color->y, color->z);
  }

  // Update score text boxes
  if (player_1_display_score < player_1_score) {
    player_1_display_score += 1;
  } else if (player_1_display_score > player_1_score) {
    player_1_display_score -= 1;
  }
  player_1_score_box->setText(std::to_string(player_1_display_score));
  player_1_score_box->x = hot_config->getInt("player_1_score_box_x");
  if (game_mode == k_end_mode) {
    player_1_score_box->x = hot_config->getInt("player_1_end_score_box_x");
    player_1_score_box->y = hot_config->getInt("player_1_end_score_box_y");
  }
  if (player_1_display_score >= 10) {
    player_1_score_box->x = player_1_score_box->x - (int) (hot_config->getInt("wicket_font_size") / 4);
  }
  if (player_1_display_score >= 100) {
    player_1_score_box->x = player_1_score_box->x - (int) (hot_config->getInt("wicket_font_size") / 4);
  }

  if (player_2_display_score < player_2_score) {
    player_2_display_score += 1;
  } else if (player_2_display_score > player_2_score) {
    player_2_display_score -= 1;
  }
  player_2_score_box->setText(std::to_string(player_2_display_score));
  player_2_score_box->x = hot_config->getInt("player_2_score_box_x");
  if (game_mode == k_end_mode) {
    player_2_score_box->x = hot_config->getInt("player_2_end_score_box_x");
    player_2_score_box->y = hot_config->getInt("player_2_end_score_box_y");
  }
  if (player_2_display_score >= 10) {
    player_2_score_box->x = player_2_score_box->x - (int) (hot_config->getInt("wicket_font_size") / 4);
  }
  if (player_2_display_score >= 100) {
    player_2_score_box->x = player_2_score_box->x - (int) (hot_config->getInt("wicket_font_size") / 4);
  }
}

void Game::afterUpdate() {
}

// Return true if no bears are sidelined and all bears have stopped moving 
bool Game::bearsAreSetup() {
  bool bearsAreSetup = true;
  for (auto character = characters.begin(); character != characters.end(); ++character) {
    if ((*character)->status == k_bear_status_sidelined) {
      bearsAreSetup = false;
    }

    if ((*character)->status == k_bear_status_normal && !(*character)->stoppedMoving()) {
      bearsAreSetup = false;
    }
  }
  return bearsAreSetup;
}

// Return true if all bears are either sidelined or finished or have stopped moving 
bool Game::bearsAreExhausted() {
  bool bearsAreExhausted = true;
  for (auto character = characters.begin(); character != characters.end(); ++character) {
    if ((*character)->status != k_bear_status_sidelined && (*character)->status != k_bear_status_finished && !(*character)->stoppedMoving()) {
      bearsAreExhausted = false;
    }
  }
  return bearsAreExhausted;
}

void Game::shoot() {
  game_mode = k_action_mode;
  for (auto character = characters.begin(); character != characters.end(); ++character) {
    (*character)->velocity_history = {};
    (*character)->position_history = {};
  }
  future_positions.clear();
  simulation_speed = default_speed_ramping;
  if (current_character->up_shot) {
    // an angled shot

    current_character->impulse(cos(k_up_shot_angle) * current_character->shot_power * sin(current_character->shot_rotation),
      cos(k_up_shot_angle) * -current_character->shot_power * cos(current_character->shot_rotation),
      sin(k_up_shot_angle) * current_character->shot_power);
  } else {
    // a flat shot
    current_character->impulse(current_character->shot_power * sin(current_character->shot_rotation),
      -current_character->shot_power * cos(current_character->shot_rotation),
      0.5);
  }
  // Reset shot rotation
  current_character->shot_rotation = current_character->default_shot_rotation;
  physics->activate(current_character->identity);
}

// Handle keyboard input
void Game::handleKeys(SDL_Event e) {

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
void Game::handleController(SDL_Event e) {
  std::string value = control_map->translateControllerEvent(e);

  for (auto item = control_map->ordered_controls.begin(); item != control_map->ordered_controls.end(); ++item) {
    if (control_map->control_map[*item] == value) {
      handleAction(*item);
    }
  }
}


// Handle actions as translated from the control map
void Game::handleAction(std::string action) {

  if (game_mode == k_aim_mode || game_mode == k_power_mode || game_mode == k_action_mode) {
    // Taunt
    if ((action == "player_1_view_taunt" && current_character_number % 2 == 1) ||
        ((action == "player_2_view_taunt" && current_character_number % 2 == 0))) {
      taunt_time = k_total_taunt_time;
      if (current_character_number % 2 == 1) {
        taunt_bear = player_1_bears[0];
      } else {
        taunt_bear = player_2_bears[0];
      }
      taunt_box->setText(taunts[rand() % taunts.size()]);
    }
  }

  if (game_mode == k_aim_mode) {
    if (!look) {
      if ((action == "player_1_right" && current_character_number % 2 == 0) ||
          ((action == "player_2_right" && current_character_number % 2 == 1))) {
        current_character->setShotRotation(current_character->shot_rotation - M_PI / 25, false);
        recompute_trajectory = true;
      }

      if ((action == "player_1_left" && current_character_number % 2 == 0) ||
          ((action == "player_2_left" && current_character_number % 2 == 1))) {
        current_character->setShotRotation(current_character->shot_rotation + M_PI / 25, false);
        recompute_trajectory = true;
      }

      if ((action == "player_1_up" && current_character_number % 2 == 0) ||
          ((action == "player_2_up" && current_character_number % 2 == 1))) {
        current_character->up_shot = true;
        current_character->setShotRotation(current_character->shot_rotation, false);
        recompute_trajectory = true;
      }

      if ((action == "player_1_down" && current_character_number % 2 == 0) ||
          ((action == "player_2_down" && current_character_number % 2 == 1))) {
        current_character->up_shot = false;
        current_character->setShotRotation(current_character->shot_rotation, false);
        recompute_trajectory = true;
      }
    } else if (look) {

      if ((action == "player_1_right" && current_character_number % 2 == 0) ||
          ((action == "player_2_right" && current_character_number % 2 == 1))) {
        look_y += 3;
        look_x -= 3;
      }

      if ((action == "player_1_left" && current_character_number % 2 == 0) ||
          ((action == "player_2_left" && current_character_number % 2 == 1))) {
        look_y -= 3;
        look_x += 3;
      }

      if ((action == "player_1_up" && current_character_number % 2 == 0) ||
          ((action == "player_2_up" && current_character_number % 2 == 1))) {
        look_y -= 3;
        look_x -= 3;
      }

      if ((action == "player_1_down" && current_character_number % 2 == 0) ||
          ((action == "player_2_down" && current_character_number % 2 == 1))) {
        look_y += 3;
        look_x += 3;
      }
    }

    // View
    if ((action == "player_1_view_taunt" && current_character_number % 2 == 0) ||
        ((action == "player_2_view_taunt" && current_character_number % 2 == 1))) {
      if (game_mode == k_aim_mode && !look) {
        look = true;
        look_x = 0;
        look_y = 0;
      } else if (look) {
        look = false;
      }
    }

    // Cancel look
    if ((action == "player_1_cancel_switch" && current_character_number % 2 == 0) ||
        ((action == "player_2_cancel_switch" && current_character_number % 2 == 1))) {
      look = false;
    }

    if ((action == "player_1_shoot_accept" && current_character_number % 2 == 0) ||
        ((action == "player_2_shoot_accept" && current_character_number % 2 == 1))) {
      look = false;
      game_mode = k_power_mode;
      current_character->shot_power = 0;
      shot_rising = true;
      return;
    }
  }

  if (game_mode == k_power_mode) {
    if ((action == "player_1_shoot_accept" && current_character_number % 2 == 0) ||
        ((action == "player_2_shoot_accept" && current_character_number % 2 == 1))) {
      shoot();
    }

    if ((action == "player_1_cancel_switch" && current_character_number % 2 == 0) ||
        ((action == "player_2_cancel_switch" && current_character_number % 2 == 1))) {
      game_mode = k_aim_mode;
      look = false;
    }
  }
}

void Game::render() {
  graphics->clearScreen();
  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);

  renderBackground();

  graphics->set3d(zoom);
  graphics->enableLights();

  // Set the camera to look down at the character.
  // For fun, change the z-value to change the viewing angle of the game.
  int look_adjust_x = 0;
  int look_adjust_y = 0;
  if (game_mode == k_aim_mode && look) {
    look_adjust_x = look_x;
    look_adjust_y = look_y;
  }
  if (current_character->status == k_bear_status_normal) {
    graphics->standardCamera(current_character->position->x + 15 + look_adjust_x, current_character->position->y + 15 + look_adjust_y, 10,
      current_character->position->x + look_adjust_x, current_character->position->y + look_adjust_y, 0);
  } else {
    graphics->standardCamera(current_character->last_drop_position->x + 15 + look_adjust_x, current_character->last_drop_position->y + 15 + look_adjust_y, 10,
      current_character->last_drop_position->x + look_adjust_x, current_character->last_drop_position->y + look_adjust_y, 0);
  }

  float cycle = ((current_time - start_time) / 1000.0f) / 100.0f;
  float distance = 200;
  float light_z = 200 * cos(cycle);
  if (light_z < 0) light_z = -1 * light_z;
  graphics->setLightPosition(200 * sin(cycle), 18, light_z);

  // render hazards (this should be something different, like components)
  for (auto hazard = hazards.begin(); hazard != hazards.end(); ++hazard) {
    (*hazard)->render();
  }

  // render characters (balls) (i need to work on my naming)
  for (auto character = characters.begin(); character != characters.end(); ++character) {
    if ((*character)->status == k_bear_status_normal) {
      (*character)->render(0);
    }
  }

  // Render shot marker
  if (game_mode == k_aim_mode || game_mode == k_power_mode) {
    for (auto position = future_positions.begin(); position != future_positions.end(); ++position) {
      btScalar transform_matrix[16];
      position->getOpenGLMatrix(transform_matrix);
      graphics->pushModelMatrix();
      graphics->multMatrix(transform_matrix);
      shot_marker->render();
      graphics->popModelMatrix();
    }
  }

  // Wicket info
  for (auto wicket = wickets.begin(); wicket != wickets.end(); ++wicket) {
    (*wicket)->setRenderInfo((current_time - start_time) / 1000.0f);
  }
  last_wicket->setRenderInfo((current_time - start_time) / 1000.0f);

  for (auto free_star = free_stars.begin(); free_star != free_stars.end(); ++free_star) {
    (*free_star)->setRenderInfo((current_time - start_time) / 1000.0f);
  }

  // Render theme tile
  if (theme == "water") {
    graphics->pushModelMatrix();
    graphics->translate(50, 0 + sway, 0);
    //theme_tile->render();
    graphics->popModelMatrix();
  }

  // render 2D overlay
  graphics->start2DDraw();

  for (auto wicket = wickets.begin(); wicket != wickets.end(); ++wicket) {
    (*wicket)->renderInfo();
  }
  last_wicket->renderLastWicketInfo();
  for (auto free_star = free_stars.begin(); free_star != free_stars.end(); ++free_star) {
    (*free_star)->renderInfo();
  }

  textures->setTexture("player_1_HUD_background");
  graphics->rectangle(0, 0, 120, k_screen_height);

  textures->setTexture("player_2_HUD_background");
  graphics->rectangle(k_screen_width - 120, 0, 120, k_screen_height);

  //if ((int) ((current_time - start_time) / 20.0f) % 2 == 0) {
    textures->setTexture("selection_2");
    graphics->rectangle(hot_config->getInt("player_" + std::to_string(current_character_number % 2 + 1) + "_x_margin"), hot_config->getInt("y_margin") + ((int) current_character_number / 2) * hud_step, 103, 103);
  //}

  for (int i = 0; i < player_1_bears.size(); i++) {
    textures->setTexture(player_1_bears[i] + "_box");
    graphics->rectangle(hot_config->getInt("player_1_x_margin"), hot_config->getInt("y_margin") + i * hud_step, 103, 103);
  }

  for (int i = 0; i < player_2_bears.size(); i++) {
    textures->setTexture(player_2_bears[i] + "_box");
    graphics->rectangle(hot_config->getInt("player_2_x_margin"), hot_config->getInt("y_margin") + i * hud_step, 103, 103);
  }


  // bear_velocity_1->render();
  // bear_velocity_2->render();
  // bear_velocity_3->render();
  // bear_velocity_4->render();
  // bear_velocity_5->render();
  // bear_velocity_6->render();

  int act_margin = hot_config->getInt("player_1_x_margin");
  int taunt_margin = hot_config->getInt("player_2_x_margin");
  int act_step = player_1_bears.size();
  int taunt_step = player_2_bears.size();
  if (current_character_number % 2 == 1) {
    act_margin = hot_config->getInt("player_2_x_margin");
    taunt_margin = hot_config->getInt("player_1_x_margin");
    act_step = player_2_bears.size();
    taunt_step = player_1_bears.size();
  }

  if (game_mode == k_aim_mode || game_mode == k_power_mode || game_mode == k_action_mode) {
    if(current_character->up_shot) {
      textures->setTexture("up_shot_glyph");
    } else {
      textures->setTexture("flat_shot_glyph");
    }
    graphics->rectangle(act_margin, hot_config->getInt("y_margin") + act_step * hud_step, 103, 103);

    textures->setTexture("taunt");
    graphics->rectangle(taunt_margin, hot_config->getInt("y_margin") + taunt_step * hud_step, 103, 103);  

    control_map->render(taunt_margin + 18, hot_config->getInt("y_margin") + (act_step + 1) * hud_step, "player_" + std::to_string((current_character_number+1) % 2 + 1) + "_view_taunt");      
  }

  if (game_mode == k_aim_mode) {
    control_map->render(act_margin - 10, hot_config->getInt("y_margin") + (act_step + 1) * hud_step, "player_" + std::to_string(current_character_number % 2 + 1) + "_up");
    control_map->render(act_margin - 10 + 55, hot_config->getInt("y_margin") + (act_step + 1) * hud_step, "player_" + std::to_string(current_character_number % 2 + 1) + "_down");
  
    control_map->render(act_margin + 18, hot_config->getInt("y_margin") + (act_step + 1) * hud_step + 55, "player_" + std::to_string(current_character_number % 2 + 1) + "_shoot_accept");
  }

  // render power mode
  if (game_mode == k_power_mode || game_mode == k_action_mode) {
    textures->setTexture("mallet_background_glyph");
    graphics->rectangle(act_margin, hot_config->getInt("y_margin") + (act_step + 1) * hud_step, 103, 103);

    textures->setTexture("mallet_glyph");
    graphics->pushModelMatrix();
    graphics->translate(act_margin + 36, hot_config->getInt("y_margin") + (act_step + 1) * hud_step + 21, 0);
    graphics->rotate(-90.0 * current_character->shot_power / current_character->default_shot_power, 0.0f, 0.0f, 1.0f);
    //graphics->printModel();
    graphics->rectangle(-36, -21, 103, 103);
    graphics->popModelMatrix();
  }

  if (game_mode == k_power_mode) {
    control_map->render(act_margin - 10, hot_config->getInt("y_margin") + (act_step + 2) * hud_step, "player_" + std::to_string(current_character_number % 2 + 1) + "_shoot_accept");
    control_map->render(act_margin - 10 + 55, hot_config->getInt("y_margin") + (act_step + 2) * hud_step, "player_" + std::to_string(current_character_number % 2 + 1) + "_cancel_switch");
  }

  if (game_mode == k_aim_mode && look) {
    if (current_character_number % 2 == 0) {
      control_map->render(hot_config->getInt("look_left_x"), hot_config->getInt("look_left_y"), "player_1_left");
      control_map->render(hot_config->getInt("look_right_x"), hot_config->getInt("look_right_y"), "player_1_right");
      control_map->render(hot_config->getInt("look_up_x"), hot_config->getInt("look_up_y"), "player_1_up");
      control_map->render(hot_config->getInt("look_down_x"), hot_config->getInt("look_down_y"), "player_1_down");
    } else {
      control_map->render(hot_config->getInt("look_left_x"), hot_config->getInt("look_left_y"), "player_2_left");
      control_map->render(hot_config->getInt("look_right_x"), hot_config->getInt("look_right_y"), "player_2_right");
      control_map->render(hot_config->getInt("look_up_x"), hot_config->getInt("look_up_y"), "player_2_up");
      control_map->render(hot_config->getInt("look_down_x"), hot_config->getInt("look_down_y"), "player_2_down");
    }
  }

  if (game_mode == k_action_mode) {
    textures->setTexture("special_power");
    graphics->rectangle(act_margin, hot_config->getInt("y_margin") + (act_step + 2) * hud_step, 103, 103);

    control_map->render(act_margin + 18, hot_config->getInt("y_margin") + (act_step + 3) * hud_step, "player_" + std::to_string(current_character_number % 2 + 1) + "_shoot_accept");
  }


  if (taunt_time > 0) {
    textures->setTexture(taunt_bear + "_box");
    graphics->rectangle(201, 777, 103, 103);

    textures->setTexture("speech_bubble");
    graphics->rectangle(306, 649, 640, 140);

    taunt_box->render();
  }


  if (game_mode == k_end_mode) {
    graphics->fadeInOut(0.0f, 1.0f, 0.1666f);
    graphics->color(1.0f, 1.0f, 1.0f, 1.0);

    textures->setTexture("large_salmon_star");
    graphics->rectangle(hot_config->getInt("player_1_end_score_box_x") - 52, hot_config->getInt("player_1_end_score_box_y") - 55, 128, 128);

    textures->setTexture("large_purple_star");
    graphics->rectangle(hot_config->getInt("player_2_end_score_box_x") - 52, hot_config->getInt("player_2_end_score_box_y") - 55, 128, 128);

    end_mode_box->render();
  } else {
    textures->setTexture("large_salmon_star");
    graphics->rectangle(0, k_screen_height - 120, 128, 128);

    textures->setTexture("large_purple_star");
    graphics->rectangle(k_screen_width - 120, k_screen_height - 120, 128, 128);
  }

  player_1_score_box->render();
  player_2_score_box->render();

  // render coordinates
  // int coord_x = 40;
  // int coord_y = k_screen_height - 79 - 40;
  // textures->setTexture("coordinates");
  // graphics->rectangle(coord_x, coord_y, 93, 79);



  graphics->end2DDraw();
}

void Game::renderBackground() {
  graphics->start2DDraw();

  // background render
  textures->setTexture("clouds");
  graphics->rectangle(0, 0, k_screen_width, k_screen_height);

  graphics->end2DDraw();
}


// This huge method puts all the crap on the game board.
bool Game::initializeGamePieces() {

  XMLDocument doc;
  XMLError result = doc.LoadFile(map_file.c_str());
  XMLCheckResult(result);

  // Music
  this->music = doc.FirstChildElement("music")->GetText();
  printf("Music: %s\n", this->music.c_str());
  
  // Theme
  this->theme = doc.FirstChildElement("theme")->GetText();
  printf("Theme: %s\n", this->theme.c_str());

  if (theme == "water") {
    theme_tile = model_cache->getModel("huge_water_tile.obj");

    // uncomment to add the water tile as a physics floor
    // physics->addMesh(theme_tile->getMeshAsTriangles(), new Point(50, 0, 0), M_PI);
  }

  doc.FirstChildElement("bpm")->QueryFloatText(&bpm);

  // Level Shape
  XMLElement* level_shape = doc.FirstChildElement("shape");

  int player_1_starts = 0;
  int player_2_starts = 0;
  int total_starts = 0;

  characters.resize(player_1_bears.size() + player_2_bears.size());

  XMLElement * tile_element = level_shape->FirstChildElement("tile");
  while (tile_element != nullptr) {
    int x, y, z, grade;
    float r;
    tile_element->FirstChildElement("x")->QueryIntText(&x);
    tile_element->FirstChildElement("y")->QueryIntText(&y);
    tile_element->FirstChildElement("z")->QueryIntText(&z);
    tile_element->FirstChildElement("r")->QueryFloatText(&r);
    std::string tile_type(tile_element->Attribute("type"));

    
    Hazard* hazard;
    if (tile_type == "wicket") {
      Wicket* wicket = new Wicket(tile_type, physics,
        new Point(x, y, z), M_PI + r);
      wicket->bpm = bpm;
      hazard = (Hazard*) wicket;
      hazards.push_front(hazard);
      wickets.push_back(wicket);
    } else if (tile_type == "free_star") {
      FreeStar* free_star = new FreeStar(tile_type, physics,
        new Point(x, y, z), M_PI + r);
      free_star->bpm = bpm;
      free_star->setFloatingHeight(0.75f);
      hazard = (Hazard*) free_star;
      hazards.push_front(hazard);
      free_stars.push_back(free_star);
    } else if (tile_type == "last_wicket") {
      Wicket* wicket = new Wicket(tile_type, physics,
        new Point(x, y, z), M_PI + r);
      wicket->bpm = bpm;
      hazard = (Hazard*) wicket;
      hazards.push_front(hazard);
      last_wicket = wicket;
      last_wicket->value = 9;
      last_wicket->wicket_value_text->setText("9");
      last_wicket->active = false;
    } else if (tile_type == "boxguy") {
      BoxGuy* boxguy = new BoxGuy(tile_type, physics,
        new Point(x, y, z), M_PI + r);
      boxguy->bpm = bpm;
      hazard = (Hazard*) boxguy;
      hazards.push_front(hazard);
    } else {
      hazard = new Hazard(tile_type, physics,
        new Point(x, y, z), M_PI + r);
      hazards.push_front(hazard);
    }

    if (tile_type == "player_1_start") {
      std::string model_name = "";

      Character* character = new Character(physics, new Point(x + hot_config->getInt("x_drop"), y + hot_config->getInt("y_drop"), z + k_character_drop_height), model_name); // 
      physics->setRotation(character->identity, 0, 0, character->default_shot_rotation);

      starts.push_back(hazard);

      character->roster_number = player_1_starts * 2;
      character->player_number = 1;
      characters[player_1_starts * 2] = character;
      player_1_starts += 1;
      total_starts +=1;

    } else if (tile_type == "player_2_start") {
      std::string model_name = "teddy_2.obj";

      Character* character = new Character(physics, new Point(x + hot_config->getInt("x_drop"), y + hot_config->getInt("y_drop"), z + k_character_drop_height), model_name); // 
      physics->setRotation(character->identity, 0, 0, character->default_shot_rotation);

      starts.push_back(hazard);
      
      character->roster_number = player_2_starts * 2 + 1;
      character->player_number = 2;
      characters[player_2_starts * 2 + 1] = character;
      player_2_starts += 1;
      total_starts += 1;
    }

    tile_element = tile_element->NextSiblingElement("tile");
  }

  if (total_starts < player_1_bears.size() + player_2_bears.size()) {
    characters.resize(total_starts);
  }
  if (player_1_starts < player_1_bears.size()) {
    player_1_bears.resize(player_1_starts);
  }
  if (player_2_starts < player_2_bears.size()) {
    player_2_bears.resize(player_2_starts);
  }
  current_character_number = 0;
  current_character = characters[current_character_number];

  return true;
}

bool Game::initialize() {
  graphics->initialize();

  // Initialize Textures
  if (!initializeTextures()) {
    printf("Unable to initialize textures!\n");
    return false;
  }

  control_map = new ControlMap();

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

  textures->addTexture("shot_power_outline", "ShotPowerOutline.png");
  textures->addTexture("shot_power_fill", "ShotPowerFill.png");
  
  textures->addTexture("clouds", "clouds_soft.png");
  textures->addTexture("water", "water.png");

  textures->addTexture("player_1_HUD_background", "player_1_HUD_background.png");
  textures->addTexture("player_2_HUD_background", "player_2_HUD_background.png");

  textures->addTexture("large_salmon_star", "large_salmon_star.png");
  textures->addTexture("large_purple_star", "large_purple_star.png");

  textures->addTexture("bear_selection_box", "bear_selection_box.png");

  textures->addTexture("selection_2", "selection_2.png");  

  textures->addTexture("speech_bubble", "speech_bubble.png");  

  textures->addTexture("lil_jon_box", "lil_jon_box.png");
  textures->addTexture("mortimer_box", "mortimer_box.png");
  textures->addTexture("gluke_box", "gluke_box.png");
  textures->addTexture("mags_box", "mags_box.png");
  textures->addTexture("bob_smith_box", "bob_smith_box.png");
  textures->addTexture("lord_lonsdale_box", "lord_lonsdale_box.png");
  textures->addTexture("hpf_swinnerton_dyer_box", "hpf_swinnerton_dyer_box.png");
  textures->addTexture("jeff_bridges_box", "jeff_bridges_box.png");
  textures->addTexture("grim_box", "grim_box.png");

  textures->addTexture("flat_shot_glyph", "flat_shot_glyph.png");
  textures->addTexture("up_shot_glyph", "up_shot_glyph.png");
  textures->addTexture("mallet_glyph", "mallet_glyph.png");
  textures->addTexture("mallet_background_glyph", "mallet_background_glyph.png");
  textures->addTexture("special_power", "special_power.png");
  textures->addTexture("taunt", "taunt.png");

  player_1_score_box = new TextBox(hot_config->getString("wicket_font"), hot_config->getInt("wicket_font_size"),
    std::to_string(player_1_display_score), 0, 0, 0, hot_config->getInt("player_1_score_box_x"), hot_config->getInt("player_1_score_box_y"));
  player_2_score_box = new TextBox(hot_config->getString("wicket_font"), hot_config->getInt("wicket_font_size"),
    std::to_string(player_2_display_score), 0, 0, 0, hot_config->getInt("player_2_score_box_x"), hot_config->getInt("player_2_score_box_y"));

  end_mode_box = new TextBox(hot_config->getString("wicket_font"), hot_config->getInt("end_mode_font_size"),
    "aok", 0, 0, 0, hot_config->getInt("end_mode_box_x"), hot_config->getInt("end_mode_box_y"));

  taunt_box = new TextBox(hot_config->getString("wicket_font"), 40,
    "g", 0, 0, 0, 340, 692);

  // TO DO: if this gets brought back for another reason, redo it as a vector so it can have variable size
  // bear_velocity_1 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
  //   hot_config->getInt("player_1_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + hot_config->getInt("v_y"));
  // bear_velocity_2 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
  //   hot_config->getInt("player_2_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + hot_config->getInt("v_y"));
  // bear_velocity_3 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
  //   hot_config->getInt("player_1_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + hud_step + hot_config->getInt("v_y"));
  // bear_velocity_4 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
  //   hot_config->getInt("player_2_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + hud_step + hot_config->getInt("v_y"));
  // bear_velocity_5 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
  //   hot_config->getInt("player_1_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + 2 * hud_step + hot_config->getInt("v_y"));
  // bear_velocity_6 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
  //   hot_config->getInt("player_2_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + 2 * hud_step + hot_config->getInt("v_y"));

  return true;
}

void Game::shutdown() {
  physics->shutdown();
  delete physics;
}
