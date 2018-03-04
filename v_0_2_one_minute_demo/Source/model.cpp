/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "model.h"

Model::Model(std::string model_file_name) {
  std::string line;
  std::smatch match;

  this->cache_id = -1;

  std::string material = model_file_name;
  material.replace(material.length() - 3, 3, "mtl");
  std::string material_path = k_model_root_path + material;
  printf("Loading material from %s\n", material_path.c_str());

  std::ifstream material_file;
  material_file.open(material_path);

  if (!material_file.is_open()) {
    printf("Error: Failed to load material from %s\n", material_path.c_str());
    return;
  }

  std::regex new_material_regex("newmtl ([a-zA-Z0-9_]+)");
  std::regex texture_map_regex("map_Kd ([a-zA-Z0-9\._]+)");

  std::string last_texture_name = "";
  while (getline(material_file, line)) {
    if (std::regex_search(line, match, new_material_regex)) {
      printf("Matched material: %s\n", match.str(1).c_str());
      last_texture_name = match.str(1);
      texture_names[last_texture_name] = "";
    } else if (std::regex_search(line, match, texture_map_regex)) {
      printf("Matched texture name: %s\n", match.str(1).c_str());
      textures->addTexture(match.str(1), match.str(1));
      texture_names[last_texture_name] = match.str(1);
    }
  }

  material_file.close();

  int tex_counter = 0;
  for (const auto &pair : texture_names) {
    texture_keys[pair.second] = tex_counter;
    texture_map[tex_counter] = pair.second;
    printf("Storing %s with %d\n", pair.second.c_str(), tex_counter);
    tex_counter += 1;
  }

  std::string path = k_model_root_path + model_file_name;
  printf("Loading model from %s\n", path.c_str());

  std::ifstream model_file;
  model_file.open(path);

  if (!model_file.is_open()) {
    printf("Error: Failed to load model from %s\n", path.c_str());
    return;
  }

  component_names = {};

  std::regex vertex_regex("v ([\-0-9\.]+) ([\-0-9\.]+) ([\-0-9\.]+)");
  std::regex texture_regex("vt ([\-0-9\.]+) ([\-0-9\.]+)");
  std::regex material_regex("usemtl ([a-zA-Z0-9_]+)");
  std::regex normal_regex("vn ([\-0-9\.]+) ([\-0-9\.]+) ([\-0-9\.]+)");
  std::regex three_point_face_regex("f ([0-9]+)\/([0-9]+)\/([0-9]+) ([0-9]+)\/([0-9]+)\/([0-9]+) ([0-9]+)\/([0-9]+)\/([0-9]+)");
  std::regex four_point_face_regex("f ([0-9]+)\/([0-9]+)\/([0-9]+) ([0-9]+)\/([0-9]+)\/([0-9]+) ([0-9]+)\/([0-9]+)\/([0-9]+) ([0-9]+)\/([0-9]+)\/([0-9]+)");
  std::regex component_name_regex("g ([a-zA-Z0-9_]+)");

  std::string component_name = "";

  bool look_for_new_component = true;
  std::map<int, Point*> current_vertices = {};
  std::map<int, Point*> current_texture_coords = {};
  std::map<int, Point*> current_normals = {};
  std::list<std::map<int, int>> current_faces = {};
  int vertex_counter = 1;
  int texture_coord_counter = 1;
  int normal_counter = 1;

  while (getline(model_file, line)) {
    if (std::regex_search(line, match, vertex_regex)) {
      float v1 = ::atof(match.str(1).c_str());
      float v2 = ::atof(match.str(2).c_str());
      float v3 = ::atof(match.str(3).c_str());
      // printf("Matched vertex coords: %s\n", line.c_str());
      // printf("%f\n", v1);
      // printf("%f\n", v2);
      // printf("%f\n", v3);

      if (look_for_new_component) {
        look_for_new_component = false;

        // we have an existing component to store
        if (component_name != "") {
          vertices[component_name] = current_vertices;
          texture_coords[component_name] = current_texture_coords;
          normals[component_name] = current_normals;
          faces[component_name] = current_faces;
          component_names.push_back(component_name);

          current_vertices = {};
          current_texture_coords = {};
          current_normals = {};
          current_faces = {};

          // vertex_counter = 1;
          // texture_coord_counter = 1;
          // normal_counter = 1;

          component_name = "";
        }
      }

      current_vertices[vertex_counter] = new Point(v1, v2, v3);
      vertex_counter += 1;

    } else if (std::regex_search(line, match, texture_regex)) {
      float t1 = ::atof(match.str(1).c_str());
      float t2 = ::atof(match.str(2).c_str());
      current_texture_coords[texture_coord_counter] = new Point(t1, t2, 0);
      texture_coord_counter += 1;
    } else if (std::regex_search(line, match, normal_regex)) {
      float n1 = ::atof(match.str(1).c_str());
      float n2 = ::atof(match.str(2).c_str());
      float n3 = ::atof(match.str(3).c_str());
      current_normals[normal_counter] = new Point(n1, n2, n3);
      normal_counter += 1;
    } else if (std::regex_search(line, match, material_regex)) {
      std::string name = texture_names[match.str(1)];
      std::map<int, int> face = {};
      face[0] = texture_keys[name];
      current_faces.push_back(face);
    } else if (std::regex_search(line, match, four_point_face_regex)) {
      std::map<int, int> face = {};
      face[1] = ::atoi(match.str(1).c_str());
      face[2] = ::atoi(match.str(2).c_str());
      face[3] = ::atoi(match.str(3).c_str());
      face[4] = ::atoi(match.str(4).c_str());
      face[5] = ::atoi(match.str(5).c_str());
      face[6] = ::atoi(match.str(6).c_str());
      face[7] = ::atoi(match.str(7).c_str());
      face[8] = ::atoi(match.str(8).c_str());
      face[9] = ::atoi(match.str(9).c_str());
      face[10] = ::atoi(match.str(10).c_str());
      face[11] = ::atoi(match.str(11).c_str());
      face[12] = ::atoi(match.str(12).c_str());
      current_faces.push_back(face);
    } else if (std::regex_search(line, match, three_point_face_regex)) {
      std::map<int, int> face = {};
      face[1] = ::atoi(match.str(1).c_str());
      face[2] = ::atoi(match.str(2).c_str());
      face[3] = ::atoi(match.str(3).c_str());
      face[4] = ::atoi(match.str(4).c_str());
      face[5] = ::atoi(match.str(5).c_str());
      face[6] = ::atoi(match.str(6).c_str());
      face[7] = ::atoi(match.str(7).c_str());
      face[8] = ::atoi(match.str(8).c_str());
      face[9] = ::atoi(match.str(9).c_str());
      current_faces.push_back(face);
    } else if (std::regex_search(line, match, component_name_regex)) {
      look_for_new_component = true;
      component_name = match.str(1);
    }
  }

  if (look_for_new_component) {
    look_for_new_component = false;

    // we have an existing component to store
    if (component_name != "") {
      vertices[component_name] = current_vertices;
      texture_coords[component_name] = current_texture_coords;
      normals[component_name] = current_normals;
      faces[component_name] = current_faces;
      component_names.push_back(component_name);

      current_vertices = {};
      current_texture_coords = {};
      current_normals = {};
      current_faces = {};

      component_name = "";
    }
  }

  model_file.close();
}

