#include <glm/glm.hpp>

#include "ChunkManager.h"
#include <glm/ext/matrix_transform.hpp>

ChunkManager::ChunkManager(Renderer *renderer, GameConfiguration *gameConf, glm::vec3 currentChunk):
	m_Renderer(renderer),
	m_GameConfiguration(gameConf),
	m_ChunkCount(CalculateChunkCount()),
	m_Chunks(new Chunk* [m_ChunkCount] {nullptr}),
	//m_ChunkThread(&ChunkManager::RunChunkLoader, this),
	m_ShouldUpdateChunks(false),
	m_Running(true),
	m_LastChunkPosition(currentChunk),
	m_CurrentChunk(currentChunk),
	m_ChunkCV()
{
	const auto starting_chunk = m_LastChunkPosition - glm::vec3(m_GameConfiguration->chunkRenderDistance, m_GameConfiguration->chunkRenderHeight, m_GameConfiguration->chunkRenderDistance);
	const auto chunk_height = 1 + 2 * m_GameConfiguration->chunkRenderHeight;
	// first setup of chunks, initialize m_LastChunkPosition
	// we want to create chunks from the closest to the player and away.
	const auto
		chunk_width = 1 + 2 * m_GameConfiguration->chunkRenderDistance;
	auto* const vao_array = new GLuint[m_ChunkCount];
	auto* const vb_array = new GLuint[m_ChunkCount];
	auto* const ib_array = new GLuint[m_ChunkCount];
	glGenBuffers(m_ChunkCount, vb_array);
	glGenBuffers(m_ChunkCount, ib_array);
	glGenVertexArrays(m_ChunkCount, vao_array);
	for (auto z = 0; z < chunk_width; z++)
	{
		for (auto y = 0; y < chunk_height; y++)
		{
			for (auto x = 0; x < chunk_width; x++)
			{
				const int i = x + y * chunk_width + z * chunk_width * chunk_height;
				m_Chunks[i] = 
					new Chunk(glm::vec3(x + starting_chunk.x, y + starting_chunk.y, z + starting_chunk.z), 
						new VertexArray(vao_array[i]),
						new VertexBuffer(vb_array[i]), 
						new IndexBuffer(ib_array[i]));
			}
		}
	}
	std::cout << "finished loading " << m_ChunkCount <<" chunks!";
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
	for (size_t i = 0; i < m_ChunkCount; i++)
	{
		auto chunk = m_Chunks[i];
		if (chunk != nullptr && 
			chunk->GetVisibleBlocksCount() != 0 && 
			m_Renderer->GetFrustrum().CheckRect(chunk->GetCenterWorldSpace(), CHUNK_SIZE, CHUNK_HEIGHT)) 
		{
			auto model = translate(glm::mat4(1.0), chunk->GetPositionWorldSpace());
			auto mvp = m_Renderer->m_ViewProjection * model;
			blockShader->SetUniformMatrix4fv("MVP", 1, GL_FALSE, &mvp[0][0]);
			blockShader->SetUniformMatrix4fv("M", 1, GL_FALSE, &model[0][0]);
			chunk->Draw(m_Renderer);
		}
	}
}

void ChunkManager::Update()
{
}

void ChunkManager::SetCurrenChunk(glm::vec3 currentChunk)
{
	if (currentChunk != m_CurrentChunk)
	{
		std::unique_lock<std::mutex> lk(m_ChunksLock);
		m_CurrentChunk = currentChunk;
		m_ShouldUpdateChunks = true;
		m_ChunkCV.notify_all();
		std::cout << "Player has moved chunk! x:" << currentChunk.x << " y:" << currentChunk.y << " z:" << currentChunk.z <<"\n";
	}
}

uint32_t ChunkManager::CalculateChunkCount()
{
	const auto range = 1 + 2 * m_GameConfiguration->chunkRenderDistance;
	const auto height = 1 + 2 * m_GameConfiguration->chunkRenderHeight;
	return range * range * height;
}

void ChunkManager::RunChunkLoader() {
	while (m_Running) {
		//aquire the lock, check if we should update chunks, and if not, wait untill we do.
		glm::vec3 currentChunk;
		{
			std::unique_lock<std::mutex> lk(m_ChunksLock);
			if (!m_ShouldUpdateChunks) {
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
			if (chunk_pos.x < currentChunk.x - chunk_dist ||
				chunk_pos.x > currentChunk.x + chunk_dist ||
				chunk_pos.z < currentChunk.z - chunk_dist ||
				chunk_pos.z > currentChunk.z + chunk_dist ||
				chunk_pos.y < currentChunk.y - chunk_height ||
				chunk_pos.y > currentChunk.y + chunk_height) {
				// chunk is out side of render area.
				// if the chunk at position 'p' relativly to the player last position is now out of render area,
				// the chunk at position '-p' relativly to the player current  position must be in the render area.
				auto reversed = currentChunk - (chunk_pos - m_LastChunkPosition);
				reversed.y = chunk_pos.y;
				chunk->LoadPosition(reversed);
			}
		}
		m_LastChunkPosition = currentChunk;
		std::cout << "finished chunk calculation!\n";
		// finished recalculating chunks!
	}
	// save and close
}

