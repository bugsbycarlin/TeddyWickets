/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Screen class is the abstract class for different types of screens, 
  such as the title screen, a comic screen, or a game screen.
*/

#ifndef TEDDY_WICKETS_SCREEN_H_
#define TEDDY_WICKETS_SCREEN_H_

// SDL, for window, user input, and media
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

// Fmod for sound
#include "fmod.hpp"

#include "globals.h"
#include "graphics.h"

class Screen {
 public:
  Screen();

  int current_screen;
  bool quit;
  std::string music;
  FMOD::System* sound_system;

  unsigned long start_time;
  unsigned long last_time;
  unsigned long mark_time;
  unsigned long framerate_time;
  unsigned long current_time;
  int frames_since_last;

  virtual void loop(SDL_Window* window, FMOD::System *sound_system);

  virtual bool initialize();
  virtual void shutdown();
};

#endif
