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
  quit = false;
  current_screen = k_2p_game_screen;

  zoom = k_default_zoom;
  default_speed_ramping = k_default_speed_ramping;
  simulation_speed = k_default_minimum_speed;

  this->player_1_bears = player_1_bears;
  this->player_2_bears = player_2_bears;

  sway = 0;

  hud_step = hot_config->getInt("hud_step");

  player_1_score = 0;
  player_1_display_score = 0;
  player_2_score = 0;
  player_2_display_score = 0;

  game_mode = k_drop_mode;

  //map_file = "Levels/wawa_shores_1.txt";
  map_file = "Levels/test_level_1.txt";
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
  unsigned long current_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  physics->update(simulation_speed * (current_time - last_time) / 1000.0f);
  last_time = current_time;
  if (simulation_speed > k_default_minimum_speed) {
    simulation_speed *= 0.98f;
  }

  if (player_1_display_score < player_1_score) {
    player_1_display_score += 1;
  } else if (player_1_display_score > player_1_score) {
    player_1_display_score -= 1;
  }
  player_1_score_box->setText(std::to_string(player_1_display_score));
  player_1_score_box->x = hot_config->getInt("player_1_score_box_x");
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
  if (player_2_display_score >= 10) {
    player_2_score_box->x = player_2_score_box->x - (int) (hot_config->getInt("wicket_font_size") / 4);
  }
  if (player_2_display_score >= 100) {
    player_2_score_box->x = player_2_score_box->x - (int) (hot_config->getInt("wicket_font_size") / 4);
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

  // Check whether in prep mode, and if so, manually set the position and keep the ball active
  // in the physics environment. I believe this is necessary to allow the turns to keep happening.
  if (!physics->checkActive(current_character->identity) && game_mode == k_prep_mode) {
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

    // uncomment this to force a weak shot if the player doesn't activate the power gauge.
    // if (current_character->shot_power < 0.1f) {
    //   current_character->shot_power = k_default_shot_power / 10.0f;
    //   shoot();
    // }
  }

  // Check whether action has finished, and if so, switch to drop mode
  if (!physics->checkActive(current_character->identity) && game_mode == k_action_mode) {
    game_mode = k_drop_mode;
    physics->stop(current_character->identity);
    current_character->shot_rotation = current_character->default_shot_rotation;
    physics->setPositionAndRotation(current_character->identity,
      new Point(current_character->position->x, current_character->position->y, current_character->position->z + k_character_drop_height),
      0, 0, current_character->default_shot_rotation);

    // update to the next character and activate.
    current_character_number += 1;
    if (current_character_number > 5) {
      current_character_number = 0;
    }
    current_character = characters[current_character_number];
    physics->activate(current_character->identity);
  }

  for (auto character = characters.begin(); character != characters.end(); ++character) {
    // Update the character position (for rendering and game logic) from physics
    (*character)->updateFromPhysics();

    // If the character has slowed to a crawl, stop the character
    if (physics->getVelocity((*character)->identity) < 0.02f) {
      physics->stop((*character)->identity);
    }

    // If the character has fallen off the world, reset the character to its starting place
    if ((*character)->position->z < -10) {
      Hazard* start = starts[(*character)->roster_number];
      physics->stop((*character)->identity);
      physics->setPositionAndRotation((*character)->identity,
        new Point(start->position->x + 3, start->position->y + 3, start->position->z + k_character_drop_height),
        0, 0, (*character)->default_shot_rotation);
      if ((*character)->roster_number == current_character_number) {
        game_mode = k_drop_mode;

        // update to the next character and activate.
        current_character_number += 1;
        if (current_character_number > 5) {
          current_character_number = 0;
        }
        current_character = characters[current_character_number];
        physics->activate(current_character->identity);
      }
    }

    // Test character against wickets.
    if ((*character)->position_history.size() > 1) {
      for (auto wicket = wickets.begin(); wicket != wickets.end(); ++wicket) {
        (*wicket)->flipWicket((*character)->position_history[0], (*character)->position_history[1], (*character)->player_number);
      }
    }
  }

  // Calculate scores based on wickets
  player_1_score = 0;
  player_2_score = 0;
  for (auto wicket = wickets.begin(); wicket != wickets.end(); ++wicket) {
    int value = (*wicket)->value;
    int player_owner = (*wicket)->player_owner;
    if (player_owner == 1) {
      player_1_score += value;
    } else if (player_owner == 2) {
      player_2_score += value;
    }
  }

  std::stringstream stream;
  std::string s;

  stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[0]->identity);
  bear_velocity_1->setText(stream.str());
  stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[1]->identity);
  bear_velocity_2->setText(stream.str());
  stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[2]->identity);
  bear_velocity_3->setText(stream.str());
  stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[3]->identity);
  bear_velocity_4->setText(stream.str());
  stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[4]->identity);
  bear_velocity_5->setText(stream.str());
  stream.str(std::string()); stream.clear(); stream << std::fixed << std::setprecision(2) << physics->getVelocity(characters[5]->identity);
  bear_velocity_6->setText(stream.str());
  
}

