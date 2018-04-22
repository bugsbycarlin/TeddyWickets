/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "control_map.h"

ControlMap::ControlMap() {
  default_control_map = {};
  control_map = {};
  ordered_controls = {
    "player_1_up",
    "player_1_down",
    "player_1_left",
    "player_1_right",
    "player_1_shoot_accept",
    "player_1_cancel_switch",
    "player_1_special",
    "player_1_view_taunt",
    "player_2_up",
    "player_2_down",
    "player_2_left",
    "player_2_right",
    "player_2_shoot_accept",
    "player_2_cancel_switch",
    "player_2_special",
    "player_2_view_taunt"
  };

  default_control_map["player_1_up"] = "Key Up";
  default_control_map["player_1_down"] = "Key Down";
  default_control_map["player_1_left"] = "Key Left";
  default_control_map["player_1_right"] = "Key Right";
  default_control_map["player_1_shoot_accept"] = "Key X";
  default_control_map["player_1_cancel_switch"] = "Key Z";
  default_control_map["player_1_special"] = "Key S";
  default_control_map["player_1_view_taunt"] = "Key A";
  default_control_map["player_2_up"] = "Key Up";
  default_control_map["player_2_down"] = "Key Down";
  default_control_map["player_2_left"] = "Key Left";
  default_control_map["player_2_right"] = "Key Right";
  default_control_map["player_2_shoot_accept"] = "Key X";
  default_control_map["player_2_cancel_switch"] = "Key Z";
  default_control_map["player_2_special"] = "Key S";
  default_control_map["player_2_view_taunt"] = "Key A";

  std::ifstream key_map_file;
  key_map_file.open("control_map.txt");

  if (!key_map_file.is_open()) {
    printf("Failed to load key map, using default.");
    for (auto item = default_control_map.begin(); item != default_control_map.end(); ++item) {
      control_map[item->first] = default_control_map[item->first];
    }
  } else {
    std::regex key_map_regex("([^:]+):([^:]+)");

    std::string line;
    std::smatch match;
    while (getline(key_map_file, line)) {
      if (std::regex_search(line, match, key_map_regex)) {
        printf("Found key map: %s to %s\n", match.str(1).c_str(), match.str(2).c_str());
        control_map[match.str(1)] = match.str(2);
      }
    }

    key_map_file.close();
  }


  textures->addTexture("keyboard", "keyboard.png");
  textures->addTexture("pad_up", "pad_up.png");
  textures->addTexture("pad_down", "pad_down.png");
  textures->addTexture("pad_left", "pad_left.png");
  textures->addTexture("pad_right", "pad_up.png");
  textures->addTexture("button_1", "button_1.png");
  textures->addTexture("button_2", "button_2.png");
  textures->addTexture("button_3", "button_3.png");
  textures->addTexture("button_4", "button_4.png");

  info_box = new TextBox("candy_crayon.ttf", 40,
    "A", 0, 0, 0, 0, 0);

  info_box_small = new TextBox("candy_crayon.ttf", 22,
    "A", 0, 0, 0, 0, 0);
}

// This method is the key map
std::string ControlMap::getAction(std::string input) {
  if (control_map.find(input) == control_map.end()) {
    return "";
  } else {
    return control_map[input];
  }
}

void ControlMap::swapMapsAndSave() {
  control_map = new_control_map;

  std::ofstream key_map_file;
  key_map_file.open("control_map.txt");

  for (auto item = ordered_controls.begin(); item != ordered_controls.end(); ++item) {
    std::string output = *item + ":" + control_map[*item] + "\n";
    key_map_file << output;
  }
  key_map_file.close();
}

std::string ControlMap::translateControllerEvent(SDL_Event e) {
  std::string prefix = "P";
  if (e.type == SDL_JOYBUTTONDOWN) {
    return prefix + std::to_string(e.jbutton.which) + " Button " + std::to_string(e.jbutton.button);
  } else if (e.type == SDL_JOYAXISMOTION && e.jaxis.axis < 4) {
    if (e.jaxis.axis % 2 == 0 && e.jaxis.value < -3200) {
      // stick left
      return prefix + std::to_string(e.jaxis.which) + " " + std::to_string(e.jaxis.axis / 2) + " Stick Left";
    } else if (e.jaxis.axis % 2 == 0 && e.jaxis.value > 3200) {
      // stick right
      return prefix + std::to_string(e.jaxis.which) + " " + std::to_string(e.jaxis.axis / 2) + " Stick Right";
    } else if (e.jaxis.axis % 2 == 1 && e.jaxis.value < -3200) {
      // stick up
      return prefix + std::to_string(e.jaxis.which) + " " + std::to_string(e.jaxis.axis / 2) + " Stick Up";
    } else if (e.jaxis.axis % 2 == 1 && e.jaxis.value > 3200) {
      // stick down
      return prefix + std::to_string(e.jaxis.which) + " " + std::to_string(e.jaxis.axis / 2) + " Stick Down";
    }
  } else if (e.type == SDL_JOYHATMOTION) {
    if (e.jhat.value == SDL_HAT_UP) {
      return prefix + std::to_string(e.jaxis.which) + " Dpad Up";
    } else if (e.jhat.value == SDL_HAT_DOWN) {
      return prefix + std::to_string(e.jaxis.which) + " Dpad Down";
    } else if (e.jhat.value == SDL_HAT_LEFT) {
      return prefix + std::to_string(e.jaxis.which) + " Dpad Left";
    } else if (e.jhat.value == SDL_HAT_RIGHT) {
      return prefix + std::to_string(e.jaxis.which) + " Dpad Right";
    }
  }

  return "Unknown";
}

