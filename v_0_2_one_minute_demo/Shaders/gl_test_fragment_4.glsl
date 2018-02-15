// https://www.tomdalling.com/blog/modern-opengl/01-getting-started-in-xcode-and-visual-cpp/
// #version 150

// out vec4 finalColor;

// void main() {
//     //set every drawn pixel to white
//     finalColor = vec4(1.0, 1.0, 1.0, 1.0);
// }



// #version 330 core

// uniform sampler2D myTextureSampler;
// in vec2 fragTexCoord;
// in vec3 ourColor;
// out vec4 color;

// void main() {
//   //color = vec4(ourColor, 1.0f);
//   //color = texture(tex, fragTexCoord)
//   color = texture(myTextureSampler, fragTexCoord) * vec4(ourColor, 1.0f);
// }


// #version 330 core

// // Interpolated values from the vertex shaders
// in vec3 fragmentColor;

// // Ouput data
// out vec3 color;

// void main(){

//   // Output color = color specified in the vertex shader, 
//   // interpolated between all 3 surrounding vertices
//   color = fragmentColor;

// }


#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 fragmentColor;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){

  // Output color = color of the texture at the specified UV
  color = texture( myTextureSampler, UV ).rgb * fragmentColor;
  //color = vec3(UV.t, UV.t, 1.0);
}