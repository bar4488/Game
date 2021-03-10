#pragma once

#include "graphics/Renderer.h"
#include "Player.h"
#include <thread>
#include <mutex>
#include "Configuration.h"
#include "ChunkManager.h"
#include "decor/Skybox.h"
#include "decor/Crosshair.h"
#include "utils/keyboard/KeyboardMgr.h"

class Game;

class World
{
public: 
    World(Renderer *renderer, Context *gameConf, KeyboardMgr* keyboard);
    void Update();
    void Draw();
    ~World();
public:
    void DrawSkybox();
    void DrawCrosshair();

    ChunkManager m_ChunkMgr;
    KeyboardMgr* m_KeyboardMgr;
private:
    Context *m_GameConfiguration;
    WorldConficuration m_Configuration;
    Renderer *m_Renderer;
    Skybox m_Skybox;
    Crosshair m_Crosshair;
    Player m_Player;

    friend class Game;
};