std::string ControlMap::translateKeyEvent(SDL_Event e) {
  std::string short_name = "Unknown";
  switch (e.key.keysym.sym) {
    case SDLK_UP : short_name = "Up"; break;
    case SDLK_DOWN : short_name = "Down"; break;
    case SDLK_RIGHT : short_name = "Right"; break;
    case SDLK_LEFT : short_name = "Left"; break;
    case SDLK_BACKSPACE : short_name = "Backspace"; break;
    case SDLK_TAB : short_name = "Tab"; break;
    case SDLK_SPACE : short_name = "Space"; break;
    case SDLK_0 : short_name = "0"; break;
    case SDLK_1 : short_name = "1"; break;
    case SDLK_2 : short_name = "2"; break;
    case SDLK_3 : short_name = "3"; break;
    case SDLK_4 : short_name = "4"; break;
    case SDLK_5 : short_name = "5"; break;
    case SDLK_6 : short_name = "6"; break;
    case SDLK_7 : short_name = "7"; break;
    case SDLK_8 : short_name = "8"; break;
    case SDLK_9 : short_name = "9"; break;
    case SDLK_KP_0 : short_name = "0"; break;
    case SDLK_KP_1 : short_name = "1"; break;
    case SDLK_KP_2 : short_name = "2"; break;
    case SDLK_KP_3 : short_name = "3"; break;
    case SDLK_KP_4 : short_name = "4"; break;
    case SDLK_KP_5 : short_name = "5"; break;
    case SDLK_KP_6 : short_name = "6"; break;
    case SDLK_KP_7 : short_name = "7"; break;
    case SDLK_KP_8 : short_name = "8"; break;
    case SDLK_KP_9 : short_name = "9"; break;
    case SDLK_a : short_name = "A"; break;
    case SDLK_b : short_name = "B"; break;
    case SDLK_c : short_name = "C"; break;
    case SDLK_d : short_name = "D"; break;
    case SDLK_e : short_name = "E"; break;
    case SDLK_f : short_name = "F"; break;
    case SDLK_g : short_name = "G"; break;
    case SDLK_h : short_name = "H"; break;
    case SDLK_i : short_name = "I"; break;
    case SDLK_j : short_name = "J"; break;
    case SDLK_k : short_name = "K"; break;
    case SDLK_l : short_name = "L"; break;
    case SDLK_m : short_name = "M"; break;
    case SDLK_n : short_name = "N"; break;
    case SDLK_o : short_name = "O"; break;
    case SDLK_p : short_name = "P"; break;
    case SDLK_q : short_name = "Q"; break;
    case SDLK_r : short_name = "R"; break;
    case SDLK_s : short_name = "S"; break;
    case SDLK_t : short_name = "T"; break;
    case SDLK_u : short_name = "U"; break;
    case SDLK_v : short_name = "V"; break;
    case SDLK_w : short_name = "W"; break;
    case SDLK_x : short_name = "X"; break;
    case SDLK_y : short_name = "Y"; break;
    case SDLK_z : short_name = "Z"; break;
  }
  return "Key " + short_name;
}

unsigned long ControlMap::hash(const std::string& str) {
    unsigned long hash = 5381;
    for (size_t i = 0; i < str.size(); ++i)
        hash = 33 * hash + (unsigned char)str[i];
    return hash;
}

void ControlMap::render(int x, int y, std::string action) {
  std::string input_value = getAction(action);
  if (input_value == "") return;

  int last = (int) input_value.find_last_of(" ");
  std::string key = input_value.substr(last+1);
  std::string root = input_value.substr(0,last);
  int second_to_last = (int) root.find_last_of(" ");
  if (second_to_last >= root.size()) {
    second_to_last = -1;
  }
  std::string button_type = root.substr(second_to_last+1);

  //printf("--Key:%s Type:%s--\n", key.c_str(), button_type.c_str());
  bool render_text = true;

  if (button_type == "Key") {
    textures->setTexture("keyboard");
  } else if (button_type == "Dpad" || button_type == "Stick") {
    if (key == "Left") textures->setTexture("pad_left");
    if (key == "Right") textures->setTexture("pad_right");
    if (key == "Up") textures->setTexture("pad_up");
    if (key == "Down") textures->setTexture("pad_down");
    render_text = false;
  } else if (button_type == "Button") {
    int button_value = hash(key);
    textures->setTexture("button_" + std::to_string(button_value % 4 + 1));
  } else {
    //printf("Haven't implemented this glyph method\n");
    //printf("Value: %s\n", input_value.c_str());
    textures->setTexture("keyboard");
  }
  graphics->rectangle(x, y, 103, 103);

  if (render_text) {
    if (key.length() < 2) {
      info_box->x = x + 25;
      info_box->y = y + 18;
      info_box->setText(key);
      info_box->render();
    } else {
      info_box_small->x = x + 17;
      info_box_small->y = y + 22;
      info_box_small->setText(key);
      info_box_small->render();
    }
  }
}
