// https://www.tomdalling.com/blog/modern-opengl/01-getting-started-in-xcode-and-visual-cpp/
// #version 150

// in vec3 vert;

// void main() {
//   // does not alter the vertices at all
//   gl_Position = vec4(vert, 1);
// }


// #version 330 core

// layout (location = 0) in vec3 position;
// layout (location = 1) in vec3 color;
// layout (location = 2) in vec2 vertTexCoord;

// out vec2 fragTexCoord;
// out vec3 ourColor;
// uniform mat4 transform;

// void main() {
//   gl_Position = transform * vec4(position, 1.0f);
//   fragTexCoord = vertTexCoord;
//   ourColor = color;
// }


// #version 330 core

// // Input vertex data, different for all executions of this shader.
// layout(location = 0) in vec3 vertexPosition_modelspace;
// layout(location = 1) in vec3 vertexColor;

// // Output data ; will be interpolated for each fragment.
// out vec3 fragmentColor;
// // Values that stay constant for the whole mesh.
// uniform mat4 MVP;

// void main(){  

//   // Output position of the vertex, in clip space : MVP * position
//   gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

//   // The color of each vertex will be interpolated
//   // to produce the color of each fragment
//   fragmentColor = vertexColor;
// }



#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 fragmentColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){

  // Output position of the vertex, in clip space : MVP * position
  gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

  // The color of each vertex will be interpolated
  // to produce the color of each fragment
  fragmentColor = vertexColor;
  
  // UV of the vertex. No special space for this one.
  UV = vertexUV;
}


