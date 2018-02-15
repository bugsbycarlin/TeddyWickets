// https://www.tomdalling.com/blog/modern-opengl/01-getting-started-in-xcode-and-visual-cpp/
#version 150

in vec3 vert;

void main() {
  // does not alter the vertices at all
  gl_Position = vec4(vert, 1);
}