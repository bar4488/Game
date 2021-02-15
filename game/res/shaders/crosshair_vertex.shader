#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 view;

out vec3 color_v;

void main(){
  gl_Position = view * vec4(position, 1.0);
  color_v = color;
}