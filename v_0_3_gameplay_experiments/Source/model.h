/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Heh. Model class models one 3d model.
*/

#ifndef TEDDY_WICKETS_MODEL_H_
#define TEDDY_WICKETS_MODEL_H_

#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <list>
#include <map>
#include <regex>
#include <string>

#include "globals.h"
#include "graphics.h"
#include "point.h"
#include "textures.h"
#include "triangle.h"

class Model {
 public:
    std::list<std::string> component_names;
    std::map<std::string, std::map<int, Point*>> vertices;
    std::map<std::string, std::map<int, Point*>> texture_coords;
    std::map<std::string, std::map<int, Point*>> normals;
    std::map<std::string, std::list<std::map<int, int>>> faces;
    std::map<std::string, std::string> texture_names;
    std::map<std::string, int> texture_keys;
    std::map<int, std::string> texture_map;

    std::map<std::string, std::vector<float>> vbo_vertex_data_by_texture;
    std::map<std::string, std::vector<float>> vbo_normal_data_by_texture;
    std::map<std::string, std::vector<float>> vbo_texture_data_by_texture;
    std::map<std::string, std::vector<float>> vbo_color_data_by_texture;
    std::map<std::string, int> vbo_cache_ids_by_texture;

    float outlineColor[3] = {0.0f, 0.0f, 0.0f};
    float outlineWidth = 10.0f;
    bool outline = true;

    int cache_id;

    Model(std::string model_file_name);

    void render();
    void oldRender();

    std::list<Triangle*> getMeshAsTriangles();
};

#endif
