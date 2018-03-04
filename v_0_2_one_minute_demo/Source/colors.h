/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Color class holds the allowable colors of the game. The intention is to have a consistent color palette.
*/

#ifndef TEDDY_WICKETS_COLORS_H_
#define TEDDY_WICKETS_COLORS_H_

#include "globals.h"
#include "point.h"

#include <stdio.h>
#include <cstdlib>
#include <unordered_map>
#include <string>

class Colors {
 public:
  std::unordered_map<std::string, Point*> palette;

  Colors();

  Point* color(std::string name);
  Point* lighter(Point* color);
  Point* darker(Point* color);
};

extern Colors* colors;

#endif
