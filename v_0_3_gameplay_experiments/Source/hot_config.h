/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Hot Config allows values to be changed without a recompile.
  Maybe it's just going to be for quick dev purposes.
  Maybe it will slow replace all the numbers in the game.
  Who knows?!
*/

#ifndef TEDDY_WICKETS_HOT_CONFIG_H_
#define TEDDY_WICKETS_HOT_CONFIG_H_

// Standard libraries
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <map>
#include <string>

// TinyXML for level file reading and writing
#include "tinyxml2.h"

class HotConfig {
 public:
  
  std::map<std::string, int> ints = { };
  std::map<std::string, float> floats = { };
  std::map<std::string, std::string> strings = { };

  HotConfig();

  float getFloat(std::string name);
  int getInt(std::string name);
  std::string getString(std::string name);
};

extern HotConfig* hot_config;

#endif
