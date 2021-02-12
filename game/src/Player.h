//
// Created by bar44 on 11/3/2020.
//

#ifndef GOME_PLAYER_H
#define GOME_PLAYER_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>

class Player {
public:
    Player(glm::vec3 initialPosition, glm::vec3 initialDirection);
    void Update(GLFWwindow *window, int width, int height);
    glm::mat4 GetViewProjection() const;
    glm::vec3 GetCurrentChunkPosition() const;
    glm::vec3 m_Position;
    glm::vec2 m_Direction;
private:
    glm::mat4 m_ViewProjection;
};


#endif //GOME_PLAYER_H
