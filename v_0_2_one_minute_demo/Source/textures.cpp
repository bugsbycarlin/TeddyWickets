/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "textures.h"

Textures::Textures() {
  texture_map = {};
  texture_root = k_texture_root_path;
}

// Add a texture to Textures
void Textures::addTexture(std::string texture_name, std::string texture_file) {
  std::string path = texture_root + texture_file;

  if (texture_map.count(texture_name) == 1) {
    return;
  }

  // Use the SDL to load the image
  SDL_Surface* image = IMG_Load(path.c_str());
  if (image == nullptr) {
      printf("IMG_Load failed for %s with error: %s\n", path.c_str(), IMG_GetError());
  }

  // Use OpenGL to generate a texture
  GLuint* texture = new GLuint[1];
  glGenTextures(1, texture);

  glBindTexture(GL_TEXTURE_2D, texture[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Save the texture id in a map
  texture_map[texture_name] = &texture[0];
}

// Look up the texture id by name in the texture map, and use OpenGL to bind the texture id
void Textures::setTexture(std::string texture_name) {
  glBindTexture(GL_TEXTURE_2D, *texture_map[texture_name]);
}
