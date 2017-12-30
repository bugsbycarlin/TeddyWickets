#include "textures.h"

//tex.texture_map = {};
//tex.texture_root = "Art/";

std::map<std::string, GLuint*> Textures::texture_map = {};
std::string Textures::texture_root = "Art/";


Textures::Textures() {

}

void Textures::addTexture(std::string texture_name, std::string texture_file) {
  std::string path = texture_root + texture_file;

  if (texture_map.count(texture_name) == 1) {
    return;
  }

  SDL_Surface*  image = IMG_Load(path.c_str());
  if (image == nullptr) {
      printf( "IMG_Load failed for %s with error: %s\n", path.c_str(), IMG_GetError() );
  }

  GLuint* tex = new GLuint[1];
  glGenTextures(1, tex);

  glBindTexture(GL_TEXTURE_2D, tex[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  texture_map[texture_name] = &tex[0];
}

void Textures::setTexture(std::string texture_name) {
  glBindTexture(GL_TEXTURE_2D, *texture_map[texture_name]);
}
