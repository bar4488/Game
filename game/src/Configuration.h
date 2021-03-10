#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <cstdint>

class KeyboardMgr;
class ChunksManager;

typedef struct {
    uint32_t chunkRenderDistance;
    uint32_t chunkRenderHeight;
    int width;
    int height;
    KeyboardMgr* keyboardManager;
    GLFWwindow* window;
} Context ;

typedef struct {
    ChunkManager* chunksManager;
} WorldConficuration ;
