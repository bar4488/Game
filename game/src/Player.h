//
// Created by bar44 on 11/3/2020.
//

#ifndef GOME_PLAYER_H
#define GOME_PLAYER_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>

#include "ChunkManager.h"

#define GRAVITY 0.1f

class Player {
public:
    Player(glm::vec3 initialPosition, glm::vec3 initialDirection, ChunkManager* manager);
    void Update(GLFWwindow *window, int width, int height);
    glm::mat4 GetViewProjection() const;
    glm::ivec2 GetCurrentChunkPosition() const;
    glm::vec3 GetViewDirection() const;
    glm::mat4 GetInverseViewMatrix();
    glm::vec3 m_Position;
    glm::ivec3 m_PointedBlock;
    bool m_IsPointing;
    glm::vec2 m_Direction;
    glm::vec3 m_Speed;
    glm::vec3 m_Acceleration;
private:
    void CalculateTerrainMouseIntersection();
    glm::mat4 m_ViewProjection;

    ChunkManager* m_ChunkManager;
};


#endif //GOME_PLAYER_H
