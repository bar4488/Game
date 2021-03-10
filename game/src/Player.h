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

class World;

class Player {
public:
    Player(glm::vec3 initialPosition, glm::vec3 initialDirection, World* world);
    void Update(GLFWwindow *window, int width, int height);
    glm::mat4 GetViewProjection() const;
    glm::ivec2 GetCurrentChunkPosition() const;
    glm::vec3 GetViewDirection() const;
    glm::vec3 CalculateMovement();
    glm::vec3 m_Position;
    glm::ivec3 m_PointedBlock;
    glm::ivec3 m_PointedBlockTop;
    bool m_Flying = false;
    bool m_IsPointing;
    bool m_IsPointingTop;
    glm::vec2 m_Direction;
    glm::vec3 m_Speed;
    glm::vec3 m_Acceleration;
private:
    void CalculateTerrainMouseIntersection();
    glm::mat4 m_ViewProjection;

    World* m_World;
};


#endif //GOME_PLAYER_H
