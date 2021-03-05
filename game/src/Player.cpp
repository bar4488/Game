//
// Created by bar44 on 11/3/2020.
//

#include "Player.h"
#include "Chunk.h"

Player::Player(glm::vec3 initialPosition, glm::vec3 initialDirection, ChunkManager* manager):
	m_Position(initialPosition),
	m_Direction(initialDirection),
	m_Speed(0),
	m_Acceleration(0),
	m_ChunkManager(manager)
{
}

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
    glm::mat4 view = glm::lookAt(
        m_Position,
        m_Position + direction,
        glm::cross(right, direction));
	m_ViewProjection = view;
    float speed = 0.1f;
    int ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
    if(ctrl == GLFW_PRESS) {
        speed = 0.8;
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

    CalculateTerrainMouseIntersection();

    glfwSetCursorPos(window, (double)width / 2, (double)height / 2);
}

glm::mat4 Player::GetViewProjection() const {
    return m_ViewProjection;
}

glm::ivec2 Player::GetCurrentChunkPosition() const
{
    return glm::ivec2(floor(m_Position.x / CHUNK_SIZE), floor(m_Position.z / CHUNK_SIZE));
}

glm::vec3 Player::GetViewDirection() const
{
    return glm::vec3(
            cos(m_Direction.y) * sin(m_Direction.x),
            sin(m_Direction.y),
            cos(m_Direction.y) * cos(m_Direction.x)
    );
}

void Player::CalculateTerrainMouseIntersection()
{
	const glm::vec3 startPosition = m_Position;
	auto slope = GetViewDirection();
	glm::ivec3 block = glm::ivec3(floor(startPosition.x), floor(startPosition.y), floor(startPosition.z));

	// out vector is startPosition + t*slope where t is the free variable.
	int stepX, stepY, stepZ; // the signes of the values
	stepX = (slope.x >= 0) - (slope.x < 0);
	stepY = (slope.y >= 0) - (slope.y < 0);
	stepZ = (slope.z >= 0) - (slope.z < 0);

	float maxX, maxY, maxZ; // how much we can travel without changing a voxel in a specific direction
    if (slope.x >= 0)
		maxX = slope.x == 0 ? INFINITY : (floor(startPosition.x) + 1 - startPosition.x) / slope.x;
    else 
		maxX = -(startPosition.x - floor(startPosition.x)) / slope.x;
    if (slope.y >= 0)
		maxY = slope.y == 0 ? INFINITY : (floor(startPosition.y) + 1 - startPosition.y) / slope.y;
    else 
		maxY = slope.y == 0 ? INFINITY : -(startPosition.y - floor(startPosition.y)) / slope.y;
    if (slope.z >= 0)
		maxZ = slope.z == 0 ? INFINITY : (startPosition.z - floor(startPosition.z)) / slope.z;
    else 
		maxZ = slope.z == 0 ? INFINITY : -(startPosition.z - floor(startPosition.z)) / slope.z;

	float deltaX, deltaY, deltaZ; // how far we can travel to move one unit in a specific direction
	deltaX = slope.x == 0 ? INFINITY : 1 / slope.x * stepX;
	deltaY = slope.y == 0 ? INFINITY : 1 / slope.y * stepY;
	deltaZ = slope.z == 0 ? INFINITY : 1 / slope.z * stepZ;

	glm::ivec3 output = block;

	while(glm::distance(glm::vec3(output), startPosition) <= 10)
	{
        if(m_ChunkManager->GetBlockId(output) != 0)
        {
            m_PointedBlock = output;
            m_IsPointing = true;
            return;
        }
		if(maxX < maxY)
		{
			if(maxX < maxZ)
			{
				output.x += stepX;
				maxX += deltaX;
			}
			else
			{
				output.z += stepZ;
				maxZ += deltaZ;
			}
		}
		else
		{
			if(maxY < maxZ)
			{
				output.y += stepY;
				maxY += deltaY;
			}
			else
			{
				output.z += stepZ;
				maxZ += deltaZ;
			}
		}
	}
    m_IsPointing = false;
}
