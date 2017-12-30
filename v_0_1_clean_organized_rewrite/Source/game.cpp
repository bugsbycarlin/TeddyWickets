/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "game.h"

void Start2DDraw();
void End2DDraw();

// Game constructor
Game::Game() {
  quit = false;
  game_mode = DROP_MODE;
  mousedown = false;
  drag_x = 0;
  drag_y = 0;
}

void Game::game_loop(SDL_Window* window, FMOD::System *sound_system) {
  // Event handler
  SDL_Event e;

  // Enable text input
  SDL_StartTextInput();

  startTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  lastTime = startTime;

  // While application is running
  while (!quit) {
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

    postUpdate();
  }

  // Disable text input
  SDL_StopTextInput();

  shutdown();
}

void Game::postUpdate() {
  if (game_mode == DROP_MODE && (!physics->checkActive(character->identity) ||
    physics->getVelocity(character->identity) < 0.00001)) {
    game_mode = PREP_MODE;
    character->up_shot = false;
    character->setShotRotation(character->default_shot_rotation, true);
  }
}

void Game::update() {
  Point* lp = character->position;

  unsigned long currentTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  // if (game_mode != PREP_MODE) {
  //   printf("Diff %d Total %d Game_Mode %d\n", currentTime - lastTime, currentTime - startTime, game_mode);
  // }
  physics->update((currentTime - lastTime) / 1000.0f);
  lastTime = currentTime;

  for (auto bumper = bumpers.begin(); bumper != bumpers.end(); ++bumper) {
    (*bumper)->update();
    if (physics->hasCollision((*bumper)->identity, character->identity)) {
      (*bumper)->bump();
    }
  }

  if (!physics->checkActive(character->identity) && game_mode == ACTION_MODE) {
    game_mode = DROP_MODE;
    physics->stop(character->identity);
    character->shot_rotation = character->default_shot_rotation;
    physics->setPositionAndRotation(character->identity,
      new Point(character->position->x, character->position->y, character->position->z + 0.5f),
      0, 0, character->default_shot_rotation);
    physics->activate(character->identity);
  }

  if (!physics->checkActive(character->identity) && game_mode == PREP_MODE) {
    physics->setPositionAndRotation(character->identity,
      new Point(character->position->x, character->position->y, character->position->z),
      0, 0, character->shot_rotation);
    physics->activate(character->identity);
  }

  if (game_mode == POWER_MODE) {
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
    //   game_mode = ACTION_MODE;
    //   character->impulse(0.01 * sin(character->shot_rotation), -0.01 * cos(character->shot_rotation), 0.5);
    //   physics->activate(character->identity);
    // }
  }

  character->updateFromPhysics();
  Point* p = character->position;

  if (physics->getVelocity(character->identity) < 0.02f) {
    physics->stop(character->identity);
  }

  if (character->position->z < -50) {
    quit = true;
  }
}

