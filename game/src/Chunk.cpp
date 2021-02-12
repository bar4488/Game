//
// Created by bar44 on 11/3/2020.
//

#include "Chunk.h"

#include <glm/glm.hpp>
#include <vector>

#include <ctime> 

#include <glm/gtc/matrix_transform.hpp>
#include "graphics/VertexArray.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"

Chunk::Chunk(glm::vec3 position, VertexArray *vao, VertexBuffer *vb, IndexBuffer *ib)
	: m_Position(position),
	m_VertexArray(vao),
	m_VertexBuffer(vb),
	m_IndexBuffer(ib)
{
	if (position.y != 0) return;
	srand((unsigned)time(0));
	for (auto& x : m_ChunkData) {
		for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
			for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
				x[y][z] = rand() % 5 == 1 ? 1 : 0;
			}
		}
	}

	// we should load the chunk data to the gpu.
	// compute all the needed triangles
	CalculateIndices();
}
Chunk::Chunk(glm::vec3 position)
	: m_Position(position),
	m_VertexArray(),
	m_VertexBuffer(),
	m_IndexBuffer()
{
	srand((unsigned)time(0));
	for (auto& x : m_ChunkData) {
		for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
			for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
				x[y][z] = rand() % 2;
			}
		}
	}

	// we should load the chunk data to the gpu.
	// compute all the needed triangles
	CalculateIndices();
}

static constexpr GLfloat right_face[] = {
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.33f, 0.0f,
	1.0f, 1.0f, 0.0f, 0.33f, 1.0f,
};
static constexpr GLuint right_indices[] = {
	0, 1, 2,
	0, 2, 3,
};
static constexpr GLfloat up_face[] = {
	// Up face 4-8
	1.0f, 1.0f, 1.0f, 0.335f, 1.0f,
	1.0f, 1.0f, 0.0f, 0.67f, 1.0f,
	0.0f, 1.0f, 0.0f, 0.67f, 0.0f,
	0.0f, 1.0f, 1.0f, 0.335f, 0.0f,
};
static constexpr GLuint up_indices[] = {
	0, 1, 2,
	0, 2, 3,
};
static constexpr GLfloat down_face[] = {
	0.0f, 0.0f, 0.0f, 0.67f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.67f, 0.0f,
	0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
};
static constexpr GLuint down_indices[] = {
	0, 2, 1,
	0, 1, 3,
};
static constexpr GLfloat back_face[] = {
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 0.33f, 1.0f,
	1.0f, 0.0f, 0.0f, 0.33f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
};
static constexpr GLuint back_indices[] = {
	0, 1, 2,
	0, 3, 1,
};
static constexpr GLfloat front_face[] = {
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 0.33f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 0.33f, 1.0f,
};
static constexpr GLuint front_indices[] = {
	0, 1, 2,
	0, 3, 1,
};
static constexpr GLfloat left_face[] = {
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 0.33f, 1.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 0.33f, 0.0f,
};
static constexpr GLuint left_indices[] = {
	0, 1, 2,
	0, 3, 1,
};
static constexpr GLfloat vertices[] = {
	// Right face 0-4
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f,1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.33f, 0.0f,1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 0.33f, 1.0f,1.0f, 0.0f, 0.0f,

	// Up face 4-8
	1.0f, 1.0f, 1.0f, 0.335f, 1.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 0.67f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.67f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 0.335f, 0.0f, 0.0f, 1.0f, 0.0f,

	// Forward face 8-12
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 0.33f, 0.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 0.33f, 1.0f, 0.0f, 0.0f, 1.0f,

	// Left face 12-16
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 0.33f, 1.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.33f, 0.0f, -1.0f, 0.0f, 0.0f,

	// Down face 16-20
	0.0f, 0.0f, 0.0f, 0.67f, 1.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.67f, 0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,


	// Backward 20-24
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	1.0f, 1.0f, 0.0f, 0.33f, 1.0f, 0.0f, 0.0f, -1.0f,
	1.0f, 0.0f, 0.0f, 0.33f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
};

#define VERTEX_SIZE 8

