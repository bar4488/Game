#include <glm/glm.hpp>

#include "ChunkManager.h"
#include <glm/ext/matrix_transform.hpp>

#include "utils/timer.h"

ChunkManager::ChunkManager(Renderer* renderer, GameConfiguration* gameConf, glm::ivec2 currentChunk):
	m_Renderer(renderer),
	m_GameConfiguration(gameConf),
	m_ChunkCount(CalculateChunkCount()),
	m_Chunks(new Chunk*[m_ChunkCount]{nullptr}),
	m_BackBufferChunks(new Chunk*[m_ChunkCount]{nullptr}),
	m_ShouldUpdateChunks(false),
	m_Running(true),
	m_LastChunkPosition(currentChunk),
	m_CurrentChunk(currentChunk),
	m_Noise(7),
	m_ChunkCV()
{
	auto* const vao_array = new GLuint[m_ChunkCount];
	auto* const vb_array = new GLuint[m_ChunkCount];
	auto* const ib_array = new GLuint[m_ChunkCount];
	glGenBuffers(m_ChunkCount, vb_array);
	glGenBuffers(m_ChunkCount, ib_array);
	glGenVertexArrays(m_ChunkCount, vao_array);
	unsigned int h = timer::start();
	for(int i = 0; i < (int)m_ChunkCount; i++)
	{
		auto chunkPos = CalculateChunkPositionByIndex(i, m_CurrentChunk);
		m_Chunks[i] = new Chunk(
			m_Noise,
			chunkPos,
			new VertexArray(vao_array[i]));
	}
	m_Chunks[0]->LoadMesh();
	m_ChunkThread = std::thread(&ChunkManager::RunChunkLoader, this);
	auto t = timer::total(h);
	std::cout << "finished loading " << m_ChunkCount << " chunks!\n";
	std::cout << "avg. for chunk: " << t.count() / (1000000.0 * m_ChunkCount) << "ms" <<  std::endl;
}

ChunkManager::~ChunkManager()
{
	m_Running = false;
	m_ChunkThread.join();
	delete m_Chunks;
}

void ChunkManager::Draw()
{
	Program* blockShader = m_Renderer->GetProgramByName("block");
	// chunk manager specific:
	blockShader->Bind();
	m_Renderer->BindTexture("dirt", 0);
	blockShader->SetUniform1i("tex", 0);
	blockShader->SetUniform3f("lightDir", 0.2f, 1.0f, 0.7f);
	blockShader->SetUniformVec3("viewPos", m_Renderer->m_CameraPosition);
	blockShader->SetUniform3f("lightColor", 0.8f, 0.8f, 0.0f);
	blockShader->SetUniform1i("faces", 2);
	unsigned int renderedCount = 0;
	{
		std::unique_lock<std::mutex> lk(m_BackBufferLock);
		int chunksLoaded = 0;
		for (size_t i = 0; i < m_ChunkCount; i++)
		{
			auto* chunk = m_Chunks[i];
			if(chunk == nullptr)
			{
				std::cout << "ERROR: Chunk is null!" << std::endl;
			}
			if (chunk != nullptr && 
				chunk->m_CurrentState != dirty)
			{
				if(chunk->m_CurrentState == meshed)
				{
					if (chunksLoaded > 5) continue;
					chunk->LoadData();
				}
				if(chunk->GetVisibleFacesCount() != 0 &&
					m_Renderer->GetFrustum().CheckRect(chunk->GetPositionWorldSpace(), CHUNK_SIZE, chunk->GetHeight() + 1))
				{
					renderedCount++;
					auto model = translate(glm::mat4(1.0), static_cast<glm::vec3>(chunk->GetPositionWorldSpace()));
					auto mvp = m_Renderer->m_ViewProjection * model;
					blockShader->SetUniformMatrix4fv("MVP", 1, GL_FALSE, &mvp[0][0]);
					blockShader->SetUniformMatrix4fv("M", 1, GL_FALSE, &model[0][0]);
					chunk->Draw(m_Renderer);
				}
			}
		}
	}
	m_RenderedChunksCount = renderedCount;
}

void ChunkManager::Update()
{
}

void ChunkManager::SetCurrenChunk(glm::ivec2 currentChunk)
{
	if (currentChunk != m_CurrentChunk)
	{
		std::unique_lock<std::mutex> lk(m_ChunksLock);
		m_CurrentChunk = currentChunk;
		m_ShouldUpdateChunks = true;
		m_ChunkCV.notify_all();
		std::cout << "Player has moved chunk! x:" << currentChunk.x << " z:" << currentChunk.y << "\n";
	}
}

Chunk* ChunkManager::GetChunkByPosition(glm::ivec2 position)
{
	return m_Chunks[CalculateChunkIndex(position, m_CurrentChunk)];
}

uint32_t ChunkManager::CalculateChunkCount()
{
	const auto range = 1 + 2 * m_GameConfiguration->chunkRenderDistance;
	const auto height = 1 + 2 * m_GameConfiguration->chunkRenderHeight;
	return range * range * height;
}

