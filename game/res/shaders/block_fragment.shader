#version 460 core

struct vertex{
	vec3 position;
	vec2 tex;
	vec3 normals;
};

out vec4 color;
in vertex v_out;

uniform sampler2D tex;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main(){
    vec3 norm = normalize(v_out.normals);
    vec3 n_lightDir = normalize(lightDir);

    // diffuse
    float diff = max(dot(norm, n_lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - v_out.position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 17);
    float specularStrength = 0.07;
    vec3 specular = specularStrength * spec * lightColor;

    vec4 texColor = texture(tex, v_out.tex/ 3);
    vec4 result = vec4(ambient + 0.7 * diffuse + 0.7 * specular, 1.0) * texColor;
    
    color = result;
}