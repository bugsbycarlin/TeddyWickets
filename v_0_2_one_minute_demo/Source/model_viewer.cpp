/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "model_viewer.h"

// Game constructor
ModelViewer::ModelViewer() {
  quit = false;

  zoom = k_default_zoom / 2.0f;
  rotation = 0;

  mousedown = false;
  drag_x = 0;
  drag_y = 0;

  current_screen = k_model_viewer_screen;
}

// Game loop. Main.cpp is running this on a loop until it's time to switch to a different part of the game.
void ModelViewer::loop(SDL_Window* window, FMOD::System *sound_system) {
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

  // Update screen
  SDL_GL_SwapWindow(window);
}

void ModelViewer::update() {
  rotation -= 1.0f;
}

// Handle keyboard input
void ModelViewer::handleKeys(unsigned char key) {
  if (key == 'p') {
    quit = true;
  }

  if (key == 'z') {
    zoom += 1.0f;
  } else if (key == 'x') {
    zoom -= 1.0f;
  }
}

// Handle mouse input
void ModelViewer::handleMouse(SDL_Event e) {
  // // In prep mode, I use mousemotion, mousedown and mouseup to track mouse dragging.
  // // In a drag event, I set the rotation of the character.
  // if (game_mode == k_prep_mode) {
  //   if (mousedown && e.type == SDL_MOUSEMOTION) {
  //     int x = 0, y = 0;
  //     SDL_GetMouseState(&x, &y);
  //     character->setShotRotation(pre_drag_rotation - (drag_x - x) / 50.0f, false);
  //   } else if (e.type == SDL_MOUSEBUTTONDOWN) {
  //     mousedown = true;
  //     SDL_GetMouseState(&drag_x, &drag_y);
  //     pre_drag_rotation = character->shot_rotation;
  //   } else if (e.type == SDL_MOUSEBUTTONUP) {
  //     mousedown = false;
  //     int x = 0, y = 0;
  //     SDL_GetMouseState(&x, &y);
  //     character->setShotRotation(pre_drag_rotation - (drag_x - x) / 50.0f, true);
  //     // this is a cheap hack; if the player has released a click
  //     // and there's no change in the mouse position, count it as a regular click
  //     // and switch to power mode.
  //     if (x == drag_x && y == drag_y) {
  //       game_mode = k_power_mode;
  //       character->shot_power = 0;
  //       shot_rising = true;
  //     }
  //   }
  // } else if (game_mode == k_power_mode) {
  //   // In power mode, clicking just makes you take the "golf" shot.
  //   if (e.type == SDL_MOUSEBUTTONUP) {
  //     shoot();
  //   }
  // }
}

void ModelViewer::render() {
  graphics->clearScreenWithColor(1.0f, 1.0f, 1.0f, 1.0f);
  
  graphics->color(0.5f, 1.0f, 1.0f, 1.0f);

  graphics->set3d(5.0f);

  // Set the camera to look down at the character.
  // For fun, change the z-value to change the viewing angle of the game.
  graphics->standardCamera(15, 15, 10, 0, 0, 0);

  graphics->standardLightPosition();

  graphics->pushMatrix();
  graphics->rotate(rotation, 0.0f, 0.0f, 1.0f);

  model->render();

  graphics->popMatrix();
}

bool ModelViewer::initialize() {
  graphics->initializeBasic();

  textures = new Textures();

  textures->addTexture("bear_face", "bear_test2.png");
  textures->addTexture("plain", "plain.png");
  textures->addTexture("barber", "barber_pole.png");

  //model = new Model(textures, "teddy_bear_draft_2.obj");
  model = new Model(textures, "wicket.obj");

  return true;
}

void ModelViewer::shutdown() {
  // To do: release the image files.
  delete textures;
}

