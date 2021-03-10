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
#include "utils/keyboard/KeyboardMgr.h"

class Game {
public:
    Game(int width, int height);
    ~Game();
    void Run();
    void Update();
    void Draw();
private:
    int m_Height, m_Width, m_TicksPerSecond;
    bool m_InGame{};
    bool m_Fullscreen;
    bool t_pressed;
    bool full_ratio;
    bool f_pressed{};
    float viewport_ratio;
    GLFWwindow *m_Window;
    World *world{};
    Renderer m_Renderer;
    KeyboardMgr m_KeyboardMgr;
    Context m_Configuration;
private:
    void ProcessEvents();
    GLFWwindow* SetupGraphics();
};


#endif //GOME_GAME_H
