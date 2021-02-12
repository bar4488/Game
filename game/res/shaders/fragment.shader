#version 460 core

out vec4 color;
in vec2 v_texCoords;
in vec3 v_fragPos;
in vec3 v_normals;

uniform sampler2D tex;
uniform vec3 lightDir;
uniform vec3 lightColor;

void main(){
    vec3 norm = normalize(v_normals);
    vec3 n_lightDir = normalize(lightDir);

    float diff = max(dot(norm, n_lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;

    vec4 texColor = texture(tex, v_texCoords);
    vec4 result = vec4(ambient + diffuse, 1.0) * texColor;
    color = texColor;
}