#version 460 core

layout(location = 0) in vec3 position;

uniform float width;
uniform float height;

void main(){
  gl_Position = vec4(position * vec3(height / width, 1, 1), 1.0);
}