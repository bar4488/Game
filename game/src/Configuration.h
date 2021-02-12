#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <cstdint>

typedef struct {
    uint32_t chunkRenderDistance;
    uint32_t chunkRenderHeight;
    int width;
    int height;
    GLFWwindow* window;
    GLFWwindow* slaveWindow;
} GameConfiguration ;

typedef struct {
    
} WorldConficuration ;
