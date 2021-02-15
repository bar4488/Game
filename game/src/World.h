#pragma once

#include "graphics/Renderer.h"
#include "Player.h"
#include <thread>
#include <mutex>
#include "Configuration.h"
#include "ChunkManager.h"


class World
{
public: 
    World(Renderer *renderer, GameConfiguration *gameConf);
    void Update();
    void Draw();
    ~World();
private:
    Player m_Player;
    ChunkManager m_ChunkMgr;
    WorldConficuration m_Configuration;
    GameConfiguration *m_GameConfiguration;
    Renderer *m_Renderer;
};

