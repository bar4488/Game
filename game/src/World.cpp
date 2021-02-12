#include "World.h"

uint32_t CalculateChunkCount(GameConfiguration configuration) {
	uint32_t range = 1 + 2 * configuration.chunkRenderDistance;
	uint32_t height = 1 + 2 * configuration.chunkRenderHeight;
	return range * range * height;
}

World::World(Renderer* renderer, GameConfiguration* gameConf) :
	m_Player(glm::vec3(-4, 0, 0), glm::vec3(1, 0, 0)),
	m_GameConfiguration(gameConf),
	m_Configuration(),
	m_Renderer(renderer),
	m_ChunkCount(CalculateChunkCount(*gameConf)),
	m_Chunks(new Chunk* [m_ChunkCount] {nullptr}),
	//m_ChunkThread(&World::RunChunkLoader, this),
	m_ShouldUpdateChunks(false),
	m_Running(true),
	m_LastChunkPosition(glm::vec3()),
	m_CurrentChunk(m_Player.GetCurrentChunkPosition())
{
	uint32_t chunkHeight = 1 + 2 * m_GameConfiguration->chunkRenderHeight,
		chunkWidth = 1 + 2 * m_GameConfiguration->chunkRenderDistance;
	GLuint* vao_array = new GLuint[m_ChunkCount];
	GLuint* vb_array = new GLuint[m_ChunkCount];
	GLuint* ib_array = new GLuint[m_ChunkCount];
	glGenBuffers(m_ChunkCount, vb_array);
	glGenBuffers(m_ChunkCount, ib_array);
	glGenVertexArrays(m_ChunkCount, vao_array);
	for (int z = 0; z < chunkWidth; z++)
	{
		for (int y = 0; y < chunkHeight; y++)
		{
			for (int x = 0; x < chunkWidth; x++)
			{
				int i = x + y * chunkWidth + z * chunkWidth * chunkHeight;
				m_Chunks[i] = 
					new Chunk(glm::vec3(x, y, z), 
						new VertexArray(vao_array[i]),
						new VertexBuffer(vb_array[i]), 
						new IndexBuffer(ib_array[i]));
			}
		}
	}

}

void World::Update()
{
	// update the player
	m_Player.Update(m_GameConfiguration->window, m_GameConfiguration->width, m_GameConfiguration->height);
	glm::vec3 currPos = m_Player.GetCurrentChunkPosition();
	if (currPos != m_CurrentChunk)
	{
		std::unique_lock<std::mutex> lk(m_ChunksLock);
		m_CurrentChunk = currPos;
		m_ShouldUpdateChunks = true;
		m_ChunkCV.notify_all();
	}
}

void World::Draw()
{
	m_Renderer->BeginDraw(m_Player.GetViewProjection());
	for (size_t i = 0; i < m_ChunkCount; i++)
	{
		if (m_Chunks[i] != nullptr) {
			m_Renderer->DrawChunk(*m_Chunks[i]);
		}
	}
}

void World::RunChunkLoader() {
	glfwMakeContextCurrent(m_GameConfiguration->slaveWindow);
	// first setup of chunks, initialize m_LastChunkPosition
	// we want to create chunks from the closest to the player and away.
	uint32_t chunkHeight = 1 + 2 * m_GameConfiguration->chunkRenderHeight,
		chunkWidth = 1 + 2 * m_GameConfiguration->chunkRenderDistance;
	GLuint* vao_array = new GLuint[m_ChunkCount];
	GLuint* vb_array = new GLuint[m_ChunkCount];
	GLuint* ib_array = new GLuint[m_ChunkCount];
	glGenBuffers(m_ChunkCount, vb_array);
	glGenBuffers(m_ChunkCount, ib_array);
	glGenVertexArrays(m_ChunkCount, vao_array);
	for (int z = 0; z < chunkWidth; z++)
	{
		for (int y = 0; y < chunkHeight; y++)
		{
			for (int x = 0; x < chunkWidth; x++)
			{
				int i = x + y * chunkWidth + z * chunkWidth * chunkHeight;
				m_Chunks[i] = 
					new Chunk(glm::vec3(x, y, z), 
						new VertexArray(vao_array[i]),
						new VertexBuffer(vb_array[i]), 
						new IndexBuffer(ib_array[i]));
			}
		}
	}
	while (m_Running) {
		//aquire the lock, check if we should update chunks, and if not, wait untill we do.
		glm::vec3 currentChunk;
		{
			std::unique_lock<std::mutex> lk(m_ChunksLock);
			if (!m_ShouldUpdateChunks) {
				m_ChunkCV.wait(lk);
			}
			currentChunk = m_CurrentChunk;
		}
		uint32_t chunkDist = m_GameConfiguration->chunkRenderDistance, 
			chunkHeight = m_GameConfiguration->chunkRenderHeight;
		for (size_t i = 0; i < m_ChunkCount; i++)
		{
			Chunk* chunk = m_Chunks[i];
			glm::vec3 chunkPos = chunk->GetPositionChunkSpace();
			if (chunkPos.x < currentChunk.x - chunkDist ||
				chunkPos.x > currentChunk.x + chunkDist ||
				chunkPos.z < currentChunk.z - chunkDist ||
				chunkPos.z > currentChunk.z + chunkDist ||
				chunkPos.y < currentChunk.y - chunkHeight ||
				chunkPos.y > currentChunk.y + chunkHeight) {
				// chunk is out side of render area.
				// if the chunk at position 'p' relativly to the player last position is now out of render area,
				// the chunk at position '-p' relativly to the player current  position must be in the render area.
				chunk->LoadPosition(currentChunk - (chunkPos - m_LastChunkPosition));
			}
		}
		m_LastChunkPosition = currentChunk;
		// finished recalculating chunks!
	}
	// save and close
}


World::~World() {
	m_Running = false;
	m_ChunkThread.join();
	delete m_Chunks;
}
