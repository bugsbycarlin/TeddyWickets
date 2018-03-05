#version 330 core

uniform mat4 mvp_matrix;

uniform float bool_lighting;
uniform float bool_cel_shading;

layout(location = 0) in vec3 position_vector;
layout(location = 1) in vec3 normal_vector;
layout(location = 2) in vec4 color_vector;
layout(location = 3) in vec2 texture_vector;

out vec2 fragment_texture_vector;
out vec4 fragment_color_vector;

void main(){
  gl_Position =  mvp_matrix * vec4(position_vector,1);
  fragment_color_vector = color_vector;
  fragment_texture_vector = texture_vector;
}
