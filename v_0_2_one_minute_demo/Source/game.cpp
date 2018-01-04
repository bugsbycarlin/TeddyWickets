/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "game.h"

// Game constructor
Game::Game() {
  quit = false;
  game_mode = k_drop_mode;
  mousedown = false;
  zoom = k_default_zoom;
  drag_x = 0;
  drag_y = 0;
  default_speed_ramping = k_default_speed_ramping;
  simulation_speed = k_default_minimum_speed;
  current_screen = k_1p_game_screen;
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
    } else if (e.type == SDL_TEXTINPUT) {
      handleKeys(e.text.text[0]);
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
void Game::handleKeys(unsigned char key) {
  if (key == 'p') {
    quit = true;
  }

  if (key == 'z') {
    zoom += 1.0f;
  } else if (key == 'x') {
    zoom -= 1.0f;
  }  

  if (game_mode == k_prep_mode) {
    if (key == 'a') {
      character->setShotRotation(character->shot_rotation + 3.14159 / 20, true);
    }

    if (key == 'd') {
      character->setShotRotation(character->shot_rotation - 3.14159 / 20, true);
    }

    if (key == 'w') {
      character->up_shot = true;
      character->setShotRotation(character->shot_rotation, true);
    }

    if (key == 's') {
      character->up_shot = false;
      character->setShotRotation(character->shot_rotation, true);
    }

    if (key == 'm') {
      game_mode = k_power_mode;
      character->shot_power = 0;
      shot_rising = true;
      return;
    }
  }

  if (game_mode == k_power_mode) {
    if (key == 'm') {
      shoot();
    }

    if (key == 'c') {
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
  // Simple viewport.
  glViewport(0, 0, k_screen_width, k_screen_height);

  // Clear color buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderBackground();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Set one of these perspectives:

  // 1. this is cool, super zoomed out
  // glOrtho(-50, 50, -50, 50, -50, 50);

  // 2. weirdly reasonable ortho
  glOrtho(-zoom * k_aspect_ratio, zoom * k_aspect_ratio, -zoom, zoom, -10 * zoom, 10 * zoom);
  
  // 3. normal perspective
  //gluPerspective(45.0f,k_screen_width/(1.0 * k_screen_height),0.1f,1000.0f);

  // Simple Opengl Lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat ambient[] = {0.5, 0.5, 0.5, 1.0};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_NORMALIZE);
  glEnable(GL_RESCALE_NORMAL);

  GLfloat light_ambient[] = {0.5, 0.5, 0.5, 1.0};
  GLfloat light_diffuse[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_position[] = {-1.0, 0.0, 1.0, 0.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // Set the camera to look down at the character.
  // For fun, change the z-value to change the viewing angle of the game.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(character->position->x - 15, character->position->y - 15, 10,
            character->position->x, character->position->y, 0,
            0, 0, 1);

  glColor4f(1.0, 1.0, 1.0, 1.0);

  // render surfaces (walls and floors)
  for (auto surface = surfaces.begin(); surface != surfaces.end(); ++surface) {
    (*surface)->render();
  }

  // render bumpers
  for (auto bumper = bumpers.begin(); bumper != bumpers.end(); ++bumper) {
    (*bumper)->render();
  }

  // render wickets
  for (auto wicket = wickets.begin(); wicket != wickets.end(); ++wicket) {
    (*wicket)->render();
  }

  // render the character
  character->render(game_mode);

  // render 2D overlay
  Start2DDraw();

  int info_x = 324;
  int info_y = 20;

  // render shot prep infographic
  if (game_mode == k_prep_mode) {
    textures->setTexture("m_prep_info");
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(info_x + 0.0, info_y + 0.0);
    glTexCoord2d(0.0, 1.0); glVertex2d(info_x + 0.0, info_y + 400);
    glTexCoord2d(1.0, 1.0); glVertex2d(info_x + 400, info_y + 400);
    glTexCoord2d(1.0, 0.0); glVertex2d(info_x + 400, info_y + 0.0);
    glEnd();
  }

  // render power mode
  if (game_mode == k_power_mode) {
    // render power mode infographic
    textures->setTexture("m_shot_info");
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(info_x + 0.0, info_y + 0.0);
    glTexCoord2d(0.0, 1.0); glVertex2d(info_x + 0.0, info_y + 400);
    glTexCoord2d(1.0, 1.0); glVertex2d(info_x + 400, info_y + 400);
    glTexCoord2d(1.0, 0.0); glVertex2d(info_x + 400, info_y + 0.0);
    glEnd();

    // render power gauge outline
    int power_x = 974;
    int power_y = 20;
    textures->setTexture("shot_power_outline");
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(power_x + 0.0, power_y + 0.0);
    glTexCoord2d(0.0, 1.0); glVertex2d(power_x + 0.0, power_y + 150);
    glTexCoord2d(1.0, 1.0); glVertex2d(power_x + 30, power_y + 150);
    glTexCoord2d(1.0, 0.0); glVertex2d(power_x + 30, power_y + 0.0);
    glEnd();

    // render power gauge fill
    float portion = character->shot_power / character->default_shot_power;
    textures->setTexture("shot_power_fill");
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 1.0 - 140.0 / 150.0 * portion); glVertex2d(power_x + 0.0, power_y + 150 - 140 * portion);
    glTexCoord2d(0.0, 1.0); glVertex2d(power_x + 0.0, power_y + 150);
    glTexCoord2d(1.0, 1.0); glVertex2d(power_x + 30, power_y + 150);
    glTexCoord2d(1.0, 1.0 - 140.0 / 150.0 * portion); glVertex2d(power_x + 30, power_y + 150 - 140 * portion);
    glEnd();
  }

  End2DDraw();
}

void Game::renderBackground() {
  Start2DDraw();

  // background render
  textures->setTexture("clouds");
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
  glTexCoord2d(0.0, 1.0); glVertex2d(0.0, k_screen_height);
  glTexCoord2d(1.0, 1.0); glVertex2d(k_screen_width, k_screen_height);
  glTexCoord2d(1.0, 0.0); glVertex2d(k_screen_width, 0.0);
  glEnd();

  End2DDraw();
}

// This huge method puts all the crap on the game board.
bool Game::initializeGamePieces() {
  character = new Character(physics, textures, new Point(0, 0, k_character_drop_height));
  physics->setRotation(character->identity, 0, 0, character->default_shot_rotation);

  // first test wicket
  wickets.push_front(new Wicket(physics, textures,
    new Point(0, -6, 0),
    new Point(-3, -6, 0),
    4.0));

  // second test wicket
  wickets.push_front(new Wicket(physics, textures,
    new Point(-21, 0, -4),
    new Point(-21, -3, -4),
    4.0));

  // third test wicket
  wickets.push_front(new Wicket(physics, textures,
    new Point(-54, 0, -8),
    new Point(-54, -3, -8),
    4.0));

  // fourth test wicket
  wickets.push_front(new Wicket(physics, textures,
    new Point(-48, -9, -8),
    new Point(-51, -9, -8),
    4.0));

  // fifth test wicket
  wickets.push_front(new Wicket(physics, textures,
    new Point(-45, 45, 0),
    new Point(-45, 48, 0),
    4.0));

  // surfaces segment 1 (slide down)
  int height_array[] = {0, 0, 0, -2, -4, -4, -4, -6, -8};
  for (int k = 0; k < 8; k++) {
    for (int y = 0; y < 3; y++) {
      surfaces.push_front(new Surface(physics, textures,
        false,
        new Point(0 - 6 * k, 0 - 6 * y, height_array[k+1]),
        new Point(6 - 6 * k, 0 - 6 * y, height_array[k]),
        new Point(6 - 6 * k, 6 - 6 * y, height_array[k]),
        new Point(0 - 6 * k, 6 - 6 * y, height_array[k+1])));
    }
  }

  // bumper segment 1
  bumpers.push_front(new Bumper(physics, textures,
    new Point(6, -12, 0),
    new Point(-6, -12, 0),
    new Point(0, k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-6, -12, 0),
    new Point(-18, -12, -4),
    new Point(0, k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-18, -12, -4),
    new Point(-30, -12, -4),
    new Point(0, k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-30, -12, -4),
    new Point(-42, -12, -8),
    new Point(0, k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(6, 6, 0),
    new Point(-6, 6, 0),
    new Point(0, -k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-6, 6, 0),
    new Point(-18, 6, -4),
    new Point(0, -k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-18, 6, -4),
    new Point(-30, 6, -4),
    new Point(0, -k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-30, 6, -4),
    new Point(-42, 6, -8),
    new Point(0, -k_bumper_width, 0)));

  // mountain segment 1
  int first_x = 10 + rand() % static_cast<int>(20 + 1);
  int last_y = 25 + rand() % static_cast<int>(25 + 1);
  for (int k = 0; k < 8; k++) {
    int new_y = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics, textures,
      true,
      new Point(0 - 6 * k, 0 - (6 * 2) - new_y, height_array[k+1] - 70),
      new Point(6 - 6 * k + first_x, 0 - (6 * 2) - last_y, height_array[k] - 70),
      new Point(6 - 6 * k, 0 - (6 * 2), height_array[k]),
      new Point(0 - 6 * k, 0 - (6 * 2), height_array[k+1])));
    last_y = new_y;
    first_x = 0;
  }

  // surfaces segment 2 (corner)
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      surfaces.push_front(new Surface(physics, textures,
        false,
        new Point(-48 - 6 * x, 0 - 6 * y, -8),
        new Point(-42 - 6 * x, 0 - 6 * y, -8),
        new Point(-42 - 6 * x, 6 - 6 * y, -8),
        new Point(-48 - 6 * x, 6 - 6 * y, -8)));
    }
  }

  // bumper segment 2 (corner)
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-60, -12, -8),
    new Point(-42, -12, -8),
    new Point(0, k_bumper_width, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-60, -12, -8),
    new Point(-60, 6, -8),
    new Point(k_bumper_width, 0.0, 0)));

  // mountain segment 2
  for (int k = 0; k < 3; k++) {
    int new_y = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics, textures,
      true,
      new Point(-48 - 6 * k, 0 - (6 * 2) - new_y, -8 - 70),
      new Point(-42 - 6 * k, 0 - (6 * 2) - last_y, -8 - 70),
      new Point(-42 - 6 * k, 0 - (6 * 2), -8),
      new Point(-48 - 6 * k, 0 - (6 * 2), -8)));
    last_y = new_y;
  }

  // mountain corner
  int last_x = 25 + rand() % static_cast<int>(25 + 1);
  surfaces.push_front(new Surface(physics, textures,
    true,
    new Point(-60 - last_x, 0 - (6 * 2) - last_y, -8 - 70),
    new Point(-60, 0 - (6 * 2) - last_y, -8 - 70),
    new Point(-60, 0 - (6 * 2), -8),
    new Point(-60, 0 - (6 * 2), -8)));

  // mountain segment 3
  for (int k = 0; k < 3; k++) {
    int new_x = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics, textures,
      true,
      new Point(-48 - 6 * 2 - new_x, -6 + 6 * k, -8 - 70),
      new Point(-48 - 6 * 2 - last_x, -12 + 6 * k - last_y, -8 - 70),
      new Point(-48 - 6 * 2, -12 + 6 * k, -8),
      new Point(-48 - 6 * 2, -6 + 6 * k, -8)));
    last_x = new_x;
    last_y = 0;
  }

  // surfaces segment 3 (slide back up)
  int height_array2[] = {-8, -6, -4, -4, -4, -2, 0, 0, 0};
  for (int k = 0; k < 8; k++) {
    for (int x = 0; x < 3; x++) {
      surfaces.push_front(new Surface(physics, textures,
        false,
        new Point(-48 - 6 * x, 6 + 6 * k, height_array2[k]),
        new Point(-42 - 6 * x, 6 + 6 * k, height_array2[k]),
        new Point(-42 - 6 * x, 12 + 6 * k, height_array2[k+1]),
        new Point(-48 - 6 * x, 12 + 6 * k, height_array2[k+1])));
    }
  }

  // bumper segment 3
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-60, 6, -8),
    new Point(-60, 18, -4),
    new Point(k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-60, 18, -4),
    new Point(-60, 30, -4),
    new Point(k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-60, 30, -4),
    new Point(-60, 42, 0),
    new Point(k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-60, 42, 0),
    new Point(-60, 54, 0),
    new Point(k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-42, 6, -8),
    new Point(-42, 18, -4),
    new Point(-k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-42, 18, -4),
    new Point(-42, 30, -4),
    new Point(-k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-42, 30, -4),
    new Point(-42, 42, 0),
    new Point(-k_bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics, textures,
    new Point(-42, 42, 0),
    new Point(-42, 54, 0),
    new Point(-k_bumper_width, 0.0, 0)));

  // mountain segment 4
  for (int k = 0; k < 8; k++) {
    int new_x = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics, textures,
      true,
      new Point(-48 - 6 * 2 - new_x, 12 + 6 * k, height_array2[k+1] - 70),
      new Point(-48 - 6 * 2 - last_x, 6 + 6 * k, height_array2[k] - 70),
      new Point(-48 - 6 * 2, 6 + 6 * k, height_array2[k]),
      new Point(-48 - 6 * 2, 12 + 6 * k, height_array2[k+1])));
    last_x = new_x;
  }

  return true;
}

bool Game::initialize() {
  // Initialize Textures
  if (!initializeTextures()) {
    printf("Unable to initialize textures!\n");
    return false;
  }

  // Initialize Lighting
  if (!initializeLighting()) {
    printf("Unable to initialize lighting!\n");
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

  return true;
}

bool Game::initializeTextures() {
  glEnable(GL_TEXTURE_2D);

  textures = new Textures();

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

  textures->addTexture("wicket", "barber_pole.png");

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

  return true;
}

// To do: lighting here instead of per render
bool Game::initializeLighting() {
  glViewport(0, 0, k_screen_width, k_screen_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, k_screen_width/(1.0 * k_screen_height), 0.1f, 1000.0f);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error initializing lighting! %s\n", gluErrorString(error));
    return false;
  }

  return true;
}

void Game::shutdown() {
  physics->shutdown();
  delete physics;

  // To do: release the image files.
  delete textures;
}