void ChunkManager::RunChunkLoader()
{
	for(int i = 0; i < (int)m_ChunkCount; i++)
	{
		m_Chunks[i]->LoadMesh();
	}
	while (m_Running)
	{
		//aquire the lock, check if we should update chunks, and if not, wait until we do.
		glm::ivec2 currentChunk;
		{
			std::unique_lock<std::mutex> lk(m_ChunksLock);
			if (!m_ShouldUpdateChunks)
			{
				m_ChunkCV.wait(lk);
			}
			currentChunk = m_CurrentChunk;
			m_ShouldUpdateChunks = false;
		}
		const auto chunk_dist = m_GameConfiguration->chunkRenderDistance,
		           chunk_height = m_GameConfiguration->chunkRenderHeight;
		for (size_t i = 0; i < m_ChunkCount; i++)
		{
			auto* const chunk = m_Chunks[i];
			auto chunk_pos = chunk->GetPositionChunkSpace();
			int index = CalculateChunkIndex(chunk_pos, currentChunk);
			if (index >= (int)m_ChunkCount)
			{
				// chunk is out side of render area.
				// if the chunk at position 'p' relativly to the player last position is now out of render area,
				// the chunk at position '-p' relativly to the player current  position must be in the render area.
				glm::ivec2 reversed = currentChunk - (chunk_pos - m_LastChunkPosition);
				chunk->SetPosition(reversed);
				index = CalculateChunkIndex(reversed, currentChunk);
				if (index >= m_ChunkCount) std::cout << "ERROR: ChunkManager calculating chunk reversed!" << std::endl;
			}

			m_BackBufferChunks[index] = chunk;
		}
		{
			std::unique_lock<std::mutex> lk(m_BackBufferLock);
			auto* chunks = m_Chunks;
			m_Chunks = m_BackBufferChunks;
			m_BackBufferChunks = chunks;
			m_LastChunkPosition = currentChunk;
		}
		for (size_t i = 0; i < m_ChunkCount; i++)
		{
			auto* const chunk = m_Chunks[i];
			if (chunk->m_CurrentState == dirty)
			{
				chunk->LoadMesh();
			}
		}
		std::cout << "finished chunk calculation!\n";
		// finished recalculating chunks!
	}
	// save and close
}

int ChunkManager::CalculateChunkIndex(glm::ivec2 chunkPosition, glm::ivec2 centerPosition)
{
	/*
	 *  we are calculating the index of the chunk with position 'chunkPosition'
	 *  in an array where the center is 'centerPosition'
	 *
	 *  the array is sorted like so:
	 *  | 12-11-10-9  24
	 *  | 13 2--1  8  23
	 *  | 14 3  0  7  22
	 *  | 15 4--5--6  21
	 *  | 16-17-18-19-20
	 *
	 *  and we would like to find the index of a specific chunk.
	 */
	
	const int rx = chunkPosition.x - centerPosition.x;
	const int ry = chunkPosition.y - centerPosition.y;
	const int dx = abs(rx);
	const int dy = abs(ry);
	const int distance = dx > dy ? dx : dy; // max(dx, dy);
	int index;
	if(rx == -distance || (rx < distance && ry == distance)) {
		const int startingDistIndex = (2 * distance - 1) * (2 * distance - 1);
		const int sx = distance - 1 - rx;
		const int sy = distance - ry;
		index = startingDistIndex + sx + sy;
	}
	else
	{
		const int startingDistIndex = 4 * distance * distance; // basically (2*distance)^2
		index = startingDistIndex + (distance + ry) + (distance + rx);
	}
	return index;
}


glm::ivec2 ChunkManager::CalculateChunkPositionByIndex(int index, glm::ivec2 centerPosition)
{
	// does the opposite of CalculateChunkIndex
	const int sq = static_cast<int>(glm::sqrt(index));
	int distance;
	if (sq % 2 == 1) distance = (sq + 1) / 2;
	else distance = sq / 2;

	const int starting_index = sq * sq;
	glm::ivec2 pos;
	if(sq % 2 == 1)
	{
		const glm::ivec2 starting_pos = glm::ivec2(distance - 1, distance);
		const int remaining = index - starting_index;
		const int dx = remaining >= 2 * distance ? 2 * distance - 1 : remaining;
		const int dy = remaining - (2*distance - 1);
		pos = glm::ivec2(starting_pos.x - dx, starting_pos.y - (dy > 0 ? dy : 0));
	}
	else
	{
		const glm::ivec2 starting_pos = glm::ivec2(-distance, -distance);
		const int remaining = index - starting_index;
		const int dx = remaining > 2 * distance ? 2 * distance : remaining;
		const int dy = remaining - 2*distance;
		pos = glm::ivec2(starting_pos.x + dx, starting_pos.y + (dy > 0 ? dy : 0));
	}
	return centerPosition + pos;
}