void Game::afterUpdate() {

  // This is the current way to check for a break out of drop mode
  // To do: fix this so it doesn't break the drop action, but also doesn't
  // go back to long, awkward drops.
  if (game_mode == k_drop_mode && (!physics->checkActive(current_character->identity) ||
    physics->getVelocity(current_character->identity) < 0.00001)) {
    game_mode = k_prep_mode;
    current_character->up_shot = false;
    current_character->setShotRotation(current_character->default_shot_rotation, false);
  }
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
  std::string value = control_map->translateKeyEvent(e);

  for (auto item = control_map->ordered_controls.begin(); item != control_map->ordered_controls.end(); ++item) {
    if (control_map->control_map[*item] == value) {
      handleAction(*item);
    }
  }
}


// Handle actions as translated from the control map
void Game::handleAction(std::string action) {

// if (action == "player_1_down") {
//       bear_choice += 3;
//       if (bear_choice > 8) bear_choice -= 9;
//     } else if (action == "player_1_up") {
//       bear_choice -= 3;
//       if (bear_choice < 0) bear_choice += 9;
//     } else if (action == "player_1_left") {
//       bear_choice -= 1;
//       if ((bear_choice + 3) % 3 == 2) bear_choice += 3;
//     } else if (action == "player_1_right") {
//       bear_choice += 1;
//       if (bear_choice % 3 == 0) bear_choice -= 3;
//     } else if (action == "player_1_shoot_accept") {
//       if (availab

  if (game_mode == k_prep_mode) {
    if ((action == "player_1_right" && current_character_number % 2 == 0) ||
        ((action == "player_2_right" && current_character_number % 2 == 1))) {
      printf("Right, I read you\n");
      current_character->setShotRotation(current_character->shot_rotation + M_PI / 25, false);
    }

    if ((action == "player_1_left" && current_character_number % 2 == 0) ||
        ((action == "player_2_left" && current_character_number % 2 == 1))) {
      printf("Left, I read you\n");
      current_character->setShotRotation(current_character->shot_rotation - M_PI / 25, false);
    }

    if ((action == "player_1_up" && current_character_number % 2 == 0) ||
        ((action == "player_2_up" && current_character_number % 2 == 1))) {
      current_character->up_shot = true;
      current_character->setShotRotation(current_character->shot_rotation, false);
    }

    if ((action == "player_1_down" && current_character_number % 2 == 0) ||
        ((action == "player_2_down" && current_character_number % 2 == 1))) {
      current_character->up_shot = false;
      current_character->setShotRotation(current_character->shot_rotation, false);
    }

    if ((action == "player_1_shoot_accept" && current_character_number % 2 == 0) ||
        ((action == "player_2_shoot_accept" && current_character_number % 2 == 1))) {
      game_mode = k_power_mode;
      current_character->shot_power = 0;
      shot_rising = true;
      return;
    }
  }

  if (game_mode == k_power_mode) {
    if ((action == "player_1_shoot_accept" && current_character_number % 2 == 0) ||
        ((action == "player_2_shoot_accept" && current_character_number % 2 == 1))) {
      game_mode = k_action_mode;
      simulation_speed = default_speed_ramping;
      current_character->future_positions.clear();
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
      physics->activate(current_character->identity);
    }

    if ((action == "player_1_cancel_switch" && current_character_number % 2 == 0) ||
        ((action == "player_2_cancel_switch" && current_character_number % 2 == 1))) {
      game_mode = k_prep_mode;
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
  graphics->standardCamera(current_character->position->x + 15, current_character->position->y + 15, 10,
    current_character->position->x, current_character->position->y, 0);

  graphics->setLightPosition(50, 0, 15);

  // render hazards (this should be something different, like components)
  for (auto hazard = hazards.begin(); hazard != hazards.end(); ++hazard) {
    (*hazard)->render();
  }

  // render characters (balls) (i need to work on my naming)
  for (auto character = characters.begin(); character != characters.end(); ++character) {
    (*character)->render(0);
  }

  // Wicket info
  for (auto wicket = wickets.begin(); wicket != wickets.end(); ++wicket) {
    (*wicket)->setRenderInfo();
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

  textures->setTexture("player_1_HUD_background");
  graphics->rectangle(0, 0, 120, k_screen_height);

  textures->setTexture("player_2_HUD_background");
  graphics->rectangle(k_screen_width - 120, 0, 120, k_screen_height);

  textures->setTexture(player_1_bears[0] + "_box");
  graphics->rectangle(hot_config->getInt("player_1_x_margin"), hot_config->getInt("y_margin"), 103, 103);
  textures->setTexture(player_1_bears[1] + "_box");
  graphics->rectangle(hot_config->getInt("player_1_x_margin"), hot_config->getInt("y_margin") + hud_step, 103, 103);
  textures->setTexture(player_1_bears[2] + "_box");
  graphics->rectangle(hot_config->getInt("player_1_x_margin"), hot_config->getInt("y_margin") + 2 * hud_step, 103, 103);
  
  textures->setTexture(player_2_bears[0] + "_box");
  graphics->rectangle(hot_config->getInt("player_2_x_margin"), hot_config->getInt("y_margin"), 103, 103);
  textures->setTexture(player_2_bears[1] + "_box");
  graphics->rectangle(hot_config->getInt("player_2_x_margin"), hot_config->getInt("y_margin") + hud_step, 103, 103);
  textures->setTexture(player_2_bears[2] + "_box");
  graphics->rectangle(hot_config->getInt("player_2_x_margin"), hot_config->getInt("y_margin") + 2 * hud_step, 103, 103);

  textures->setTexture("bear_selection_box");
  graphics->rectangle(hot_config->getInt("player_" + std::to_string(current_character_number % 2 + 1) + "_x_margin"), hot_config->getInt("y_margin") + ((int) current_character_number / 2) * hud_step, 103, 103);

  bear_velocity_1->render();
  bear_velocity_2->render();
  bear_velocity_3->render();
  bear_velocity_4->render();
  bear_velocity_5->render();
  bear_velocity_6->render();

  int act_margin = hot_config->getInt("player_1_x_margin");
  int taunt_margin = hot_config->getInt("player_2_x_margin");
  if (current_character_number % 2 == 1) {
    act_margin = hot_config->getInt("player_2_x_margin");
    taunt_margin = hot_config->getInt("player_1_x_margin");
  }

  if (game_mode == k_prep_mode || game_mode == k_power_mode || game_mode == k_action_mode) {
    if(current_character->up_shot) {
      textures->setTexture("up_shot_glyph");
    } else {
      textures->setTexture("flat_shot_glyph");
    }
    graphics->rectangle(act_margin, hot_config->getInt("y_margin") + 3 * hud_step, 103, 103);

    textures->setTexture("mallet_background_glyph");
    graphics->rectangle(act_margin, hot_config->getInt("y_margin") + 4 * hud_step, 103, 103);

    textures->setTexture("taunt");
    graphics->rectangle(taunt_margin, hot_config->getInt("y_margin") + 3 * hud_step, 103, 103);    
  }

  // render power mode
  if (game_mode == k_power_mode || game_mode == k_action_mode) {
    textures->setTexture("mallet_glyph");
    graphics->pushModelMatrix();
    graphics->translate(act_margin + 36, hot_config->getInt("y_margin") + 4 * hud_step + 21, 0);
    graphics->rotate(-90.0 * current_character->shot_power / current_character->default_shot_power, 0.0f, 0.0f, 1.0f);
    //graphics->printModel();
    graphics->rectangle(-36, -21, 103, 103);
    graphics->popModelMatrix();
  }

  if (game_mode == k_action_mode) {
    textures->setTexture("special_power");
    graphics->rectangle(act_margin, hot_config->getInt("y_margin") + 5 * hud_step, 103, 103);
  }

  textures->setTexture("large_salmon_star");
  graphics->rectangle(0, k_screen_height - 120, 128, 128);

  textures->setTexture("large_purple_star");
  graphics->rectangle(k_screen_width - 120, k_screen_height - 120, 128, 128);

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

  // Level Shape
  XMLElement* level_shape = doc.FirstChildElement("shape");

  int alternating_starts = 0;

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
      hazard = (Hazard*) wicket;
      hazards.push_front(hazard);
      wickets.push_back(wicket);
    } else {
      hazard = new Hazard(tile_type, physics,
        new Point(x, y, z), M_PI + r);
      hazards.push_front(hazard);
    }

    if (tile_type == "start") {
      std::string model_name = "";
      if (alternating_starts % 2 == 1) model_name = "teddy_2.obj";
      Character* character = new Character(physics, new Point(x + hot_config->getInt("x_drop"), y + hot_config->getInt("y_drop"), z + k_character_drop_height), model_name); // 
      physics->setRotation(character->identity, 0, 0, character->default_shot_rotation);
      characters.push_back(character);

      starts.push_back(hazard);
      character->roster_number = alternating_starts;
      character->player_number = alternating_starts % 2 + 1;

      alternating_starts +=1;
    }

    tile_element = tile_element->NextSiblingElement("tile");
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

  bear_velocity_1 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
    hot_config->getInt("player_1_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + hot_config->getInt("v_y"));
  bear_velocity_2 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
    hot_config->getInt("player_2_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + hot_config->getInt("v_y"));
  bear_velocity_3 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
    hot_config->getInt("player_1_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + hud_step + hot_config->getInt("v_y"));
  bear_velocity_4 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
    hot_config->getInt("player_2_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + hud_step + hot_config->getInt("v_y"));
  bear_velocity_5 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
    hot_config->getInt("player_1_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + 2 * hud_step + hot_config->getInt("v_y"));
  bear_velocity_6 = new TextBox("alien_planet.ttf", hot_config->getInt("velocity_font_size"), "000", 0, 0, 0,
    hot_config->getInt("player_2_x_margin") + hot_config->getInt("v_x"), hot_config->getInt("y_margin") + 2 * hud_step + hot_config->getInt("v_y"));
  return true;
}

void Game::shutdown() {
  physics->shutdown();
  delete physics;
}
