//
// Created by bar44 on 11/3/2020.
//

#include "Player.h"
#include "Chunk.h"

void Player::Update(GLFWwindow* window, int width, int height) {
    glm::vec3 direction = glm::vec3(
            cos(m_Direction.y) * sin(m_Direction.x),
            sin(m_Direction.y),
            cos(m_Direction.y) * cos(m_Direction.x)
    );
    glm::vec3 right = glm::vec3(
            sin(m_Direction.x - 3.14f/2.0f),
            0,
            cos(m_Direction.x - 3.14f/2.0f)
    );
    m_ViewProjection =  glm::lookAt(
            m_Position,
            m_Position + direction,
            glm::cross(right, direction)
    );
    float speed = 0.1f;
    int ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
    if(ctrl == GLFW_PRESS) {
        speed = 0.4;
    }
    int w = glfwGetKey(window, GLFW_KEY_W);
    if(w == GLFW_PRESS) {
        m_Position += direction * speed;
    }
    int s = glfwGetKey(window, GLFW_KEY_S);
    if(s == GLFW_PRESS) {
        m_Position -= direction * speed;
    }
    int d = glfwGetKey(window, GLFW_KEY_D);
    if(d == GLFW_PRESS) {
        m_Position += right * speed;
    }
    int a = glfwGetKey(window, GLFW_KEY_A);
    if(a == GLFW_PRESS) {
        m_Position -= right * speed;
    }
    int space = glfwGetKey(window, GLFW_KEY_SPACE);
    if(space == GLFW_PRESS) {
        m_Position += glm::vec3(0,1,0) * speed;
    }
    int shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    if(shift == GLFW_PRESS) {
        m_Position += glm::vec3(0,1,0) * -speed;
    }
    float mouseSpeed = 0.005;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    // Compute new orientation
    m_Direction.y = glm::clamp(m_Direction.y + mouseSpeed * float((float)height/2 - ypos), -3.14f/2, 3.14f/2);
    m_Direction.x += mouseSpeed * float( (float)width/2 - xpos );

    glfwSetCursorPos(window, (double)width / 2, (double)height / 2);
}

Player::Player(glm::vec3 initialPosition, glm::vec3 initialDirection)
: m_Position(initialPosition), m_Direction(initialDirection){

}

glm::mat4 Player::GetViewProjection() const {
    return m_ViewProjection;
}

glm::vec3 Player::GetCurrentChunkPosition() const
{
    return glm::vec3(floor(m_Position.x / CHUNK_SIZE), floor(m_Position.y / CHUNK_HEIGHT), floor(m_Position.z / CHUNK_SIZE));
}
