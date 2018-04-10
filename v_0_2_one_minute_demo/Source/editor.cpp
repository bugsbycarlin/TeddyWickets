/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "editor.h"

// Editor constructor
Editor::Editor() {
  current_screen = k_editor_screen;
}

// Editor loop. Main.cpp is running this on a loop until it's time to switch to a different part of the game.
void Editor::loop(SDL_Window* window, FMOD::System *sound_system) {
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

  // Update screen
  SDL_GL_SwapWindow(window);
}

void Editor::update() {
  // Set time and perform physics update
  unsigned long current_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  physics->update(k_default_minimum_speed * (current_time - last_time) / 1000.0f);
  last_time = current_time;

  if (current_time - framerate_time > 1000.0f) {
    printf("Framerate: %d\n", frames_since_last);
    framerate_time = current_time;
    frames_since_last = 0;
  } else {
    frames_since_last++;
  }
}


// Handle keyboard input
void Editor::handleKeys(SDL_Event e) {
}

void Editor::render() {
  graphics->clearScreen();
  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);

  renderBackground();

  graphics->set3d(k_default_zoom);
  graphics->enableLights();

  graphics->standardCamera(0 + 15, 0 + 15, 10,
    0, 0, 0);

  graphics->setLightPosition(50, 0, 15);

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

  // render 2D overlay
  graphics->start2DDraw();
  graphics->end2DDraw();
}

void Editor::renderBackground() {
  graphics->start2DDraw();

  // background render
  textures->setTexture("clouds");
  graphics->rectangle(0, 0, k_screen_width, k_screen_height);

  graphics->end2DDraw();
}

// This huge method puts all the crap on the game board.
bool Editor::initializeGamePieces() {
  return true;
}

bool Editor::initialize() {
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

bool Editor::initializeTextures() {
  textures->addTexture("clouds", "clouds_soft.png");
  return true;
}

void Editor::shutdown() {
  physics->shutdown();
  delete physics;
}