std::list<Triangle*> Model::getMeshAsTriangles() {
  std::list<Triangle*> mesh = {};
  int counter = 0;
  for (auto component = component_names.begin(); component != component_names.end(); ++component) {
    std::string component_name = component->c_str();
    std::list<std::map<int, int>> current_faces = faces[component_name];

    std::map<int, Point*> current_vertices = vertices[component_name];

    for (auto face = current_faces.begin(); face != current_faces.end(); ++face) {
      int num_edges = -1;
      if (face->size() == 1) {
        continue;
      } else if (face->size() == 12) {
        num_edges = 4;
      } else if (face->size() == 9) {
        num_edges = 3;
      }

      counter += 1;
      mesh.push_back(new Triangle(
        current_vertices[face->operator[](1)],
        current_vertices[face->operator[](4)],
        current_vertices[face->operator[](7)])
      );
      if (num_edges == 4) {
        counter += 1;
        mesh.push_back(new Triangle(
          current_vertices[face->operator[](7)],
          current_vertices[face->operator[](10)],
          current_vertices[face->operator[](1)])
        );
      }
    }
  }

  printf("Mesh loading: Counted %d triangles. Final mesh has %d triangles\n", counter, mesh.size());

  return mesh;
}

void Model::render() {
  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);
  graphics->rotate(90.0f, 1.0f, 0.0f, 0.0f);

  if (cache_id == -1) {
    vbo_vertex_data_by_texture = {};
    vbo_normal_data_by_texture = {};
    vbo_texture_data_by_texture = {};
    vbo_color_data_by_texture = {};
    vbo_cache_ids_by_texture = {};
    std::string current_texture = "";

    for (auto component = component_names.begin(); component != component_names.end(); ++component) {
      std::string component_name = component->c_str();
      std::list<std::map<int, int>> current_faces = faces[component_name];

      std::map<int, Point*> current_vertices = vertices[component_name];
      std::map<int, Point*> current_texture_coords = texture_coords[component_name];
      std::map<int, Point*> current_normals = normals[component_name];

      for (auto face = current_faces.begin(); face != current_faces.end(); ++face) {
        int num_edges = -1;
        if (face->size() == 1) {
          //textures->setTexture(texture_map[face->operator[](0)]);
          printf("Found texture %s yo\n", texture_map[face->operator[](0)].c_str());
          current_texture = texture_map[face->operator[](0)];
          if (vbo_vertex_data_by_texture.find(current_texture) == vbo_vertex_data_by_texture.end()) {
            vbo_vertex_data_by_texture[current_texture] = {};
          }
          if (vbo_normal_data_by_texture.find(current_texture) == vbo_normal_data_by_texture.end()) {
            vbo_normal_data_by_texture[current_texture] = {};
          }
          if (vbo_texture_data_by_texture.find(current_texture) == vbo_texture_data_by_texture.end()) {
            vbo_texture_data_by_texture[current_texture] = {};
          }
          if (vbo_color_data_by_texture.find(current_texture) == vbo_color_data_by_texture.end()) {
            vbo_color_data_by_texture[current_texture] = {};
          }
          continue;
        } else if (face->size() == 12) {
          num_edges = 4;
        } else if (face->size() == 9) {
          num_edges = 3;
        }

        //float data[num_edges * 8];
        // now always triangles, no quads allowed
        for (int i = 0; i < num_edges; i++) {
          int start = 3*i + 1;

          Point* vertex = current_vertices[face->operator[](start)];
          Point* texture = current_texture_coords[face->operator[](start+1)];
          Point* normal = current_normals[face->operator[](start+2)];

          vbo_vertex_data_by_texture[current_texture].push_back(vertex->x);
          vbo_vertex_data_by_texture[current_texture].push_back(vertex->y);
          vbo_vertex_data_by_texture[current_texture].push_back(vertex->z);

          vbo_normal_data_by_texture[current_texture].push_back(normal->x);
          vbo_normal_data_by_texture[current_texture].push_back(normal->y);
          vbo_normal_data_by_texture[current_texture].push_back(normal->z);

          vbo_color_data_by_texture[current_texture].push_back(1.0f);
          vbo_color_data_by_texture[current_texture].push_back(1.0f);
          vbo_color_data_by_texture[current_texture].push_back(1.0f);
          vbo_color_data_by_texture[current_texture].push_back(1.0f);

          vbo_texture_data_by_texture[current_texture].push_back(texture->x);
          vbo_texture_data_by_texture[current_texture].push_back(-texture->y);

          // data[8*i] = texture->x;
          // data[8*i+1] = -texture->y;
          // data[8*i+2] = normal->x;
          // data[8*i+3] = normal->y;
          // data[8*i+4] = normal->z;
          // data[8*i+5] = vertex->x;
          // data[8*i+6] = vertex->y;
          // data[8*i+7] = vertex->z;
        }
        //graphics->face(num_edges, data);
      }
    }

    for (auto texture = vbo_vertex_data_by_texture.begin(); texture != vbo_vertex_data_by_texture.end(); ++texture) {
      std::string current_texture = (std::string) texture->first;
      textures->setTexture(current_texture);
      printf("This many faces: %d\n", vbo_vertex_data_by_texture[current_texture].size() / 9);
      int cache_id_by_texture = graphics->cacheFullMesh(
        vbo_vertex_data_by_texture[current_texture],
        vbo_normal_data_by_texture[current_texture],
        vbo_texture_data_by_texture[current_texture],
        vbo_color_data_by_texture[current_texture]);
      vbo_cache_ids_by_texture[current_texture] = cache_id_by_texture;
    }

    cache_id = 1;
  } else {
    for (auto texture = vbo_cache_ids_by_texture.begin(); texture != vbo_cache_ids_by_texture.end(); ++texture) {
      std::string current_texture = (std::string) texture->first;
      textures->setTexture(current_texture);
      graphics->drawFullMesh(vbo_cache_ids_by_texture[current_texture]);
    }
  }
}

