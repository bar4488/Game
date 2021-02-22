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

Chunk::Chunk(siv::PerlinNoise noise, glm::ivec2 position, VertexArray *vao):
	m_Noise(noise),
	m_VertexArray(*vao),
	m_CurrentState(dirty)
{
	SetPosition(position);
}
Chunk::Chunk(siv::PerlinNoise noise, glm::ivec2 position):
	m_CurrentState(dirty),
	m_Noise(noise)
{
	SetPosition(position);
}

void Chunk::SetPosition(glm::ivec2 position)
{
	m_Position = position;
	m_CurrentState = dirty;
}

void Chunk::LoadMesh()
{
	for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
		for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
			double h = m_Noise.noise3D_0_1((m_Position.x * 16.0 + x) / 30.0, (m_Position.y * 16.0 + z) / 30.0, 0.14342);
			double h2 = m_Noise.noise3D_0_1((m_Position.x * 16.0 + x) / 1000.0, (m_Position.y * 16.0 + z) / 1000.0, 2345.24342);
			for (unsigned int y = 0; y < CHUNK_HEIGHT; y++) {
				double h3 = m_Noise.noise3D((m_Position.x * 16.0 + x) / 300.0, y / 120.0, (m_Position.y * 16.0 + z) / 300.0);
				m_ChunkData[x + y*CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] = 0.5*h2*h +  h3 + (CHUNK_HEIGHT - 2.0*y) / CHUNK_HEIGHT > 0.0 ? 1 : 0;
			}
		}
	}
	m_VisibleFaces.clear();
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

	m_CurrentState = m_VisibleFaces.empty() ? loaded : meshed;
}

void Chunk::LoadData()
{
	if(m_CurrentState == meshed)
	{
		m_TextureBuffer.SetData(&m_VisibleFaces.front(), m_VisibleFaces.size() * sizeof(block_face), GL_STATIC_DRAW);
		m_CurrentState = loaded;
	}
	else
	{
		std::cout << "ERROR: chunk activation error" << std::endl;
	}
}

glm::ivec2 Chunk::GetPositionChunkSpace()
{
	return m_Position;
}

glm::ivec3 Chunk::GetCenterWorldSpace() {
	glm::ivec2 pos = GetPositionWorldSpace();
	return glm::ivec3(pos.x * CHUNK_SIZE, 0, pos.y * CHUNK_SIZE) + glm::ivec3(CHUNK_SIZE / 2, CHUNK_HEIGHT / 2, CHUNK_SIZE / 2);
}

unsigned Chunk::GetVisibleFacesCount()
{
	return m_VisibleFaces.size();
}

unsigned char Chunk::GetHeight()
{
	return m_HeighestBlock;
}

glm::ivec3 Chunk::GetPositionWorldSpace()
{
	return glm::ivec3(m_Position.x * CHUNK_SIZE,
	                  0,
	                  m_Position.y * CHUNK_SIZE);
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
	m_TextureBuffer.Bind(2);
	m_VertexArray.Bind();
	glDrawArrays(GL_TRIANGLES, 0, m_VisibleFaces.size() * 6);
}
;

Chunk::~Chunk() {
}

