#pragma once
#include <cstdint>
#include "Chunk.h"
#include <mutex>
#include <thread>
#include "graphics/Renderer.h"
#include "Configuration.h"
class ChunkManager
{
public:
    ChunkManager(Renderer *renderer, GameConfiguration *gameConf, glm::vec3 currentChunk);
    ~ChunkManager();
    void Draw();
    void Update();
    void SetCurrenChunk(glm::vec3 currentChunk);
    uint32_t GetChunkCount();
    uint32_t GetRenderedChunksCount();
private:
    void RunChunkLoader();
    uint32_t CalculateChunkCount();
private:
    Renderer *m_Renderer;
    GameConfiguration *m_GameConfiguration;
    uint32_t m_ChunkCount;
    uint32_t m_RenderedChunksCount;
	Chunk** m_Chunks;
    bool m_ShouldUpdateChunks;
    bool m_Running;
    glm::vec3 m_LastChunkPosition;
    glm::vec3 m_CurrentChunk;
    std::mutex m_ChunksLock; // lock for when we try to change shouldUpdateChunks
    std::thread m_ChunkThread;
    std::condition_variable m_ChunkCV;
};

inline uint32_t ChunkManager::GetChunkCount()
{
	return m_ChunkCount;
}

inline uint32_t ChunkManager::GetRenderedChunksCount()
{
	return m_RenderedChunksCount;
}
