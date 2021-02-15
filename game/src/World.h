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
public:
    void DrawSkybox();
    void DrawCrosshair();
private:
    GameConfiguration *m_GameConfiguration;
    WorldConficuration m_Configuration;
    Renderer *m_Renderer;
    Player m_Player;
    ChunkManager m_ChunkMgr;
};

