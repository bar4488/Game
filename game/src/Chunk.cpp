//
// Created by bar44 on 11/3/2020.
//

#include "Chunk.h"

#include <glm/glm.hpp>
#include <vector>

#include <ctime> 

#include <glm/gtc/matrix_transform.hpp>

#include "ChunkManager.h"
#include "graphics/VertexArray.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"
#include "constants.h"

Chunk::Chunk(ChunkManager* mgr, FastNoise::SmartNode<> noise, glm::ivec2 position, VertexArray *vao):
	m_Manager(mgr),
	m_Noise(noise),
	m_VertexArray(*vao),
	m_CurrentState(unloaded),
	m_HeighestBlock(0),
	m_LevelOfDetails(1)
{
	SetPosition(position);
}

void Chunk::SetPosition(glm::ivec2 position)
{
	m_CurrentState = unloaded;
	m_Position = position;
}

void Chunk::SetLod(unsigned char lod)
{
	if (m_LevelOfDetails != lod)
	{
		if(m_CurrentState == active || m_CurrentState == dirty || m_CurrentState == dirty_meshed)
		{
			m_CurrentState = dirty;
		}
		else
		{
			m_CurrentState = loaded;
		}
		m_LevelOfDetails = lod;
	}
	else
	{
		std::cout << "error set lod!\n";
	}
}

void Chunk::LoadData()
{
	const int top = 128;
	const int bottom = 60;
	const int height = top - bottom;

	// Generate a 16 x 16 x 16 area of noise
	float noiseOutput[height * CHUNK_SIZE * CHUNK_SIZE];
	m_Noise->GenUniformGrid3D(noiseOutput, m_Position.x * CHUNK_SIZE, 0, m_Position.y * CHUNK_SIZE, CHUNK_SIZE, height, CHUNK_SIZE, 0.005f, 1237);
	int index = 0;
	m_HeighestBlock = bottom;

	for (int z = 0; z < CHUNK_SIZE; z += 1)
	{
		for (int y = 0; y < top; y += 1)
		{
			for (int x = 0; x < CHUNK_SIZE; x += 1)
			{
				if(y < bottom)
				{
					m_ChunkData[x + y*CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] = 1;
				}
				else
				{
					int blockType = noiseOutput[index++] + (height - 2.0f*(y - bottom)) / height > 0.0f ? 1 : 0;
					m_ChunkData[x + y*CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] = blockType;
					if(blockType != 0 && y > m_HeighestBlock)
						m_HeighestBlock = y;
				}
			}			
		}
	}
	m_CurrentState = loaded;
}

void Chunk::CalculateMesh()
{
	m_VisibleFaces.clear();
	auto* left_c = m_Manager->GetChunkByPosition(m_Position + glm::ivec2(-1,0));
	left_c = left_c != nullptr && left_c->m_CurrentState != unloaded ? left_c : nullptr;
	auto* right_c = m_Manager->GetChunkByPosition(m_Position + glm::ivec2(1,0));
	right_c = right_c != nullptr && right_c->m_CurrentState != unloaded ? right_c : nullptr;
	auto* forward_c = m_Manager->GetChunkByPosition(m_Position + glm::ivec2(0,1));
	forward_c = forward_c != nullptr && forward_c->m_CurrentState != unloaded ? forward_c : nullptr;
	auto* backward_c = m_Manager->GetChunkByPosition(m_Position + glm::ivec2(0,-1));
	backward_c = backward_c != nullptr && backward_c->m_CurrentState != unloaded ? backward_c : nullptr;

	int offset = pow(2, m_LevelOfDetails - 1);
	for (unsigned int x = 0; x < CHUNK_SIZE; x += pow(2, m_LevelOfDetails - 1)) {
		for (unsigned int y = 0; y < m_HeighestBlock + 1; y+= pow(2, m_LevelOfDetails - 1)) {
			for (unsigned int z = 0; z < CHUNK_SIZE; z+=pow(2, m_LevelOfDetails - 1)) {
				if (m_ChunkData[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) continue;
				if ((x!=0 && m_ChunkData[x - offset + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) ||
					(x == 0 && left_c != nullptr && left_c->m_ChunkData[CHUNK_SIZE - offset + y*CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0)) 
				{
					// Add left side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						left,
						1u
						});
				}
				if ((x != CHUNK_SIZE - offset && m_ChunkData[x + offset + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) ||
					(x == CHUNK_SIZE - offset && right_c != nullptr && right_c->m_ChunkData[0 + y*CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0)) 
				{
					// Add right side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						right,
						1u
						});
				}
				if (y != 0 && m_ChunkData[x + (y - offset) * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
					// Add down side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						down,
						1u
						});
				}
				if (y == CHUNK_HEIGHT - offset || m_ChunkData[x + (y+offset) * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] == 0) {
					// Add up side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						up,
						1u
						});
				}
				if ((z != 0 && m_ChunkData[x + y * CHUNK_SIZE + (z-offset) * CHUNK_SIZE * CHUNK_HEIGHT] == 0) ||
					(z == 0 && backward_c != nullptr && backward_c->m_ChunkData[x + y*CHUNK_SIZE + (CHUNK_SIZE - offset) * CHUNK_SIZE * CHUNK_HEIGHT] == 0)) 
				{
					// Add backward side.
					m_VisibleFaces.push_back({
						glm::tvec3<unsigned char>(x,y,z),
						backward,
						1u
						});
				}
				if ((z != CHUNK_SIZE - offset && m_ChunkData[x + y * CHUNK_SIZE + (z+offset) * CHUNK_SIZE * CHUNK_HEIGHT] == 0) ||
					(z == CHUNK_SIZE - offset && forward_c != nullptr && forward_c->m_ChunkData[x + y*CHUNK_SIZE] == 0)) 
				{
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
	m_FacesCount = m_VisibleFaces.size();

	m_CurrentState = m_VisibleFaces.empty() ? active : (m_CurrentState == dirty ? dirty_meshed : meshed);
}

void Chunk::LoadMesh()
{
	if(m_CurrentState == meshed || m_CurrentState == dirty_meshed)
	{
		m_TextureBuffer.SetData(&m_VisibleFaces.front(), m_VisibleFaces.size() * sizeof(block_face), GL_STATIC_DRAW);
		m_CurrentState = active;
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
	return m_FacesCount;
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
	glDrawArrays(GL_TRIANGLES, 0, m_FacesCount * 6);
}
;

Chunk::~Chunk() {
}

