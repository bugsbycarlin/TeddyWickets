/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "free_star.h"

FreeStar::FreeStar(std::string object_type, Physics* physics, Point* position, float rotation) : Hazard(object_type, physics, position, rotation, false) {
  this->value = 1;

  this->player_owner = 0;

  this->active = true;

  this->floating_position = new Point(position->x, position->y, position->z);

  radius_expander = 1.2f;

  this->rotated = false;
  // assuming wicket is rotated +- Pi/2, or 0
  if (rotation - M_PI > 0.2 || rotation - M_PI < -0.2) {
    printf("Rotation was %0.2f, so this has been deemed a rotated\n", rotation);
    this->rotated = true;
  }

  textures->addTexture("mint_green_star", "mint_green_star.png");

  free_star_value_text = new TextBox(hot_config->getString("wicket_font"), hot_config->getInt("wicket_font_size"),
    std::to_string(this->value), 0, 0, 0, hot_config->getInt("wicket_font_x"), hot_config->getInt("wicket_font_y"));
}

void FreeStar::render() {
  if (!active) return;
  Hazard::render();
}

void FreeStar::setFloatingHeight(float height) {
  this->floating_position->z = this->position->z + height;
}

// Return true if player X has crossed the wicket. In that case, also flip the wicket's player owner.
bool FreeStar::flipStar(Point* current_point, float radius, int new_owner) {
  if (!active) return false;

  // if (position->distance(current_point) < 10) {
  //   printf("flipWicket 2 %0.3f vs %0.3f\n", position->distance(current_point), radius * radius_expander);
  // }
  if (floating_position->distance(current_point) < radius * radius_expander) {
    active = false;
    this->player_owner = new_owner;
    return true;
  } else {
    return false;
  }
}

void FreeStar::setRenderInfo(float time_step) {
  glm::vec3 coords = graphics->get2dCoords(position->x, position->y, position->z);
  free_star_value_text->x = (int) coords.x + hot_config->getInt("free_star_font_x") + 5 * sin(time_step * M_PI * bpm/60.0f);
  free_star_value_text->y = (int) (k_screen_height - coords.y) + hot_config->getInt("free_star_font_y") + 5 * cos(time_step * M_PI * bpm/60.0f);
}

void FreeStar::renderInfo() {
  if (!active) return;
  textures->setTexture("mint_green_star");
  
  graphics->rectangle(free_star_value_text->x + hot_config->getInt("free_star_star_x"), free_star_value_text->y + hot_config->getInt("free_star_star_y"), 128, 128);
  free_star_value_text->render();
}


