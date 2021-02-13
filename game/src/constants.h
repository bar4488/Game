#include "GL/glew.h"

const GLbyte skybox_vertices[] = {
	// Right face 0-4
	1, 1, 1, 1, 0, 0,
	1, -1, 1, 1, 0, 0,
	1, -1, -1, 1, 0, 0,
	1, 1, -1, 1, 0, 0,

	// Up face 4-8
	1, 1, 1, 0, 1, 0,
	1, 1, -1, 0, 1, 0,
	-1, 1, -1, 0, 1, 0,
	-1, 1, 1,  0, 1, 0,

	// Forward face 8-12
	1, 1, 1, 0, 0, 1,
	-1, -1, 1, 0, 0, 1,
	1, -1, 1, 0, 0, 1,
	-1, 1, 1, 0, 0, 1,

	// Left face 12-16
	-1, -1, -1, -1, 0, 0,
	-1, 1, 1, -1, 0, 0,
	-1, 1, -1, -1, 0, 0,
	-1, -1, 1, -1, 0, 0,

	// Down face 16-20
	-1, -1, -1, 0, -1, 0,
	1, -1, 1, 0, -1, 0,
	1, -1, -1, 0, -1, 0,
	-1, -1, 1, 0, -1, 0,


	// Backward 20-24
	-1, -1, -1, 0, 0, -1,
	1, 1, -1, 0, 0, -1,
	1, -1, -1, 0, 0, -1,
	-1, 1, -1, 0, 0, -1
};

constexpr GLubyte skybox_indices[] = {
	// rf 0-6
	0, 1, 2,
	0, 2, 3,
	// uf 6-12
	4, 5, 6,
	4, 6, 7,
	// ff 12-18
	8, 9, 10,
	8, 11, 9,
	// lf 18-24
	12, 13, 14,
	12, 15, 13,
	// df 24-30
	16, 18, 17,
	16, 17, 19,
	// bf 30-36
	20,21,22,
	20,23,21
};
