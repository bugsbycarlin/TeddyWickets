#version 330 core

// Lighting calculations from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/#modeling-the-light

uniform mat4 mvp_matrix;
uniform mat4 v_matrix;
uniform mat4 m_matrix;

uniform float bool_lighting;
uniform float bool_cel_shading;
uniform vec3 light_position_worldspace;

layout(location = 0) in vec3 position_vector;
layout(location = 1) in vec3 normal_vector;
layout(location = 2) in vec4 color_vector;
layout(location = 3) in vec2 texture_vector;

out vec2 fragment_texture_vector;
out vec4 fragment_color_vector;

out vec3 position_vector_worldspace;
out vec3 normal_vector_cameraspace;
out vec3 eye_direction_vector_cameraspace;
out vec3 light_direction_vector_cameraspace;


void main(){
  gl_Position =  mvp_matrix * vec4(position_vector,1);
  fragment_color_vector = color_vector;
  fragment_texture_vector = texture_vector;

  position_vector_worldspace = (m_matrix * vec4(position_vector,1)).xyz;

  vec3 position_vector_cameraspace = (v_matrix * m_matrix * vec4(position_vector,1)).xyz;
  eye_direction_vector_cameraspace = vec3(0,0,0) - position_vector_cameraspace;

  vec3 light_position_cameraspace = (v_matrix * vec4(light_position_worldspace,1)).xyz;
  light_direction_vector_cameraspace = light_position_cameraspace + eye_direction_vector_cameraspace;

  normal_vector_cameraspace = (v_matrix * transpose(inverse(m_matrix)) * vec4(normal_vector,0)).xyz;
}
