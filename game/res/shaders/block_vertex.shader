#version 460 core

struct vertex{
	vec3 position;
	vec2 tex;
	vec3 normals;
};

uniform mat4 MVP;
uniform mat4 M;

out vertex v_out;

uniform usamplerBuffer faces;

vertex computeVertex(int index) {
	int face = index / 6;
	int vert = index % 6;
	vec3 position = vec3(texelFetch(faces, 5*face).r, texelFetch(faces, 5*face + 1).r, int(texelFetch(faces, 5*face + 2).r));
	int normal_dir = int(texelFetch(faces, 5*face + 3).r);
	int texID = int(texelFetch(faces, 5*face + 4).r);
	vec3 normal;
	vec2 tex;
	vec2 offset;
	switch (vert) {
	case 0:
		offset = vec2(0, 0);
		break;
	case 1:
		offset = vec2(1, 0);
		break;
	case 2:
		offset = vec2(1, 1);
		break;
	case 3:
		offset = vec2(0, 0);
		break;
	case 4:
		offset = vec2(1, 1);
		break;
	case 5:
		offset = vec2(0, 1);
		break;
	}
	switch (normal_dir) {
	case 0:
		//up
		normal = vec3(0, 1, 0);
		tex = vec2(1,0);
		tex.xy += offset;
		position += vec3(0, 1, 0);
		position.zx += offset;
		break;
	case 1:
		//down
		normal = vec3(0, -1, 0);
		tex = vec2(2,0);
		tex.yx += offset;
		position.xz += offset;
		break;
	case 2:
		//right
		normal = vec3(1, 0, 0);
		tex = vec2(0,0);
		tex.yx += offset;
		position += vec3(1, 0, 0);
		position.yz += offset;
		break;
	case 3:
		//left
		normal = vec3(-1, 0, 0);
		tex = vec2(0,0);
		tex.xy += offset;
		position.zy += offset;
		break;
	case 4:
		//forward
		normal = vec3(0, 0, 1);
		tex = vec2(0,0);
		tex.xy += offset;
		position += vec3(0, 0, 1);
		position.xy += offset;
		break;
	case 5:
		//backward
		normal = vec3(0, 0, -1);
		tex = vec2(0,0);
		tex.yx += offset;
		position.yx += offset;
		break;
	}
	tex.y *= 3;
	return vertex(position, tex, normal);
}

void main(){
	vertex v = computeVertex(gl_VertexID);
	gl_Position = MVP * vec4(v.position, 1.0);
	v.position = vec3(M * vec4(v.position, 1.0));
	v_out = v;
}