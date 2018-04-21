/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "model_cache.h"

Model_Cache* model_cache = new Model_Cache();

Model_Cache::Model_Cache() {
  models = {};
}

Model* Model_Cache::getModel(std::string model_file_name) {
  if (models.find(model_file_name) == models.end()) {
    models[model_file_name] = new Model(model_file_name);
  } 

  return models[model_file_name];
}