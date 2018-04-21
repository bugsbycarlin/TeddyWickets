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

vec4 cel_step(in vec4 light_color, in vec4 true_color) {
    float color_fraction;
    // if (true_color.r > 0) color_fraction += clamp(light_color.r / true_color.r, 1.2, 0);
    // if (true_color.g > 0) color_fraction += clamp(light_color.g / true_color.g, 1.2, 0);
    // if (true_color.b > 0) color_fraction += clamp(light_color.b / true_color.b, 1.2, 0);
    // color_fraction = color_fraction / 3.0;
    color_fraction = clamp((light_color.x + light_color.y + light_color.z) / 1.8, 0, 1);

    float new_color_fraction;

    if (color_fraction < 0.1) {
      new_color_fraction = 0.3;
    } else if(color_fraction < 0.3) {
      new_color_fraction = 0.5;
    } else if(color_fraction < 0.6) {
      new_color_fraction = 0.8;
    } else if(color_fraction < 0.8) {
      new_color_fraction = 0.9;
    } else {
      new_color_fraction = 0.98;
    }

    float alpha = 1.0;
    if (true_color.a < 0.8 && true_color.a > 0.2) {
      alpha = 0.5;
    } else if (true_color.a <= 0.2) {
      alpha = 0.0;
    }
    vec4 adjusted_color = vec4(new_color_fraction * true_color.r, new_color_fraction * true_color.g, new_color_fraction * true_color.b, alpha);
    return adjusted_color;
}

void main(){
  if (bool_lighting < 0.5) {
    final_color_vector = global_color_vector * texture(texture_sampler, fragment_texture_vector) * fragment_color_vector;
  } else {
    //final_color_vector = global_color_vector * texture(texture_sampler, fragment_texture_vector) * fragment_color_vector;

    vec4 light_color = vec4(1, 1, 1, 1);
    // spotlight needs power, directional doesn't (ish)
    float light_power = 1.0f;
    //float light_power = 50.0f;

    vec4 material_diffuse_color = (global_color_vector * texture(texture_sampler, fragment_texture_vector) * fragment_color_vector);
    vec4 material_ambient_color = vec4(0.3, 0.3, 0.3, 1.0) * material_diffuse_color;
    vec4 material_specular_color = vec4(0.0, 0.0, 0.0, 1.0);

    // fix distance to 1.0 to get a directional light, along with the mods in the shader.
    //float distance = length(light_position_worldspace - position_vector_worldspace);
    float distance = 1.0f;

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

    vec4 almost_final_color = 
      // Ambient : simulates indirect lighting
      material_ambient_color +
      // Diffuse : "color" of the object
      material_diffuse_color * light_color * light_power * cos_theta / (distance * distance) +
      // Specular : reflective highlight, like a mirror
      material_specular_color * light_color * light_power * pow(cos_alpha, 5) / (distance * distance);

    //if (bool_cel_shading > 0.5) {
      //float intensity = cel_step(clamp((almost_final_color.x + almost_final_color.y + almost_final_color.z) / 1.8, 0, 1));
      //float intensity = 0.75;
      //almost_final_color = intensity * almost_final_color;
      almost_final_color = cel_step(almost_final_color, (global_color_vector * texture(texture_sampler, fragment_texture_vector)));
    //}

    final_color_vector = almost_final_color;
  }
}