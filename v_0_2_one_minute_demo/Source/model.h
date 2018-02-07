/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Heh. Model class models one 3d model.
*/

/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

  Character class models the player's avatar (ie the bear, or the ball).
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

#include "cel_shader.h"
#include "globals.h"
#include "point.h"
#include "textures.h"

class Model {
 public:
    Textures* textures;
    CelShader* cel_shader;

    std::list<std::string> component_names;
    std::map<std::string, std::map<int, Point*>> vertices;
    std::map<std::string, std::map<int, Point*>> texture_coords;
    std::map<std::string, std::map<int, Point*>> normals;
    std::map<std::string, std::list<std::map<int, int>>> faces;
    std::map<std::string, std::string> texture_names;
    std::map<std::string, int> texture_keys;
    std::map<int, std::string> texture_map;


    // Display list global counter
    static int next_display_list_index;

    float outlineColor[3] = {0.0f, 0.0f, 0.0f};
    float outlineWidth = 10.0f;
    bool outline = true;

    int display_list_index;

    Model(Textures* textures, CelShader* cel_shader, std::string model_file_name);

    void render();
};

#endif
