#version 460 core

layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform vec3 u_color;

void main(){
  vec4 p = MVP * vec4(position, 1.0);
  gl_Position = vec4(p.x, p.y, p.z, p.w);
}