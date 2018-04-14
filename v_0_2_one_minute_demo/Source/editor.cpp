/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "editor.h"

using namespace tinyxml2;

// The author of tinyxml says,
// "For my own convenience, I also defined a macro for checking error results:"
#ifndef XMLCheckResult
  #define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); return false; }
#endif

// Editor constructor
Editor::Editor(std::string map_file) {
  current_screen = k_editor_screen;
  last_x = 0;
  last_y = 0;
  last_z = 0;
  sway = 0;
  current_shape_type = 0;
  num_types = 14;
  this->map_file = map_file;
  zoom = k_default_zoom;
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

  sway = 0.5 * sin((current_time - start_time) / 1000.0f);

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
  bool print = false;
  if (e.key.keysym.sym == SDLK_ESCAPE) {
    quit = true;
  } else if (e.key.keysym.sym == SDLK_RETURN) {
    saveMap();
  }

  if (e.key.keysym.sym == SDLK_n) {
    if (current_shape != nullptr) {
      hazards.push_front(current_shape);
      last_x = current_shape->position->x;
      last_y = current_shape->position->y;
      last_z = current_shape->position->z;
    }
    current_shape = new Hazard(shape_types[current_shape_type], physics,
        new Point(last_x, last_y + 6, last_z), M_PI);
  }

  if (current_shape != nullptr) {
    if (e.key.keysym.sym == SDLK_t) {
      current_shape_type += 1;
      if (current_shape_type >= num_types) {
        current_shape_type = 0;
      }
      current_shape->object_type = shape_types[current_shape_type];
      current_shape->model = model_cache->getModel(shape_types[current_shape_type] + ".obj");
    }

    if (e.key.keysym.sym == SDLK_UP) {
      current_shape->position->x = current_shape->position->x - 1;
    } else if (e.key.keysym.sym == SDLK_DOWN) {
      current_shape->position->x = current_shape->position->x + 1;
    } else if (e.key.keysym.sym == SDLK_LEFT) {
      current_shape->position->y = current_shape->position->y - 1;
    } else if (e.key.keysym.sym == SDLK_RIGHT) {
      current_shape->position->y = current_shape->position->y + 1;
    } else if (e.key.keysym.sym == SDLK_s) {
      current_shape->position->z = current_shape->position->z - 1;
    } else if (e.key.keysym.sym == SDLK_a) {
      current_shape->position->z = current_shape->position->z + 1;
    } else if (e.key.keysym.sym == SDLK_r) {
      current_shape->rotation = current_shape->rotation + M_PI / 4.0f;
      if (current_shape->rotation > 2 * M_PI) {
        current_shape->rotation = current_shape->rotation - 2 * M_PI;
      }
    } else if (e.key.keysym.sym == SDLK_u) {
      int min_y = last_y;
      for (auto hazard = hazards.begin(); hazard != hazards.end(); ++hazard) {
        if ((*hazard)->position->y < min_y) {
          min_y = (*hazard)->position->y;
        }
      }
      current_shape->position->y = min_y;
      current_shape->position->x = current_shape->position->x + 6;
    }
  } else {
    if (e.key.keysym.sym == SDLK_UP) {
      last_x -= 1;
    } else if (e.key.keysym.sym == SDLK_DOWN) {
      last_x += 1;
    } else if (e.key.keysym.sym == SDLK_LEFT) {
      last_y -= 1;
    } else if (e.key.keysym.sym == SDLK_RIGHT) {
      last_y += 1;
    } else if (e.key.keysym.sym == SDLK_s) {
      last_z -= 1;
    } else if (e.key.keysym.sym == SDLK_a) {
      last_z += 1;
    }
  }

  if (e.key.keysym.sym == SDLK_z) {
    zoom += 1.0f;
  } else if (e.key.keysym.sym == SDLK_x) {
    zoom -= 1.0f;
  }

  if (current_shape != nullptr) {
    printf("Position: %0.2f, %0.2f, %0.2f\n", current_shape->position->x, current_shape->position->y, current_shape->position->z);
  } else {
    printf("Position: %d, %d, %d\n", last_x, last_y, last_z);
  }
}

