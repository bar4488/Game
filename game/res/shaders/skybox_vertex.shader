#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;

uniform mat4 VP;

out vec3 v_col;
out vec3 v_fragPos;

void main(){
  gl_Position = (VP * vec4(position, 1.0)).xyww;
  v_col = (normalize(position) + vec3(1,1,3)) / 4;
  v_fragPos = vec3(vec4(position, 1.0));
}