void Game::render() {
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  // Clear color buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderBackground();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Set one of these perspectives:

  // this is cool, super zoomed out
  // glOrtho(-50, 50, -50, 50, -50, 50);

  // weirdly reasonable ortho
  glOrtho(-13.33, 13.33, -10, 10, -100, 100);

  // normal perspective
  // gluPerspective(45.0f,SCREEN_WIDTH/(1.0 * SCREEN_HEIGHT),0.1f,1000.0f);


  // Light
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat al[] = {0.5, 0.5, 0.5, 1.0};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, al);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_NORMALIZE);
  glEnable(GL_RESCALE_NORMAL);


  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();

  gluLookAt(character->position->x - 15, character->position->y - 15, 15,
            character->position->x, character->position->y, 0,
            0, 0, 1);

  GLfloat light_ambient[] = {0.5, 0.5, 0.5, 1.0};
  GLfloat light_diffuse[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_position[] = {-1.0, 0.0, 1.0, 0.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glColor4f(1.0, 1.0, 1.0, 1.0);
  for (auto surface = surfaces.begin(); surface != surfaces.end(); ++surface) {
    (*surface)->render();
  }

  for (auto bumper = bumpers.begin(); bumper != bumpers.end(); ++bumper) {
    (*bumper)->render();
  }

  for (auto wicket = wickets.begin(); wicket != wickets.end(); ++wicket) {
    (*wicket)->render();
  }

  character->render(game_mode);

  Start2DDraw();

  int info_x = 324;
  int info_y = 20;

  // prep info
  if (game_mode == PREP_MODE) {
    // prep info
    Textures::setTexture("m_prep_info");
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(info_x + 0.0, info_y + 0.0);
    glTexCoord2d(0.0, 1.0); glVertex2d(info_x + 0.0, info_y + 400);
    glTexCoord2d(1.0, 1.0); glVertex2d(info_x + 400, info_y + 400);
    glTexCoord2d(1.0, 0.0); glVertex2d(info_x + 400, info_y + 0.0);
    glEnd();
  }

  if (game_mode == POWER_MODE) {
    // power info
    Textures::setTexture("m_shot_info");
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(info_x + 0.0, info_y + 0.0);
    glTexCoord2d(0.0, 1.0); glVertex2d(info_x + 0.0, info_y + 400);
    glTexCoord2d(1.0, 1.0); glVertex2d(info_x + 400, info_y + 400);
    glTexCoord2d(1.0, 0.0); glVertex2d(info_x + 400, info_y + 0.0);
    glEnd();

    // outline
    int power_x = 974;
    int power_y = 20;
    Textures::setTexture("shot_power_outline");
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(power_x + 0.0, power_y + 0.0);
    glTexCoord2d(0.0, 1.0); glVertex2d(power_x + 0.0, power_y + 150);
    glTexCoord2d(1.0, 1.0); glVertex2d(power_x + 30, power_y + 150);
    glTexCoord2d(1.0, 0.0); glVertex2d(power_x + 30, power_y + 0.0);
    glEnd();

    // fill
    float portion = character->shot_power / character->default_shot_power;
    Textures::setTexture("shot_power_fill");
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
  Textures::setTexture("clouds");
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
  glTexCoord2d(0.0, 1.0); glVertex2d(0.0, SCREEN_HEIGHT);
  glTexCoord2d(1.0, 1.0); glVertex2d(SCREEN_WIDTH, SCREEN_HEIGHT);
  glTexCoord2d(1.0, 0.0); glVertex2d(SCREEN_WIDTH, 0.0);
  glEnd();

  End2DDraw();
}


void Game::handleMouse(SDL_Event e) {
  if (game_mode == PREP_MODE) {
    if (mousedown && e.type == SDL_MOUSEMOTION /*&& e.button.button == SDL_BUTTON_LEFT*/) {
      int x = 0, y = 0;
      SDL_GetMouseState(&x, &y);
      printf("%d %d\n", x, y);
      character->setShotRotation(pre_drag_rotation - (drag_x - x) / 50.0f, false);
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
      mousedown = true;
      SDL_GetMouseState(&drag_x, &drag_y);
      pre_drag_rotation = character->shot_rotation;
      printf("CLICK ENGAGED %d %d\n", drag_x, drag_y);
    } else if (e.type == SDL_MOUSEBUTTONUP /*&& e.button.button == SDL_BUTTON_LEFT*/) {
      mousedown = false;
      int x = 0, y = 0;
      SDL_GetMouseState(&x, &y);
      printf("CLICK RELEASED %d %d\n", x, y);
      character->setShotRotation(pre_drag_rotation - (drag_x - x) / 50.0f, true);
      // this is a cheap hack
      if (x == drag_x && y == drag_y) {
        game_mode = POWER_MODE;
        character->shot_power = 0;
        shot_rising = true;
      }
    }
  } else if (game_mode == POWER_MODE) {
    if (e.type == SDL_MOUSEBUTTONUP) {
      game_mode = ACTION_MODE;
      character->futurePositions.clear();
      if (character->up_shot) {
        // a 45 degree shot
        character->impulse(0.293 * character->shot_power * sin(character->shot_rotation),
          0.293 * -character->shot_power * cos(character->shot_rotation),
          0.707 * character->shot_power);
      } else {
        // a flat shot
        character->impulse(character->shot_power * sin(character->shot_rotation),
          -character->shot_power * cos(character->shot_rotation),
          0.5);
      }
      physics->activate(character->identity);
    }
  }
}


void Game::handleKeys(unsigned char key) {
  if (key == 'p') {
    quit = true;
  }

  if (game_mode == PREP_MODE) {
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
      game_mode = POWER_MODE;
      character->shot_power = 0;
      shot_rising = true;
      return;
    }
  }

  if (game_mode == POWER_MODE) {
    if (key == 'm') {
      game_mode = ACTION_MODE;
      character->futurePositions.clear();
      if (character->up_shot) {
        // a 45 degree shot
        character->impulse(0.293 * character->shot_power * sin(character->shot_rotation),
          0.293 * -character->shot_power * cos(character->shot_rotation),
          0.707 * character->shot_power);
      } else {
        // a flat shot
        character->impulse(character->shot_power * sin(character->shot_rotation),
          -character->shot_power * cos(character->shot_rotation),
          0.5);
      }
      physics->activate(character->identity);
    }

    if (key == 'c') {
      game_mode = PREP_MODE;
    }
  }
}

bool Game::initializeGameLogic() {
  character = new Character(physics, new Point(0, 0, 1.5f));
  physics->setRotation(character->identity, 0, 0, character->default_shot_rotation);

  float bumper_width = 0.5;

  // first test wicket
  wickets.push_front(new Wicket(physics,
    new Point(0, -6, 0),
    new Point(-3, -6, 0),
    4.0));

  // second test wicket
  wickets.push_front(new Wicket(physics,
    new Point(-21, 0, -4),
    new Point(-21, -3, -4),
    4.0));

  // third test wicket
  wickets.push_front(new Wicket(physics,
    new Point(-54, 0, -8),
    new Point(-54, -3, -8),
    4.0));

  // fourth test wicket
  wickets.push_front(new Wicket(physics,
    new Point(-48, -9, -8),
    new Point(-51, -9, -8),
    4.0));

  // fifth test wicket
  wickets.push_front(new Wicket(physics,
    new Point(-45, 45, 0),
    new Point(-45, 48, 0),
    4.0));

  // surfaces segment 1 (slide down)
  int height_array[] = {0, 0, 0, -2, -4, -4, -4, -6, -8};
  for (int k = 0; k < 8; k++) {
    for (int y = 0; y < 3; y++) {
      surfaces.push_front(new Surface(physics,
        false,
        new Point(0 - 6 * k, 0 - 6 * y, height_array[k+1]),
        new Point(6 - 6 * k, 0 - 6 * y, height_array[k]),
        new Point(6 - 6 * k, 6 - 6 * y, height_array[k]),
        new Point(0 - 6 * k, 6 - 6 * y, height_array[k+1])));
    }
  }

  // bumper segment 1
  bumpers.push_front(new Bumper(physics,
    new Point(6, -12, 0),
    new Point(-6, -12, 0),
    new Point(0, bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-6, -12, 0),
    new Point(-18, -12, -4),
    new Point(0, bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-18, -12, -4),
    new Point(-30, -12, -4),
    new Point(0, bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-30, -12, -4),
    new Point(-42, -12, -8),
    new Point(0, bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(6, 6, 0),
    new Point(-6, 6, 0),
    new Point(0, -bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-6, 6, 0),
    new Point(-18, 6, -4),
    new Point(0, -bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-18, 6, -4),
    new Point(-30, 6, -4),
    new Point(0, -bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-30, 6, -4),
    new Point(-42, 6, -8),
    new Point(0, -bumper_width, 0)));

  // mountain segment 1
  int first_x = 10 + rand() % static_cast<int>(20 + 1);
  int last_y = 25 + rand() % static_cast<int>(25 + 1);
  for (int k = 0; k < 8; k++) {
    int new_y = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics,
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
      surfaces.push_front(new Surface(physics,
        false,
        new Point(-48 - 6 * x, 0 - 6 * y, -8),
        new Point(-42 - 6 * x, 0 - 6 * y, -8),
        new Point(-42 - 6 * x, 6 - 6 * y, -8),
        new Point(-48 - 6 * x, 6 - 6 * y, -8)));
    }
  }

  // bumper segment 2 (corner)
  bumpers.push_front(new Bumper(physics,
    new Point(-60, -12, -8),
    new Point(-42, -12, -8),
    new Point(0, bumper_width, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-60, -12, -8),
    new Point(-60, 6, -8),
    new Point(bumper_width, 0.0, 0)));

  // mountain segment 2
  for (int k = 0; k < 3; k++) {
    int new_y = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics,
      true,
      new Point(-48 - 6 * k, 0 - (6 * 2) - new_y, -8 - 70),
      new Point(-42 - 6 * k, 0 - (6 * 2) - last_y, -8 - 70),
      new Point(-42 - 6 * k, 0 - (6 * 2), -8),
      new Point(-48 - 6 * k, 0 - (6 * 2), -8)));
    last_y = new_y;
  }

  // mountain corner
  int last_x = 25 + rand() % static_cast<int>(25 + 1);
  surfaces.push_front(new Surface(physics,
    true,
    new Point(-60 - last_x, 0 - (6 * 2) - last_y, -8 - 70),
    new Point(-60, 0 - (6 * 2) - last_y, -8 - 70),
    new Point(-60, 0 - (6 * 2), -8),
    new Point(-60, 0 - (6 * 2), -8)));

  // mountain segment 3
  for (int k = 0; k < 3; k++) {
    int new_x = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics,
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
      surfaces.push_front(new Surface(physics,
        false,
        new Point(-48 - 6 * x, 6 + 6 * k, height_array2[k]),
        new Point(-42 - 6 * x, 6 + 6 * k, height_array2[k]),
        new Point(-42 - 6 * x, 12 + 6 * k, height_array2[k+1]),
        new Point(-48 - 6 * x, 12 + 6 * k, height_array2[k+1])));
    }
  }

  // bumper segment 3
  bumpers.push_front(new Bumper(physics,
    new Point(-60, 6, -8),
    new Point(-60, 18, -4),
    new Point(bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-60, 18, -4),
    new Point(-60, 30, -4),
    new Point(bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-60, 30, -4),
    new Point(-60, 42, 0),
    new Point(bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-60, 42, 0),
    new Point(-60, 54, 0),
    new Point(bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-42, 6, -8),
    new Point(-42, 18, -4),
    new Point(-bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-42, 18, -4),
    new Point(-42, 30, -4),
    new Point(-bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-42, 30, -4),
    new Point(-42, 42, 0),
    new Point(-bumper_width, 0.0, 0)));
  bumpers.push_front(new Bumper(physics,
    new Point(-42, 42, 0),
    new Point(-42, 54, 0),
    new Point(-bumper_width, 0.0, 0)));

  // mountain segment 4
  for (int k = 0; k < 8; k++) {
    int new_x = 25 + rand() % static_cast<int>(25 + 1);
    surfaces.push_front(new Surface(physics,
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
  if (!initializeGameLogic()) {
    printf("Unable to initialize game logic!\n");
    return false;
  }

  return true;
}

bool Game::initializeTextures() {
  glEnable(GL_TEXTURE_2D);

  Textures::addTexture("tiles", "tiles4.png");

  Textures::addTexture("bumper", "bumper5.png");
  Textures::addTexture("lit_bumper", "bumper4.png");

  Textures::addTexture("bear_face", "bear_test2.png");
  Textures::addTexture("bear_nose", "nose.png");
  Textures::addTexture("bear_arms", "parts.png");

  Textures::addTexture("plain", "plain.png");
  Textures::addTexture("black", "black.png");

  Textures::addTexture("shot_power_outline", "ShotPowerOutline.png");
  Textures::addTexture("shot_power_fill", "ShotPowerFill.png");

  Textures::addTexture("clouds", "clouds.png");

  Textures::addTexture("left_turn_info", "A_Key_Turn_Left.png");
  Textures::addTexture("right_turn_info", "D_Key_Turn_Right.png");
  Textures::addTexture("m_prep_info", "M_Key_Prep_Your_Shot.png");
  Textures::addTexture("m_shot_info", "M_Again_Set_Power_And_Take_Shot.png");

  Textures::addTexture("wicket", "barber_pole.png");

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

bool Game::initializeLighting() {
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, SCREEN_WIDTH/(1.0 * SCREEN_HEIGHT), 0.1f, 1000.0f);

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
}

void Start2DDraw() {
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}

void End2DDraw() {
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}