bool Editor::saveMap() {
  // back up the map
  std::string copy_command = "cp " + map_file + " " + map_file + "backup_" + std::to_string(hazards.size());
  system(copy_command.c_str());
  // save the map

  XMLDocument saveDoc;

  XMLElement* music_element = saveDoc.NewElement("music");
  music_element->SetText(this->music.c_str());
  saveDoc.InsertEndChild(music_element);

  XMLElement* theme_element = saveDoc.NewElement("theme");
  theme_element->SetText(this->theme.c_str());
  saveDoc.InsertEndChild(theme_element);

  XMLNode* shape_element = saveDoc.NewElement("shape");
  saveDoc.InsertEndChild(shape_element);

  for (auto hazard = hazards.begin(); hazard != hazards.end(); ++hazard) {
    XMLElement* tile_element = saveDoc.NewElement("tile");
    tile_element->SetAttribute("type", (*hazard)->object_type.c_str());
    
    XMLElement* x_value = saveDoc.NewElement("x");
    x_value->SetText((*hazard)->position->x);
    tile_element->InsertEndChild(x_value);
    
    XMLElement* y_value = saveDoc.NewElement("y");
    y_value->SetText((*hazard)->position->y);
    tile_element->InsertEndChild(y_value);
    
    XMLElement* z_value = saveDoc.NewElement("z");
    z_value->SetText((*hazard)->position->z);
    tile_element->InsertEndChild(z_value);

    XMLElement* r_value = saveDoc.NewElement("r");
    r_value->SetText((*hazard)->rotation - M_PI);
    tile_element->InsertEndChild(r_value);

    shape_element->InsertFirstChild(tile_element);
  }

  XMLError result = saveDoc.SaveFile(map_file.c_str());
  XMLCheckResult(result);

  printf("Saved map to %s\n", map_file.c_str());
}

void Editor::render() {
  graphics->clearScreen();
  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);

  renderBackground();

  graphics->set3d(zoom);
  graphics->enableLights();

  if (current_shape != nullptr) {
    Point* position = current_shape->position;
    graphics->standardCamera(position->x + 15, position->y + 15, position->z + 10,
      position->x, position->y, position->z);
  } else {
    graphics->standardCamera(last_x + 15, last_y + 15, last_z + 10,
      last_x, last_y, last_z);
  }

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

  // Render theme tile
  if (theme == "water") {
    // for (int i = min_x - 30; i <= max_x + 40; i += 6) {
    //   for (int j = min_y - 30; j <= max_y + 40; j += 6) {
    //     graphics->pushModelMatrix();
    //     graphics->translate(i, j + sway, 0);
    //     theme_tile->render();
    //     graphics->popModelMatrix();
    //   }
    // }
    graphics->pushModelMatrix();
    graphics->translate(50, 0 + sway, 0);
    theme_tile->render();
    graphics->popModelMatrix();
  }

  if (current_shape != nullptr) {
    current_shape->render();
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
bool Editor::initializeLevel() {

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
  }

  // Level Shape
  XMLElement* level_shape = doc.FirstChildElement("shape");

  XMLElement * tile_element = level_shape->FirstChildElement("tile");
  while (tile_element != nullptr) {
    int x, y, z, grade;
    float r;
    tile_element->FirstChildElement("x")->QueryIntText(&x);
    tile_element->FirstChildElement("y")->QueryIntText(&y);
    tile_element->FirstChildElement("z")->QueryIntText(&z);
    tile_element->FirstChildElement("r")->QueryFloatText(&r);
    std::string tile_type(tile_element->Attribute("type"));

    hazards.push_front(new Hazard(tile_type, physics,
        new Point(x, y, z), M_PI + r));

    last_x = x;
    last_y = y;
    last_z = z;

    if (x < min_x) min_x = x;
    if (x > max_x) max_x = x;
    if (y < min_y) min_y = y;
    if (y > max_y) max_y = y;

    tile_element = tile_element->NextSiblingElement("tile");
  }

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

  // Initialize Level
  if (!initializeLevel()) {
    printf("Unable to initialize level!\n");
    return false;
  }

  start_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
  last_time = start_time;
  framerate_time = start_time;

  if (hot_config->getInt("use_music") == 1) {
    FMOD::Sound *audio_stream;
    sound_system->createStream(music.c_str(), FMOD_DEFAULT, 0, &audio_stream);
    sound_system->playSound(audio_stream, NULL, false, 0);
  }

  return true;
}

bool Editor::initializeTextures() {
  textures->addTexture("clouds", "clouds_soft.png");

  textures->addTexture("tiles", "tile.png");
  textures->addTexture("polar_grey", "polar_grey.png");
  textures->addTexture("pastel_rainbow", "pastel_rainbow.png");

  textures->addTexture("water", "water.png");
  return true;
}

void Editor::shutdown() {
  physics->shutdown();
  delete physics;
}
