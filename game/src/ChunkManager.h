#pragma once
#include <cstdint>
#include "Chunk.h"
#include <mutex>
#include <thread>
#include "graphics/Renderer.h"
#include "Configuration.h"
#include "utils/noise/PerlinNoise.h"

class ChunkManager
{
public:
    ChunkManager(Renderer *renderer, GameConfiguration *gameConf, glm::ivec3 currentChunk);
    ~ChunkManager();
    void Draw();
    void Update();
    void SetCurrenChunk(glm::ivec3 currentChunk);
    Chunk* GetChunkByPosition(glm::ivec3 position);
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
    // the other thread will modify this array, and when ready we will switch between them
	Chunk** m_BackBufferChunks;
    bool m_ShouldUpdateChunks;
    bool m_Running;
    siv::PerlinNoise m_Noise;
    glm::ivec3 m_LastChunkPosition;
    glm::ivec3 m_CurrentChunk;
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
