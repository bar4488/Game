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

Chunk::Chunk(siv::PerlinNoise noise, glm::vec3 position, VertexArray *vao):
	m_VertexArray(*vao),
	m_TextureBuffer(GL_R8UI),
	m_Noise(noise),
	m_Dirty(false),
	m_Loaded(false),
	m_Meshed(false)
{
	LoadPosition(position);
}
Chunk::Chunk(siv::PerlinNoise noise, glm::vec3 position):
	m_VertexArray(),
	m_TextureBuffer(GL_R8UI),
	m_Noise(noise),
	m_Dirty(false),
	m_Loaded(false),
	m_Meshed(false)
{
	LoadPosition(position);
}

void Chunk::LoadPosition(glm::vec3 position)
{
	m_Position = position;
	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
			double h = m_Noise.noise3D_0_1((position.x * 16 + x) / 100.0, (position.z * 16 + z) / 100.0, 0.14342);
			unsigned int height = static_cast<unsigned>(h * CHUNK_HEIGHT);
			for (unsigned int y = 0; y < height; y++) {
				m_ChunkData[x + y*CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] = 1;
			}
		}
	}

	CalculateMesh();
}

void Chunk::CalculateMesh()
{
	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
			for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
				if (m_ChunkData[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) continue;
				if(y > m_HeighestBlock)
					m_HeighestBlock = y;
				if (x == 0 || m_ChunkData[x - 1 + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
					// Add left side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						left,
						1u
						});
				}
				if (x == CHUNK_SIZE - 1 || m_ChunkData[x + 1 + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
					// Add right side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						right,
						1u
						});
				}
				if (y != 0 && m_ChunkData[x + (y - 1) * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
					// Add down side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						down,
						1u
						});
				}
				if (y == CHUNK_HEIGHT - 1 || m_ChunkData[x + (y+1) * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
					// Add up side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						up,
						1u
						});
				}
				if (z == 0 || m_ChunkData[x + y * CHUNK_SIZE + (z-1) * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
					// Add backward side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						backward,
						1u
						});
				}
				if (z == CHUNK_SIZE - 1 || m_ChunkData[x + y * CHUNK_SIZE + (z+1) * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
					// Add forward side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						forward,
						1u
						});
				}
			}
		}
	}

	if(m_VisibleFaces.size() != 0)
		m_TextureBuffer.SetData(&m_VisibleFaces.front(), m_VisibleFaces.size() * sizeof(block_face), GL_STATIC_DRAW);
}

glm::vec3 Chunk::GetPositionChunkSpace()
{
	return Chunk::m_Position;
}

glm::vec3 Chunk::GetCenterWorldSpace() {
	return GetPositionWorldSpace() + glm::vec3(CHUNK_SIZE / 2, CHUNK_HEIGHT / 2, CHUNK_SIZE / 2);
}

unsigned Chunk::GetVisibleFacesCount()
{
	return m_VisibleFaces.size();
}

unsigned char Chunk::GetHeight()
{
	return m_HeighestBlock;
}

glm::vec3 Chunk::GetPositionWorldSpace()
{
	return glm::vec3(Chunk::m_Position.x * CHUNK_SIZE, 
		Chunk::m_Position.y * CHUNK_HEIGHT, 
		Chunk::m_Position.z * CHUNK_SIZE);
}

void Chunk::Bind()
{
	m_VertexArray.Bind();
}

void Chunk::Unbind()
{
	m_VertexArray.Unbind();
}

void Chunk::InitializeChunk()
{
}

void Chunk::Draw(Renderer* renderer)
{
	// TODO: implement draw with faces
	m_TextureBuffer.Bind(2);
	m_VertexArray.Bind();
	glDrawArrays(GL_TRIANGLES, 0, m_VisibleFaces.size() * 6);
}
;

Chunk::~Chunk() {
}

