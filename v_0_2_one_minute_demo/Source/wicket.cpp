/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "wicket.h"

Wicket::Wicket(std::string object_type, Physics* physics, Point* position, float rotation) : Hazard(object_type, physics, position, rotation) {
  this->value = 3;

  textures->addTexture("mint_green_star", "mint_green_star.png");
  textures->addTexture("salmon_star", "salmon_star.png");
  textures->addTexture("purple_star", "purple_star.png");

  wicket_value_text = new TextBox(hot_config->getString("wicket_font"), hot_config->getInt("wicket_font_size"),
    std::to_string(this->value), 0, 0, 0, hot_config->getInt("wicket_font_x"), hot_config->getInt("wicket_font_y"));
}

void Wicket::render() {
  Hazard::render();
}

void Wicket::setRenderInfo() {
  glm::vec3 coords = graphics->get2dCoords(position->x, position->y, position->z);
  wicket_value_text->x = (int) coords.x + hot_config->getInt("wicket_font_x");
  wicket_value_text->y = (int) (k_screen_height - coords.y) + hot_config->getInt("wicket_font_y");
}

void Wicket::renderInfo() {
  textures->setTexture("mint_green_star");
  graphics->rectangle(wicket_value_text->x + hot_config->getInt("wicket_star_x"), wicket_value_text->y + hot_config->getInt("wicket_star_y"), 128, 128);
  wicket_value_text->render();
}
