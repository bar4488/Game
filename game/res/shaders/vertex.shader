#version 460 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 normals;

uniform mat4 VP;
uniform mat4 M;

out vec2 v_texCoords;
out vec3 v_normals;
out vec3 v_fragPos;

void main(){
  gl_Position = VP * M * vec4(position, 1.0);
  v_texCoords = texCoords;
  v_normals = normals;
  v_fragPos = vec3(M * vec4(position, 1.0));
}