static constexpr GLuint indices[] = {
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

void AppendVertices(std::vector<float> &face, int offset, glm::vec3 xyz) {
		for (int i = 0; i < 4; ++i) {
			face.push_back(vertices[offset + VERTEX_SIZE * i] + (float)xyz.x);
			face.push_back(vertices[offset + VERTEX_SIZE * i + 1] + (float)xyz.y);
			face.push_back(vertices[offset + VERTEX_SIZE * i + 2] + (float)xyz.z);
			for (int j = 3; j < 8; j++) {
				face.push_back(vertices[offset + VERTEX_SIZE * i + j]);
			}
		}
}
void AppendIndices(std::vector<unsigned int>& indices, int offset, const GLuint* indxs) {
	for (int i = 0; i < 6; i++)
	{
		indices.push_back(indxs[i] + offset);
	}
}

void Chunk::CalculateIndices() {
    std::vector<GLfloat> v_vertices;
    std::vector<GLuint> v_indices;
	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
			for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
				if (m_ChunkData[x][y][z] == 0) continue;
				glm::mat4 Model = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
				if (x == 0 || m_ChunkData[x - 1][y][z] == 0) {
					// Add left side.
					AppendIndices(v_indices, v_vertices.size() / VERTEX_SIZE, &left_indices[0]);
					v_vertices.reserve(v_vertices.size() + 20);
					AppendVertices(v_vertices, 12 * VERTEX_SIZE, glm::vec3((float)x, (float)y, (float)z));
				}
				if (x == CHUNK_SIZE - 1 || m_ChunkData[x + 1][y][z] == 0) {
					// Add right side.
					AppendIndices(v_indices, v_vertices.size() / VERTEX_SIZE, &right_indices[0]);
					v_vertices.reserve(v_vertices.size() + 20);
					AppendVertices(v_vertices, 0, glm::vec3((float)x, (float)y, (float)z));
				}
				if (y == 0 || m_ChunkData[x][y - 1][z] == 0) {
					// Add down side.
					AppendIndices(v_indices, v_vertices.size() / VERTEX_SIZE, &down_indices[0]);
					v_vertices.reserve(v_vertices.size() + 20);
					AppendVertices(v_vertices, 16 * VERTEX_SIZE, glm::vec3((float)x, (float)y, (float)z));
				}
				if (y == CHUNK_HEIGHT - 1 || m_ChunkData[x][y + 1][z] == 0) {
					// Add up side.
					AppendIndices(v_indices, v_vertices.size() / VERTEX_SIZE, &up_indices[0]);
					v_vertices.reserve(v_vertices.size() + 20);
					AppendVertices(v_vertices, 4 * VERTEX_SIZE, glm::vec3((float)x, (float)y, (float)z));
				}
				if (z == 0 || m_ChunkData[x][y][z - 1] == 0) {
					// Add backward side.
					AppendIndices(v_indices, v_vertices.size() / VERTEX_SIZE, &back_indices[0]);
					v_vertices.reserve(v_vertices.size() + 20);
					AppendVertices(v_vertices, 20 * VERTEX_SIZE, glm::vec3((float)x, (float)y, (float)z));
				}
				if (z == CHUNK_SIZE - 1 || m_ChunkData[x][y][z + 1] == 0) {
					// Add forward side.
					AppendIndices(v_indices, v_vertices.size() / VERTEX_SIZE, &front_indices[0]);
					v_vertices.reserve(v_vertices.size() + 20);
					AppendVertices(v_vertices, 8 * VERTEX_SIZE, glm::vec3((float)x, (float)y, (float)z));
				}
			}
		}
	}
	VertexBufferLayout vl;
	m_VertexBuffer->SetData(&v_vertices.front(), sizeof(float) * v_vertices.size(), GL_DYNAMIC_DRAW);
	m_IndexBuffer->SetData(&v_indices.front(), v_indices.size(), GL_DYNAMIC_DRAW);
	vl.Push<float>(3, 0);
	vl.Push<float>(2, 2);
	vl.Push<float>(3, 3);
	m_VertexArray->AddBuffer(*m_VertexBuffer, vl);
}

void Chunk::LoadPosition(glm::vec3 position)
{
	m_Position = position;
}

glm::vec3 Chunk::GetPositionChunkSpace()
{
	return Chunk::m_Position;
}

glm::vec3 Chunk::GetPositionWorldSpace()
{
	return glm::vec3(Chunk::m_Position.x * CHUNK_SIZE, 
		Chunk::m_Position.y * CHUNK_HEIGHT, 
		Chunk::m_Position.z * CHUNK_SIZE);
}

void Chunk::Bind()
{
	m_VertexArray->Bind();
	m_VertexBuffer->Bind();
	m_IndexBuffer->Bind();
}

unsigned int Chunk::GetIndicesCount()
{
	return m_IndexBuffer->GetCount();
}
;

Chunk::~Chunk() {
}

