/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "textbox.h"

TextBox::TextBox(std::string font_name, int font_size, std::string text, Uint8 R, Uint8 G, Uint8 B, int x, int y) {
  std::string path = k_font_root_path + font_name;
  font = TTF_OpenFont(path.c_str(), font_size);
  if (font == nullptr) {
    printf("Failed to load font.\n");
  }

  this->x = x;
  this->y = y;

  this->cache_id = -1;

  this->text = text;

  this->countdown = 0;

  color = {R, G, B};

  text_surface = TTF_RenderText_Blended(font, this->text.c_str(), color);
  texture = graphics->makeTexture(text_surface->w, text_surface->h, text_surface->pixels, false);
}

void TextBox::setText(std::string text) {
  this->text = text;

  // might have a memory leak here as I change the texture pointer.
  text_surface = TTF_RenderText_Blended(font, this->text.c_str(), color);
  texture = graphics->makeTexture(text_surface->w, text_surface->h, text_surface->pixels, false);
}

void TextBox::setColor(Uint8 R, Uint8 G, Uint8 B) {
  color = {R, G, B};

  // might have a memory leak here as I change the texture pointer.
  text_surface = TTF_RenderText_Blended(font, this->text.c_str(), color);
  texture = graphics->makeTexture(text_surface->w, text_surface->h, text_surface->pixels, false);
}

void TextBox::setTemporaryColor(Uint8 R, Uint8 G, Uint8 B) {
  permanent_color = color;
  color = {R, G, B};
  countdown = 10;

  // might have a memory leak here as I change the texture pointer.
  text_surface = TTF_RenderText_Blended(font, this->text.c_str(), color);
  texture = graphics->makeTexture(text_surface->w, text_surface->h, text_surface->pixels, false);
}

void TextBox::render() {
  graphics->setTexture(texture);

  graphics->rectangle(x, y, text_surface->w, text_surface->h);

  if (countdown > 0) {
    countdown--;

    if (countdown == 0) {
      color = permanent_color;
      text_surface = TTF_RenderText_Blended(font, this->text.c_str(), color);
      texture = graphics->makeTexture(text_surface->w, text_surface->h, text_surface->pixels, false);
    }
  }
}

void TextBox::shutdown() {
  TTF_CloseFont(font);
}
