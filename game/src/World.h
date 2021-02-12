#pragma once

#include "graphics/Renderer.h"
#include "Player.h"
#include <thread>
#include <mutex>
#include "Configuration.h"


class World
{
public: 
    World(Renderer *renderer, GameConfiguration *gameConf);
    void Update();
    void Draw();
    ~World();
    void RunChunkLoader();
private:
    Player m_Player;
    WorldConficuration m_Configuration;
    GameConfiguration *m_GameConfiguration;
    Renderer *m_Renderer;
    uint32_t m_ChunkCount;
	Chunk** m_Chunks;
    bool m_ShouldUpdateChunks;
    bool m_Running;
    glm::vec3 m_LastChunkPosition;
    glm::vec3 m_CurrentChunk;
    std::mutex m_ChunksLock; // lock for when we try to change shouldUpdateChunks
    std::thread m_ChunkThread;
    std::condition_variable m_ChunkCV;
};

