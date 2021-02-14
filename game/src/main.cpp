#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <cstdio>
#include <iostream>
#include "vector"
#include <glm/gtc/matrix_transform.hpp>

#include "Game.h"

using namespace std;


static const int width = 1920;
static const int height = 1080;

int main() {
    Game game(width, height);
    game.Run();

    return 0;
}
