/*

  Teddy Wickets
  Copyright 2017 - Matthew Carlin

*/

#include "hot_config.h"

HotConfig* hot_config = new HotConfig();

using namespace tinyxml2;

HotConfig::HotConfig() {

  XMLDocument doc;
  XMLError result = doc.LoadFile("config.txt");

  XMLElement * element = doc.FirstChildElement("param");
  while (element != nullptr) {
    std::string element_type(element->FirstChildElement("type")->GetText());
    std::string element_name(element->FirstChildElement("name")->GetText());

    printf("Type and name: %s %s\n", element_type.c_str(), element_name.c_str());

    if (element_type == "int") {
      int value;
      element->FirstChildElement("value")->QueryIntText(&value);
      ints[element_name] = value;
    } else if (element_type == "float") {
      float value;
      element->FirstChildElement("value")->QueryFloatText(&value);
      floats[element_name] = value;
    } else if (element_type == "string") {
      std::string value(element->FirstChildElement("value")->GetText());
      strings[element_name] = value;
    }

    element = element->NextSiblingElement("param");
  }

}

float HotConfig::getFloat(std::string name) {
  return floats[name];
}
  
int HotConfig::getInt(std::string name) {
  return ints[name];
}

std::string HotConfig::getString(std::string name) {
  return strings[name];
}
