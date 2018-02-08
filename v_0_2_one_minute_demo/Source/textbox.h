/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Textbox class holds a textbox of a particular size and font, and prints it to the screen.
*/

#ifndef TEDDY_WICKETS_TEXTBOX_H_
#define TEDDY_WICKETS_TEXTBOX_H_

// SDL, for window, user input, and media
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

// Standard libraries
#include <stdio.h>
#include <cstdlib>
#include <string>

// Teddy Wickets includes
#include "globals.h"
#include "teddy_gl.h"

class TextBox {
 public:
  SDL_Surface* text_surface;
  TTF_Font* font;
  SDL_Color color;
  GLuint* texture;

  std::string text;
  int x;
  int y;

  int countdown;
  SDL_Color permanent_color;

  TextBox(std::string font_name, int font_size, std::string text, Uint8 R, Uint8 G, Uint8 B, int x, int y);

  void setText(std::string text);
  void setColor(Uint8 R, Uint8 G, Uint8 B);
  void setTemporaryColor(Uint8 R, Uint8 G, Uint8 B);

  void render();

  void shutdown();
};

#endif
