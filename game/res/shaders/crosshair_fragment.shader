#version 460 core

out vec4 color;
in vec2 v_texCoords;
in vec3 v_fragPos;
in vec3 v_normals;

uniform sampler2D tex;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main(){
    vec3 norm = normalize(v_normals);
    vec3 n_lightDir = normalize(lightDir);

    // diffuse
    float diff = max(dot(norm, n_lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 12);
    float specularStrength = 0.15;
    vec3 specular = specularStrength * spec * lightColor;

    vec4 texColor = texture(tex, v_texCoords / 3);
    vec4 result = vec4(ambient + diffuse + specular, 1.0) * texColor;
    color = result;
}