//
// Created by bar44 on 11/2/2020.
//

#ifndef GOME_GAME_H
#define GOME_GAME_H

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "graphics/Renderer.h"
#include "Player.h"
#include "World.h"
#include "Configuration.h"

class Game {
public:
    Game(int width, int height);
    ~Game();
    void Run();
    GLFWwindow* SetupGraphics();
    void Update();
    void Draw();
private:
    int m_Height, m_Width, m_TicksPerSecond;
    bool m_InGame;
    bool m_Fullscreen;
    GLFWwindow *m_Window;
    World *world;
    Renderer m_Renderer;
    GameConfiguration m_Configuration;
};


#endif //GOME_GAME_H
