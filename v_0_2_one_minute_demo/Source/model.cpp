/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "model.h"

int Model::next_display_list_index = 1;

Model::Model(Textures* textures, std::string model_file_name) {
  this->textures = textures;

  std::string line;
  std::smatch match;

  this->display_list_index = -1;

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
  while(getline(material_file, line)) {
    if (std::regex_search(line, match, new_material_regex)) {
      printf("Matched material: %s\n", match.str(1).c_str());
      last_texture_name = match.str(1);
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

  while(getline(model_file, line)) {
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
      // printf("Matched texture coords: %s\n", line.c_str());
      // printf("%f\n", t1);
      // printf("%f\n", t2);
      current_texture_coords[texture_coord_counter] = new Point(t1, t2, 0);
      texture_coord_counter += 1;
    } else if (std::regex_search(line, match, normal_regex)) {
      float n1 = ::atof(match.str(1).c_str());
      float n2 = ::atof(match.str(2).c_str());
      float n3 = ::atof(match.str(3).c_str());
      // printf("Matched normal coords: %s\n", line.c_str());
      // printf("%f\n", n1);
      // printf("%f\n", n2);
      // printf("%f\n", n3);
      current_normals[normal_counter] = new Point(n1, n2, n3);
      normal_counter += 1;
    } else if (std::regex_search(line, match, material_regex)) {
      //printf("Matched a material: %s\n", line.c_str());
      //printf("Material name is %s\n", match.str(1).c_str());
      std::string name = texture_names[match.str(1)];
      //printf("Texture num is %d\n", texture_keys[name]);
      //printf("Texture name is %s\n", texture_map[texture_keys[name]].c_str());
      std::map<int, int> face = {};
      face[0] = texture_keys[name];
      current_faces.push_back(face);
    } else if (std::regex_search(line, match, four_point_face_regex)) {
      //printf("Matched a 4 point face: %s\n", line.c_str());
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
      //printf("Matched a 3 point face: %s\n", line.c_str());
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
      // printf("Matched component name: %s\n", line.c_str());
      // printf("%s\n", match.str(1).c_str());
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

void Model::render() {
  //GLUquadric* ball = gluNewQuadric();
  //gluQuadricTexture(ball, GL_TRUE);
  //gluQuadricNormals(ball, GLU_SMOOTH);

  //textures->setTexture("bear_face");
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glRotatef(90.0f,1.0f, 0.0f, 0.0f);
  //gluSphere(ball, 0.75f, 20, 20);

  //printf("Rendering\n");
  if (display_list_index == -1) {
    display_list_index = next_display_list_index;
    next_display_list_index++;

    glNewList(display_list_index, GL_COMPILE);
    for (auto component_pointer = component_names.begin(); component_pointer != component_names.end(); ++component_pointer) {
      std::string component_name = component_pointer->c_str();
      std::list<std::map<int, int>> current_faces = faces[component_name];

      //printf("Component %s\n", component_name.c_str());

      std::map<int, Point*> current_vertices = vertices[component_name];
      std::map<int, Point*> current_texture_coords = texture_coords[component_name];
      std::map<int, Point*> current_normals = normals[component_name];

      for (auto face = current_faces.begin(); face != current_faces.end(); ++face) {
        //printf("faaace %d\n", face->size());
        int num_faces = -1;
        if (face->size() == 1) {
          // printf("Face\n");
          // printf("Face is %d\n", face->operator[](0));
          // printf("Texture map is %s\n", texture_map[face->operator[](0)].c_str());
          textures->setTexture(texture_map[face->operator[](0)]);
          continue;
        }
        else if (face->size() == 12) {
          num_faces = 4;
          glBegin(GL_QUADS);
        } else if (face->size() == 9) {
          num_faces = 3;
          glBegin(GL_TRIANGLES);
        }
        
        for(int i = 0; i < num_faces; i++) {
          int start = 3*i + 1;

          Point* vertex = current_vertices[face->operator[](start)];
          Point* texture = current_texture_coords[face->operator[](start+1)];
          Point* normal = current_normals[face->operator[](start+2)];

          glNormal3f(normal->x, normal->y, normal->z);
          glTexCoord2f(texture->x, -texture->y);
          glVertex3f(vertex->x, vertex->y, vertex->z);
          
        }
        glEnd();
      }
    }
    glEndList();
  } else {
    glCallList(display_list_index);
  }

}

