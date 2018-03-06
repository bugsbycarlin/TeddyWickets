#version 330 core

// Lighting calculations from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/#modeling-the-light

uniform sampler2D texture_sampler;
uniform vec4 global_color_vector;
uniform mat4 v_matrix;
uniform mat4 m_matrix;

uniform float bool_lighting;
uniform float bool_cel_shading;

uniform vec3 light_position_worldspace;

in vec2 fragment_texture_vector;
in vec4 fragment_color_vector;

in vec3 position_vector_worldspace;
in vec3 normal_vector_cameraspace;
in vec3 eye_direction_vector_cameraspace;
in vec3 light_direction_vector_cameraspace;

out vec4 final_color_vector;

float cel_step(in float color_value) {
    float new_color_value;
    // if (color_value > 0.9) {
    //   new_color_value = 0.95;
    // } else if (color_value > 0.8) {
    //   new_color_value = 0.85;
    // } else if (color_value > 0.7) {
    //   new_color_value = 0.75;
    // } else if (color_value > 0.6) {
    //   new_color_value = 0.65;
    // } else if (color_value > 0.5) {
    //   new_color_value = 0.55;
    // } else if (color_value > 0.4) {
    //   new_color_value = 0.45;
    // } else if (color_value > 0.3) {
    //   new_color_value = 0.35;
    // } else if (color_value > 0.2) {
    //   new_color_value = 0.25;
    // } else if (color_value > 0.1) {
    //   new_color_value = 0.15;
    // } else {
    //   new_color_value = 0;
    // }

    // if (color_value > 0.9) {
    //   new_color_value = 1.0;
    // } else if (color_value > 0.6) {
    //   new_color_value = 0.6;
    // } else if (color_value > 0.2) {
    //   new_color_value = 0.2;
    // } else {
    //   new_color_value = 0;
    // }

    if (color_value < 0.1) {
      new_color_value = 0;
    } else if(color_value < 0.3) {
      new_color_value = 0.3;
    } else if(color_value < 0.6) {
      new_color_value = 0.6;
    } else {
      new_color_value = 0.98;
    }

    return new_color_value;
}

void main(){
  if (bool_lighting < 0.5) {
    final_color_vector = global_color_vector * texture(texture_sampler, fragment_texture_vector) * fragment_color_vector;
  } else {
    //final_color_vector = global_color_vector * texture(texture_sampler, fragment_texture_vector) * fragment_color_vector;

    vec3 light_color = vec3(1, 1, 1);
    // spotlight needs power, directional doesn't (ish)
    //float light_power = 1.0f;
    float light_power = 50.0f;

    vec3 material_diffuse_color = (global_color_vector * texture(texture_sampler, fragment_texture_vector) * fragment_color_vector).rgb;
    vec3 material_ambient_color = vec3(0.3, 0.3, 0.3) * material_diffuse_color;
    vec3 material_specular_color = vec3(0.1, 0.1, 0.1);

    // fix distance to 1.0 to get a directional light, along with the mods in the shader.
    float distance = length(light_position_worldspace - position_vector_worldspace);
    //float distance = 1.0f;

    vec3 n = normalize(normal_vector_cameraspace);
    vec3 l = normalize(light_direction_vector_cameraspace);

    // Cosine of the angle between the normal and the light direction, 
    // clamped above 0
    //  - light is at the vertical of the triangle -> 1
    //  - light is perpendicular to the triangle -> 0
    //  - light is behind the triangle -> 0
    float cos_theta = clamp(dot(n, l), 0, 1);

    vec3 eye = normalize(eye_direction_vector_cameraspace);
    vec3 reflection = reflect(-l,n);

    // Cosine of the angle between the Eye vector and the Reflect vector,
    // clamped to 0
    //  - Looking into the reflection -> 1
    //  - Looking elsewhere -> < 1
    float cos_alpha = clamp(dot(eye, reflection), 0, 1);

    vec3 almost_final_color = 
      // Ambient : simulates indirect lighting
      material_ambient_color +
      // Diffuse : "color" of the object
      material_diffuse_color * light_color * light_power * cos_theta / (distance * distance) +
      // Specular : reflective highlight, like a mirror
      material_specular_color * light_color * light_power * pow(cos_alpha, 5) / (distance * distance);

    if (bool_cel_shading > 0.5) {
      almost_final_color = vec3(cel_step(almost_final_color.x), cel_step(almost_final_color.y), cel_step(almost_final_color.z));
    }

    final_color_vector = vec4(almost_final_color, 1.0);
  }
}