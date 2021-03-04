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
	m_Noise(FastNoise::NewFromEncodedNodeTree( "DQADAAAAAAAAQAkAAAAAAD8AAAAAAA==")),
	m_ChunkCV()
{
	auto* const vao_array = new GLuint[m_ChunkCount];
	auto* const vb_array = new GLuint[m_ChunkCount];
	auto* const ib_array = new GLuint[m_ChunkCount];
	glGenBuffers(m_ChunkCount, vb_array);
	glGenBuffers(m_ChunkCount, ib_array);
	glGenVertexArrays(m_ChunkCount, vao_array);
	unsigned int h = timer::start();
	unsigned handle = timer::start();
	for(int i = 0; i < (int)m_ChunkCount; i++)
	{
		auto chunkPos = CalculateChunkPositionByIndex(i, m_CurrentChunk);
		m_Chunks[i] = new Chunk(
			this,
			m_Noise,
			chunkPos,
			new VertexArray(vao_array[i]));
	}
	auto l = timer::total(handle);
	std::cout << "time to create chunks: " << l.count() / 1000000 << std::endl;
	m_Chunks[0]->LoadData();
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
	int chunksLoaded = 0;
	{
		std::unique_lock<std::mutex> lk(m_BackBufferLock);
		for (size_t i = 0; i < m_ChunkCount; i++)
		{
			auto* chunk = m_Chunks[i];
			if(chunk == nullptr)
			{
				std::cout << "ERROR: Chunk is null!" << std::endl;
			}
			else if (chunk->m_CurrentState != unloaded && chunk->m_CurrentState != loaded)
			{
				if(chunk->m_CurrentState == meshed)
				{
					if (chunksLoaded > 20) continue;
					chunk->LoadMesh();
				}
				if(chunk->m_CurrentState == dirty_meshed)
				{
					if (chunksLoaded < 20)
						chunk->LoadMesh();
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
	m_MeshedChunksCount = chunksLoaded;
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
		//std::cout << "Player has moved chunk! x:" << currentChunk.x << " z:" << currentChunk.y << "\n";
	}
}

Chunk* ChunkManager::GetChunkByPosition(glm::ivec2 position)
{
	auto index = CalculateChunkIndex(position, m_Chunks[0]->m_Position);
	return index < m_ChunkCount ? m_Chunks[index] : nullptr;
}

uint32_t ChunkManager::CalculateChunkCount()
{
	const auto range = 1 + 2 * m_GameConfiguration->chunkRenderDistance;
	const auto height = 1 + 2 * m_GameConfiguration->chunkRenderHeight;
	return range * range * height;
}

void ChunkManager::RunChunkLoader()
{
	// (2(n+1)+1)^2 - (2n+1)^2 = (2n+3)^2 - (2n+1)^2 = 12n - 5n +9 - 1 = 7n+8
	unsigned handle = timer::start();
	int distance = 1;
	int a0 = 0;
	int a1 = 1;
	int a2 = 9;
	for(int i = 0; i < (int)m_ChunkCount; i++)
	{
		m_Chunks[i]->LoadData();
		if(i == a2)
		{
			for (int j = a0; j < a1; j++)
			{
				m_Chunks[j]->CalculateMesh();
			}
			distance++;
			a0 = a1;
			a1 = a2;
			a2 += 7 * distance + 8;
		}
	}
	auto t = timer::total(handle);
	std::cout << "time to load and calculate initial chunks: " << t.count() / 1000000 << std::endl;
	for(int i = a0; i < a1; i++)
	{
		m_Chunks[i]->CalculateMesh();
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
		std::cout << "starting chunks recalculation! " << std::endl;
		unsigned handle = timer::start();
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
			else if(chunk->m_CurrentState == active)
			{
				// if was in the edge
				auto r_pos = chunk_pos - m_LastChunkPosition;
				auto new_r_pos = chunk_pos - currentChunk;
				if((abs(r_pos.x) == (int)chunk_dist  && new_r_pos.x != r_pos.x) || 
					(abs(r_pos.y) == (int)chunk_dist) && new_r_pos.y != r_pos.y)
				{
					chunk->m_CurrentState = dirty;
				}
			}

			m_BackBufferChunks[index] = chunk;
		}
		auto t = timer::lap(handle);
		std::cout << "time to setting position: " << t.count() / 1000000 << std::endl;
		{
			std::unique_lock<std::mutex> lk(m_BackBufferLock);
			auto* chunks = m_Chunks;
			m_Chunks = m_BackBufferChunks;
			m_BackBufferChunks = chunks;
			m_LastChunkPosition = currentChunk;
		}
		t = timer::lap(handle);
		std::cout << "time to lock position: " << t.count() / 1000000 << std::endl;
		int loadedChunks = 0;
		for (size_t i = 0; i < m_ChunkCount; i++)
		{
			auto* const chunk = m_Chunks[i];
			if (chunk->m_CurrentState == unloaded)
			{
				loadedChunks++;
				chunk->LoadData();
			}
		}
		t = timer::lap(handle);
		std::cout << "time to loading " << loadedChunks << " data position: " << t.count() / 1000000 << std::endl;
		for (size_t i = 0; i < m_ChunkCount; i++)
		{
			auto* const chunk = m_Chunks[i];
			if (chunk->m_CurrentState == loaded || chunk->m_CurrentState == dirty)
			{
				chunk->CalculateMesh();
			}
		}
		t = timer::lap(handle);
		std::cout << "time to calculating meshes: " << t.count() / 1000000 << std::endl;
		t = timer::total(handle);
		std::cout << "finished chunk calculation in: " << t.count() / 1000000 << std::endl;
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

