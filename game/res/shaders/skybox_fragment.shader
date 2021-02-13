#version 460 core

out vec4 color;
in vec3 v_fragPos;
in vec3 v_col;

uniform vec3 lightDir;
uniform vec3 lightColor;

void main(){

    vec3 n_lightDir = normalize(lightDir);
    // specular
    vec3 n_vd = normalize(v_fragPos);
    float spec = pow(max(dot(n_vd, n_lightDir), 0), 64);
    float specularStrength = 0.7;
    vec3 specular = specularStrength * spec * lightColor;

    //vec4 result = vec4(ambient + diffuse + specular, 1.0) * vec4(0.2, 0.2, 0.8, 1.0);
    vec4 result = vec4(specular, 1.0) + vec4(0.2, 0.5, 0.8, 1.0);
    color = vec4(specular, 1.0) + vec4(0.2, 0.5, 0.8, 1.0) + vec4(v_col, 1);
}