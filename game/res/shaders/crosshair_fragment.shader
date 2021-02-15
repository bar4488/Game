#version 460 core

out vec4 color;
in vec3 color_v;

void main(){
    color = vec4(color_v, 1.0);
}