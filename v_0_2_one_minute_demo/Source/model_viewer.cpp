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
  // Simple viewport.
  glViewport(0, 0, k_screen_width, k_screen_height);

  // Clear color buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

  // Common material
  GLfloat material_ambient[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat material_diffuse[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat material_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat shininess[] = {5.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

  // Set the camera to look down at the character.
  // For fun, change the z-value to change the viewing angle of the game.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(-15, -15, 10,
            0, 0, 0,
            0, 0, 1);

  glColor4f(1.0, 1.0, 1.0, 1.0);

  glPushMatrix();
  glRotatef(rotation, 0.0f, 0.0f, 1.0f);

  model->render();

  glPopMatrix();
}

bool ModelViewer::initialize() {
  glEnable(GL_TEXTURE_2D);

  textures = new Textures();

  textures->addTexture("bear_face", "bear_test2.png");

  model = new Model(textures, "teddy_bear_draft_2.obj");

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport(0, 0, k_screen_width, k_screen_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, k_screen_width/(1.0 * k_screen_height), 0.1f, 1000.0f);

  return true;
}

void ModelViewer::shutdown() {
  // To do: release the image files.
  delete textures;
}

