/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Level editor.
*/

#ifndef TEDDY_WICKETS_EDITOR_H_
#define TEDDY_WICKETS_EDITOR_H_

// SDL, for window, user input, and media
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

// Standard libraries
#include <math.h>
#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <list>
#include <string>

// TinyXML for level file reading and writing
#include "tinyxml2.h"

// Teddy Wickets includes
#include "hot_config.h"
#include "control_map.h"
#include "bumper.h"
#include "character.h"
#include "globals.h"
#include "graphics.h"
#include "physics.h"
#include "screen.h"
#include "surface.h"
#include "textbox.h"
#include "textures.h"
#include "hazard.h"

class Editor: public Screen {
 public:
  // Stuff
  std::list<Surface*> surfaces = { };
  std::list<Bumper*> bumpers = { };
  std::list<Hazard*> hazards = { };

  Hazard* current_shape;

  Physics* physics;

  int last_x, last_y, last_z;

  int min_x, min_y, max_x, max_y;

  float sway;

  int num_types;

  float zoom;

  unsigned long start_time;
  unsigned long last_time;
  unsigned long mark_time;
  unsigned long framerate_time;
  int frames_since_last;

  std::string map_file;

  std::string theme;

  Model* theme_tile;

  std::string shape_types[16] = {
    "tile_flat",
    "tile_down_x",
    "tile_down_y",
    "tile_up_x",
    "tile_up_y",
    "tile_down_x_y",
    "flat_siding",
    "flat_siding_down_x",
    "flat_siding_up_x",
    "flat_siding_sideways",
    "ramp",
    "wicket",
    "boxguy",
    "player_1_start",
    "player_2_start",
    "bumper"
  };
  int current_shape_type;

  Editor(std::string map_file);

  void loop(SDL_Window* window, FMOD::System *sound_system);

  void update();

  void render();
  void renderBackground();

  void handleKeys(SDL_Event e);

  bool saveMap();

  bool initialize();
  bool initializeLevel();
  bool initializeTextures();
  void shutdown();
};

#endif
