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
#include "constants.h"

Chunk::Chunk(glm::vec3 position, VertexArray *vao, VertexBuffer *vb, IndexBuffer *ib)
	: m_Position(position),
	m_VertexArray(vao),
	m_VertexBuffer(vb),
	m_IndexBuffer(ib),
	m_VisibleBlocks()
{
	srand((unsigned)time(0));
	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
			for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
				m_ChunkData[x + y*CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] = rand() % 5 == 1 ? 1 : 0;
			}
		}
	}

	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
			for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
				if (m_ChunkData[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) continue;
				if (x == 0 || y == 0 || z == 0 || x == CHUNK_SIZE - 1 || y == CHUNK_HEIGHT - 1 || z == CHUNK_SIZE - 1 ||
					m_ChunkData[x + 1 + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0 ||
					m_ChunkData[x - 1 + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0 ||
					m_ChunkData[x + (y + 1) * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0 ||
					m_ChunkData[x + (y - 1) * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0 ||
					m_ChunkData[x + y * CHUNK_SIZE + (z + 1) * CHUNK_SIZE * CHUNK_HEIGHT] == 0 ||
					m_ChunkData[x + y * CHUNK_SIZE + (z - 1) * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
					m_VisibleBlocks.push_back(x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT);
				}

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
	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
			for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
				m_ChunkData[x + y*CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] = rand() % 5 == 1 ? 1 : 0;
			}
		}
	}

	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
			for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
				if (m_ChunkData[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) continue;
				if (x == 0 || y == 0 || z == 0 || x == CHUNK_SIZE - 1 || y == CHUNK_HEIGHT - 1 || z == CHUNK_SIZE - 1 ||
					m_ChunkData[x + 1 + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0 ||
					m_ChunkData[x - 1 + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0 ||
					m_ChunkData[x + (y + 1) * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0 ||
					m_ChunkData[x + (y - 1) * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0 ||
					m_ChunkData[x + y * CHUNK_SIZE + (z + 1) * CHUNK_SIZE * CHUNK_HEIGHT] == 0 ||
					m_ChunkData[x + y * CHUNK_SIZE + (z - 1) * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
					m_VisibleBlocks.push_back(x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT);
				}

			}
		}
	}

	// we should load the chunk data to the gpu.
	// compute all the needed triangles
	CalculateIndices();
}

void AppendVertices(std::vector<CubeVertex> &face, int index, glm::tvec3<unsigned char> xyz) {
		for (int i = 0; i < 4; ++i) {
			face.push_back({
					cube_vertices[index + i].position + xyz,
					cube_vertices[index + i].uv,
					cube_vertices[index + i].normal
				});
		}
}
void AppendIndices(std::vector<unsigned int>& indices, int offset, const GLuint* indxs) {
	for (int i = 0; i < 6; i++)
	{
		indices.push_back(indxs[i] + offset);
	}
}

void Chunk::CalculateIndices() {
	std::cout << sizeof(CubeVertex);
    std::vector<CubeVertex> v_vertices;
    std::vector<GLuint> v_indices;
	for (auto &index : m_VisibleBlocks) {
		auto x = index % CHUNK_SIZE;
		auto y = (index / CHUNK_SIZE) % CHUNK_HEIGHT;
		auto z = (index / (CHUNK_SIZE * CHUNK_HEIGHT)) % CHUNK_SIZE;
		if (x == 0 || m_ChunkData[x - 1 + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
			// Add left side.
			AppendIndices(v_indices, v_vertices.size(), &left_indices[0]);
			AppendVertices(v_vertices, 12, glm::tvec3<unsigned char>(x, y, z));
		}
		if (x == CHUNK_SIZE - 1 || m_ChunkData[x + 1 + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
			// Add right side.
			AppendIndices(v_indices, v_vertices.size(), &right_indices[0]);
			AppendVertices(v_vertices, 0, glm::tvec3<unsigned char>(x, y, z));
		}
		if (y == 0 || m_ChunkData[x + (y - 1) * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
			// Add down side.
			AppendIndices(v_indices, v_vertices.size(), &down_indices[0]);
			AppendVertices(v_vertices, 16, glm::tvec3<unsigned char>(x, y, z));
		}
		if (y == CHUNK_HEIGHT - 1 || m_ChunkData[x + (y+1) * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
			// Add up side.
			AppendIndices(v_indices, v_vertices.size(), &up_indices[0]);
			AppendVertices(v_vertices, 4, glm::tvec3<unsigned char>(x, y, z));
		}
		if (z == 0 || m_ChunkData[x + y * CHUNK_SIZE + (z-1) * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
			// Add backward side.
			AppendIndices(v_indices, v_vertices.size(), &back_indices[0]);
			AppendVertices(v_vertices, 20, glm::tvec3<unsigned char>(x, y, z));
		}
		if (z == CHUNK_SIZE - 1 || m_ChunkData[x + y * CHUNK_SIZE + (z+1) * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
			// Add forward side.
			AppendIndices(v_indices, v_vertices.size(), &front_indices[0]);
			AppendVertices(v_vertices, 8, glm::tvec3<unsigned char>(x, y, z));
		}
	}
	VertexBufferLayout vl;
	m_VertexBuffer->SetData(&v_vertices.front(), sizeof(CubeVertex) * v_vertices.size(), GL_DYNAMIC_DRAW);
	m_IndexBuffer->SetData(&v_indices.front(), v_indices.size(), sizeof(unsigned int), GL_DYNAMIC_DRAW);
	vl.Push<unsigned char>(3, 0);
	vl.Push<unsigned char>(2, 2);
	vl.Push<char>(3, 3);
	m_VertexArray->AddBuffer(*m_VertexBuffer, vl);
}

void Chunk::CalculateVisibleVertices() {

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

void Chunk::Unbind()
{
	m_VertexArray->Unbind();
	m_VertexBuffer->Unbind();
	m_IndexBuffer->Unbind();
}
unsigned int Chunk::GetIndicesCount()
{
	return m_IndexBuffer->GetCount();
}
;

Chunk::~Chunk() {
}

