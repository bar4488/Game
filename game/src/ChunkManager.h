#pragma once
#include <cstdint>
#include "Chunk.h"
#include <mutex>
#include <thread>
#include "graphics/Renderer.h"
#include "Configuration.h"
#include "FastNoise/FastNoise.h"

class ChunkManager
{
public:
    ChunkManager(Renderer *renderer, GameConfiguration *gameConf, glm::ivec2 currentChunk);
    ~ChunkManager();
    void Draw();
    void Update();
    void SetCurrenChunk(glm::ivec2 currentChunk);
    Chunk* GetChunkByPosition(glm::ivec2 position);
    uint32_t GetChunkCount();
    uint32_t GetRenderedChunksCount();
private:
    void RunChunkLoader();
    int CalculateChunkIndex(glm::ivec2 chunkPosition, glm::ivec2 centerPosition);
    glm::ivec2 CalculateChunkPositionByIndex(int index, glm::ivec2 centerPosition);
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
    bool debugStopUpdateChunks;
    bool m_Running;
    FastNoise::SmartNode<> m_Noise;
    glm::ivec2 m_LastChunkPosition;
    glm::ivec2 m_CurrentChunk;
    std::mutex m_ChunksLock; // lock for when we try to change shouldUpdateChunks
    std::mutex m_BackBufferLock; // lock for when we try to change between m_Chunks to m_BackBufferChunks
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
