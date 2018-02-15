// https://www.tomdalling.com/blog/modern-opengl/01-getting-started-in-xcode-and-visual-cpp/
// #version 150

// in vec3 vert;

// void main() {
//   // does not alter the vertices at all
//   gl_Position = vec4(vert, 1);
// }


#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 vertTexCoord;

out vec2 fragTexCoord;
out vec3 ourColor;
uniform mat4 transform;

void main() {
  gl_Position = transform * vec4(position, 1.0f);
  fragTexCoord = vertTexCoord;
  ourColor = color;
}