void Model::oldRender() {
  graphics->color(1.0f, 1.0f, 1.0f, 1.0f);
  graphics->rotate(90.0f, 1.0f, 0.0f, 0.0f);

  if (cache_id == -1) {
    //cache_id = graphics->cacheProgram();
    //graphics->startCelShading();
    bool fill_model = true;
    if (fill_model) {
      for (auto component = component_names.begin(); component != component_names.end(); ++component) {
        std::string component_name = component->c_str();
        std::list<std::map<int, int>> current_faces = faces[component_name];

        std::map<int, Point*> current_vertices = vertices[component_name];
        std::map<int, Point*> current_texture_coords = texture_coords[component_name];
        std::map<int, Point*> current_normals = normals[component_name];

        for (auto face = current_faces.begin(); face != current_faces.end(); ++face) {
          int num_edges = -1;
          if (face->size() == 1) {
            textures->setTexture(texture_map[face->operator[](0)]);
            continue;
          } else if (face->size() == 12) {
            num_edges = 4;
          } else if (face->size() == 9) {
            num_edges = 3;
          }

          float data[num_edges * 8];
          for (int i = 0; i < num_edges; i++) {
            int start = 3*i + 1;

            Point* vertex = current_vertices[face->operator[](start)];
            Point* texture = current_texture_coords[face->operator[](start+1)];
            Point* normal = current_normals[face->operator[](start+2)];

            data[8*i] = texture->x;
            data[8*i+1] = -texture->y;
            data[8*i+2] = normal->x;
            data[8*i+3] = normal->y;
            data[8*i+4] = normal->z;
            data[8*i+5] = vertex->x;
            data[8*i+6] = vertex->y;
            data[8*i+7] = vertex->z;
          }
          //graphics->face(num_edges, data);
        }
      }
    }

    //graphics->stopCelShading();
    //graphics->startNormalShading();

    // bool use_this = true;
    // if (outline && use_this) {
    //   glPushAttrib(GL_ALL_ATTRIB_BITS);
    //   glEnable(GL_CULL_FACE);
    //   glPolygonMode(GL_BACK, GL_LINE);
    //   glLineWidth(outlineWidth);
    //   glDisable(GL_LIGHTING);

    //   glCullFace(GL_FRONT);
    //   glDepthFunc(GL_LEQUAL);

    //   glColor3fv(&outlineColor[0]);

    //   for (auto component = component_names.begin(); component != component_names.end(); ++component) {
    //     std::string component_name = component->c_str();
    //     std::list<std::map<int, int>> current_faces = faces[component_name];

    //     std::map<int, Point*> current_vertices = vertices[component_name];

    //     for (auto face = current_faces.begin(); face != current_faces.end(); ++face) {
    //       int num_edges = -1;
    //       if (face->size() == 1) {
    //         continue;
    //       } else if (face->size() == 12) {
    //         num_edges = 4;
    //         glBegin(GL_QUADS);
    //       } else if (face->size() == 9) {
    //         num_edges = 3;
    //         glBegin(GL_TRIANGLES);
    //       }

    //       for (int i = 0; i < num_edges; i++) {
    //         int start = 3*i + 1;
    //         Point* vertex = current_vertices[face->operator[](start)];
    //         glVertex3f(vertex->x, vertex->y, vertex->z);
    //       }
    //       glEnd();
    //     }
    //   }

    //   glPopAttrib();
    // }



    // bool use_this_instead = false;
    // if (outline && use_this_instead) {
    //   glPushAttrib(GL_ALL_ATTRIB_BITS);
    //   glEnable(GL_CULL_FACE);
    //   // glPolygonMode(GL_BACK, GL_LINE);
    //   // glLineWidth(outlineWidth);
    //   // glDisable(GL_TEXTURE_2D);
    //   glDisable(GL_LIGHTING);

    //   glCullFace(GL_FRONT);
    //   glDepthFunc(GL_LEQUAL);

    //   glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

    //   for (auto component = component_names.begin(); component != component_names.end(); ++component) {
    //     std::string component_name = component->c_str();
    //     std::list<std::map<int, int>> current_faces = faces[component_name];

    //     std::map<int, Point*> current_vertices = vertices[component_name];
    //     std::map<int, Point*> current_texture_coords = texture_coords[component_name];
    //     std::map<int, Point*> current_normals = normals[component_name];

    //     for (auto face = current_faces.begin(); face != current_faces.end(); ++face) {
    //       int num_edges = -1;
    //       if (face->size() == 1) {
    //         textures->setTexture(texture_map[face->operator[](0)]);
    //         continue;
    //       } else if (face->size() == 12) {
    //         num_edges = 4;
    //         glBegin(GL_QUADS);
    //       } else if (face->size() == 9) {
    //         num_edges = 3;
    //         glBegin(GL_TRIANGLES);
    //       }

    //       for (int i = 0; i < num_edges; i++) {
    //         int start = 3*i + 1;

    //         Point* vertex = current_vertices[face->operator[](start)];
    //         Point* texture = current_texture_coords[face->operator[](start+1)];
    //         Point* normal = current_normals[face->operator[](start+2)];

    //         glNormal3f(normal->x, normal->y, normal->z);
    //         glTexCoord2f(texture->x, -texture->y);
    //         glVertex3f(vertex->x + 0.15 * normal->x, vertex->y + 0.15 * normal->y, vertex->z + 0.15 * normal->z);
    //       }
    //       glEnd();
    //     }
    //   }

    //   glPopAttrib();
    // }

    //graphics->endCacheProgram();
  } else {
    //graphics->runCacheProgram(cache_id);
  }
}

