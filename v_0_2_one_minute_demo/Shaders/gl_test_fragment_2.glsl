// https://www.tomdalling.com/blog/modern-opengl/01-getting-started-in-xcode-and-visual-cpp/
// #version 150

// out vec4 finalColor;

// void main() {
//     //set every drawn pixel to white
//     finalColor = vec4(1.0, 1.0, 1.0, 1.0);
// }



#version 330 core

in vec3 ourColor;
out vec4 color;

void main() {
  color = vec4(ourColor, 1.0f);
}