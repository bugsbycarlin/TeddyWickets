/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "wicket.h"

Wicket::Wicket(std::string object_type, Physics* physics, Point* position, float rotation) : Hazard(object_type, physics, position, rotation) {
  this->value = 3;

  wicket_value_text = new TextBox("curved_square.ttf", hot_config->getInt("wicket_font_size"),
    std::to_string(this->value), 0, 0, 0, hot_config->getInt("wicket_font_x"), hot_config->getInt("wicket_font_y"));
}

void Wicket::render() {
  Hazard::render();
}

void Wicket::setRenderInfo() {
  //graphics->pushModelMatrix();
  //graphics->translate(position->x, position->y, position->z);
  //graphics->rotate(-90.0f, 0.0f, 0.0f, 1.0f);
  //graphics->rotate(-rotation * 180.0 / M_PI, 0, 0, 1);
  glm::vec3 coords = graphics->get2dCoords(position->x, position->y, position->z);
  //graphics->popModelMatrix();

  wicket_value_text->x = (int) coords.x + hot_config->getInt("wicket_font_x");
  wicket_value_text->y = (int) coords.y + hot_config->getInt("wicket_font_y");
  // printf("I got %d,%d\n", wicket_value_text->x, wicket_value_text->y);
}

void Wicket::renderInfo() {

  wicket_value_text->render();
}
