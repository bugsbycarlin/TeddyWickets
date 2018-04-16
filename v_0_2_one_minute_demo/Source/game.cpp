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

  sway = 0;

  game_mode = k_drop_mode;

  map_file = "Levels/wawa_shores_1.txt";
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
  }
  
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
  // for (auto wicket = wickets.begin(); wicket != wickets.end(); ++wicket) {
  //   (*wicket)->renderInfo();
  // }
  wickets[0]->setRenderInfo();

  // Render theme tile
  if (theme == "water") {
    graphics->pushModelMatrix();
    graphics->translate(50, 0 + sway, 0);
    theme_tile->render();
    graphics->popModelMatrix();
  }

  // render 2D overlay
  graphics->start2DDraw();

  wickets[0]->renderInfo();

  // render power mode
  if (game_mode == k_power_mode) {
    // render power gauge outline
    int power_x = 1340;
    int power_y = 20;
    textures->setTexture("shot_power_outline");
    graphics->rectangle(power_x, power_y, 30, 150);

    // render power gauge fill
    float portion = current_character->shot_power / current_character->default_shot_power;
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
      hazard = new Hazard(tile_type, physics,
        new Point(x, y, z), M_PI + r);
      hazards.push_front(hazard);
    } else {
      Wicket* wicket = new Wicket(tile_type, physics,
        new Point(x, y, z), M_PI + r);
      hazard = (Hazard*) wicket;
      hazards.push_front(hazard);
      wickets.push_back(wicket);
    }

    if (tile_type == "start") {
      std::string model_name = "";
      if (alternating_starts % 2 == 1) model_name = "teddy_2.obj";
      Character* character = new Character(physics, new Point(x + hot_config->getInt("x_drop"), y + hot_config->getInt("y_drop"), z + k_character_drop_height), model_name); // 
      physics->setRotation(character->identity, 0, 0, character->default_shot_rotation);
      characters.push_back(character);

      starts.push_back(hazard);
      character->roster_number = alternating_starts;

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

  return true;
}

void Game::shutdown() {
  physics->shutdown();
  delete physics;
}
