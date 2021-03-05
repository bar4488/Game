#version 460 core

layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform vec3 u_color;

void main(){
  gl_Position = MVP * vec4(position, 1.0);
}