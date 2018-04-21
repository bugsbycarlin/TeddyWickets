/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Model cache holds master copies of models indexed by model name,
  to prevent model duplication.
*/


#ifndef TEDDY_WICKETS_MODEL_CACHE_H_
#define TEDDY_WICKETS_MODEL_CACHE_H_

#include <stdio.h>
#include <cstdlib>
#include <map>
#include <string>

#include "model.h"

class Model_Cache {
 public:
    std::map<std::string, Model*> models;

    Model_Cache();

    Model* getModel(std::string model_file_name);
};

extern Model_Cache* model_cache;

#endif
