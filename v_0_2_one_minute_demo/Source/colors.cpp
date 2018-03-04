/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "colors.h"

Colors* colors = new Colors();

Colors::Colors() {
  palette["sky_blue"] = new Point(135, 206, 235);
  palette["muted_dark_blue"] = new Point(53, 62, 89);
  palette["silver_mist"] = new Point(191, 200, 205);

  palette["selection_box"] = new Point(140, 98, 57);

}

Point* Colors::color(std::string name) {
  if (palette.find(name) != palette.end()) {
    return palette[name];
  } else {
    printf("Oh no! Failure to find color %s. Quitting!\n", name.c_str());
  }
}

Point* Colors::lighter(Point* color) {
  return new Point(fmax(255, color->x * 1.1), fmax(255, color->y * 1.1), fmax(255, color->z * 1.1));
}

Point* Colors::darker(Point* color) {
  return new Point(color->x * 0.9, color->y * 0.9, color->z * 0.9);
}
