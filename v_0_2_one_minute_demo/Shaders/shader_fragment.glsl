#version 330 core

uniform sampler2D texture_sampler;
uniform vec4 global_color_vector;

in vec2 fragment_texture_vector;
in vec4 fragment_color_vector;

out vec4 final_color_vector;

void main(){
  final_color_vector = global_color_vector * texture(texture_sampler, fragment_texture_vector) * fragment_color_vector;
}