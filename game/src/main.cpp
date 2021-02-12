#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <cstdio>
#include <iostream>
#include "vector"
#include <glm/gtc/matrix_transform.hpp>

#include "Game.h"

using namespace std;


static const int width = 640;
static const int height = 480;

int main() {
    Game game(1920, 1080);
    game.Run();

    return 0;
}
