/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "wicket.h"

Wicket::Wicket(std::string object_type, Physics* physics, Point* position, float rotation) : Hazard(object_type, physics, position, rotation) {
  this->value = 3;

  this->player_owner = 0;

  this->active = true;

  this->rotated = false;
  // assuming wicket is rotated +- Pi/2, or 0
  if (rotation - M_PI > 0.2 || rotation - M_PI < -0.2) {
    printf("Rotation was %0.2f, so this has been deemed a rotated\n", rotation);
    this->rotated = true;
  }

  textures->addTexture("mint_green_star", "mint_green_star.png");
  textures->addTexture("salmon_star", "salmon_star.png");
  textures->addTexture("purple_star", "purple_star.png");
  textures->addTexture("rainbow_star", "rainbow_star.png");

  wicket_value_text = new TextBox(hot_config->getString("wicket_font"), hot_config->getInt("wicket_font_size"),
    std::to_string(this->value), 0, 0, 0, hot_config->getInt("wicket_font_x"), hot_config->getInt("wicket_font_y"));
}

void Wicket::render() {
  Hazard::render();
}

// Return true if player X has crossed the wicket. In that case, also flip the wicket's player owner.
bool Wicket::flipWicket(Point* current_point, Point* last_point, int new_owner) {
  //printf("In flipWicket\n");
  // Return false if outside the box
  float wicket_height = 4.0f;
  float x_margin = 2.5f;
  float y_margin = 1.0f;
  if (this->rotated) {
    x_margin = 1.0f;
    y_margin = 2.5f;
  }
  if (last_point->x > position->x + x_margin || last_point->x < position->x - x_margin) {
    return false;
  } else if (last_point->y > position->y + y_margin || last_point->y < position->y - y_margin) {
    return false;
  } else if (current_point->x > position->x + x_margin || current_point->x < position->x - x_margin) {
    return false;
  } else if (current_point->y > position->y + y_margin || current_point->y < position->y - y_margin) {
    return false;
  } else if (current_point->z < position->z || current_point->z > position->z + wicket_height) {
    return false;
  }

  printf("Got past the outer tests\n");
  bool flip = false;
  if (this->rotated) {
    printf("Checking along the x position!\n");
    printf("Current x: %0.2f, Last x: %0.2f, Target x: %0.2f\n", current_point->x, last_point->x, position->x);
    if ((current_point->x >= position->x && last_point->x <= position->x) ||
        (current_point->x <= position->x && last_point->x >= position->x)) {
      printf("Flipped along the x position!\n");
      flip = true;
    }
  } else if (!this->rotated) {
    printf("Checking along the y position!\n");
    printf("Current y: %0.2f, Last y: %0.2f, Target y: %0.2f\n", current_point->y, last_point->y, position->y);
    if ((current_point->y >= position->y && last_point->y <= position->y) ||
        (current_point->y <= position->y && last_point->y >= position->y)) {
      printf("Flipped along the x position!\n");
      flip = true;
    }
  }

  if (flip) {
    this->player_owner = new_owner;
    printf("My owner is now %d\n", this->player_owner);
  }

  return flip;
}

void Wicket::setRenderInfo(float time_step) {
  glm::vec3 coords = graphics->get2dCoords(position->x, position->y, position->z);
  wicket_value_text->x = (int) coords.x + hot_config->getInt("wicket_font_x") + 5 * sin(time_step * M_PI * bpm/60.0f);
  wicket_value_text->y = (int) (k_screen_height - coords.y) + hot_config->getInt("wicket_font_y") + 5 * cos(time_step * M_PI * bpm/60.0f);
}

void Wicket::renderInfo() {
  if (this->player_owner == 0) {
    textures->setTexture("mint_green_star");
  } else if (this->player_owner == 1) {
    textures->setTexture("salmon_star");
  } else if (this->player_owner == 2) {
    textures->setTexture("purple_star");
  }
  graphics->rectangle(wicket_value_text->x + hot_config->getInt("wicket_star_x"), wicket_value_text->y + hot_config->getInt("wicket_star_y"), 128, 128);
  wicket_value_text->render();
}

void Wicket::renderLastWicketInfo() {
  if (!active) return;
  textures->setTexture("rainbow_star");
  graphics->rectangle(wicket_value_text->x + hot_config->getInt("wicket_star_x"), wicket_value_text->y + hot_config->getInt("wicket_star_y"), 128, 128);
  wicket_value_text->render();
}
