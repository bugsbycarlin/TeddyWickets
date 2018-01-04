/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Global constants.
*/

#ifndef TEDDY_WICKETS_GLOBALS_H_
#define TEDDY_WICKETS_GLOBALS_H_

#include <OpenGL/GLU.h>

#include <string>

// Screen variables
const int k_screen_width = 1440;
const int k_screen_height = 900;
const float k_aspect_ratio = k_screen_width / (1.0 * k_screen_height);
const float k_default_zoom = 11.0f;

// Bumper
const float k_bumper_height = 0.5f;
const float k_physics_bumper_height = 4.0f;
const float k_bumper_width = 0.5;

// Character
const float k_default_shot_rotation = -0.78f;
const float k_default_shot_power = 20.0f;
const float k_character_drop_height = 1.5f;

// Textures
const std::string k_texture_root_path = "Art/";

// Screen modes
enum {
  k_title_screen = 0,
  k_comic_screen = 1,
  k_1p_game_screen = 2,
  k_2p_game_screen = 3,
};

// Game modes
enum  {
  k_drop_mode = 0,
  k_prep_mode = 1,
  k_power_mode = 2,
  k_action_mode = 3
};

// Game variables
const float k_default_speed_ramping = 2.0f;
const float k_default_minimum_speed = 1.25f;
const float k_up_shot_angle = 60.0f * 3.141592f / 180.0f;

void Start2DDraw();
void End2DDraw();

/*

  Style guide:

  Variables:
  local variables: snake case: shot_rotation
  constants: snake case preceded by a k: k_screen_width
  methods: lower camel case: renderBackground
  classes: upper camel case: LevelMaker
  enums: snake case preceded by a k: k_prep_mode
  library junk like OpenGL: however that library does it

  Structure:
  indentation: two spaces
  indentation of extra params: two spaces
  brackets: opening brace always on the same line (including else statements)

  Example:

  void example(int thing, int multiplicity, Point* p1, Point* p2, Point* p3) {
    float modified_thing = thing * k_fine_structure_constant;
    for (int i = 0; i < multiplicity; i++) {
      if (modified_thing > k_threshold) {
        runBigCalculation(modified_thing,
          p1,
          p2,
          p3);
      } else {
        runSmallCalculation(modified_thing,
          p1,
          p2);
      }
    }
  }
*/

#endif
