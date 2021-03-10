//
// Created by bar44 on 11/3/2020.
//

#include "Player.h"
#include "Chunk.h"
#include "World.h"

Player::Player(glm::vec3 initialPosition, glm::vec3 initialDirection, World* world):
	m_Position(initialPosition),
	m_Direction(initialDirection),
	m_Speed(0),
	m_Acceleration(0),
    m_World(world)
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
    KeyboardMgr* k_mgr = m_World->m_KeyboardMgr;
    if(k_mgr->IsPressStarted(GLFW_KEY_H))
    {
        m_Flying = !m_Flying;
    }
    if(k_mgr->IsPressed(GLFW_KEY_LEFT_CONTROL))
    {
        speed = 0.2;
    }
    glm::vec3 v_speed;
    if(m_Flying)
    {
        v_speed = glm::vec3();
		if(k_mgr->IsPressed(GLFW_KEY_W)) {
			v_speed += direction * speed;
		}
		if(k_mgr->IsPressed(GLFW_KEY_S)) {
			v_speed -= direction * speed;
		}
		if(k_mgr->IsPressed(GLFW_KEY_D)) {
			v_speed += right * speed;
		}
		if(k_mgr->IsPressed(GLFW_KEY_A)) {
			v_speed -= right * speed;
		}
        if (k_mgr->IsPressed(GLFW_KEY_SPACE))
        {
            v_speed.y += 0.2;
        }
    }
    else
    {
        v_speed = m_Speed;
        glm::vec2 sp = glm::vec2();
		if(k_mgr->IsPressed(GLFW_KEY_W)) {
			sp.y += speed;
		}
		if(k_mgr->IsPressed(GLFW_KEY_S)) {
			sp.y -= speed;
		}
		if(k_mgr->IsPressed(GLFW_KEY_D)) {
			sp.x += speed;
		}
		if(k_mgr->IsPressed(GLFW_KEY_A)) {
			sp.x -= speed;
		}
        if (k_mgr->IsPressStarted(GLFW_KEY_SPACE)) {
            v_speed += glm::vec3(0, 0.3, 0);
        }

        glm::vec3 front;
		front = glm::normalize(glm::vec3(direction.x, 0, direction.z));
        v_speed = glm::vec3(0, v_speed.y, 0) + sp.x * right + sp.y * front;

		// gravity
		if(!m_Flying && v_speed.y > -0.5)
		{
			v_speed.y -= 0.01;
		}
    }
    m_Speed = v_speed;
    // calculate collisions
    m_Position = CalculateMovement();

    CalculateTerrainMouseIntersection();

    float mouseSpeed = 0.005;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    // Compute new orientation
    m_Direction.y = glm::clamp(m_Direction.y + mouseSpeed * float((float)height/2 - ypos), -3.14f/2, 3.14f/2);
    m_Direction.x += mouseSpeed * float( (float)width/2 - xpos );
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

glm::vec3 Player::CalculateMovement()
{
    glm::vec3 pos = m_Speed + m_Position;
    glm::ivec3 lastBlockPos = glm::ivec3(floor(m_Position.x), floor(m_Position.y), floor(m_Position.z));
    glm::ivec3 blockPos = glm::ivec3(floor(pos.x), floor(pos.y), floor(pos.z));

    glm::vec3 newPos = m_Position;
    ChunkManager* manager = &m_World->m_ChunkMgr;
    if(manager->GetBlockId(lastBlockPos - glm::ivec3(0,1,0)) != 0 ||
		(manager->GetBlockId(glm::ivec3(lastBlockPos.x, floor(pos.y + 0.4), lastBlockPos.z) - glm::ivec3(0,1,0)) == 0 && 
		manager->GetBlockId(glm::ivec3(lastBlockPos.x, floor(pos.y - 0.4), lastBlockPos.z) - glm::ivec3(0,1,0)) == 0 && 
		manager->GetBlockId(glm::ivec3(lastBlockPos.x, floor(pos.y + 0.2f), lastBlockPos.z)) == 0 && 
		manager->GetBlockId(glm::ivec3(lastBlockPos.x, floor(pos.y - 0.2f), lastBlockPos.z)) == 0)
        )
    {
        newPos.y += m_Speed.y;
    }
    else
    {
        m_Speed.y = 0;
    }
    if(manager->GetBlockId(lastBlockPos) != 0 ||
		(manager->GetBlockId(glm::ivec3(floor(pos.x + 0.2f), lastBlockPos.y - 1, lastBlockPos.z)) == 0 &&
		manager->GetBlockId(glm::ivec3(floor(pos.x - 0.2f), lastBlockPos.y - 1, lastBlockPos.z)) == 0 &&
		manager->GetBlockId(glm::ivec3(floor(pos.x + 0.2f), lastBlockPos.y, lastBlockPos.z)) == 0 && 
		manager->GetBlockId(glm::ivec3(floor(pos.x - 0.2f), lastBlockPos.y, lastBlockPos.z)) == 0))
        
    {
        newPos.x += m_Speed.x;
    }
    else
    {
        m_Speed.x = 0;
    }
    if(manager->GetBlockId(lastBlockPos) != 0 ||
		(manager->GetBlockId(glm::ivec3(lastBlockPos.x, lastBlockPos.y - 1, floor(pos.z + 0.2f))) == 0 && 
		manager->GetBlockId(glm::ivec3(lastBlockPos.x, lastBlockPos.y - 1, floor(pos.z - 0.2f))) == 0 && 
		manager->GetBlockId(glm::ivec3(lastBlockPos.x, lastBlockPos.y, floor(pos.z + 0.2f))) == 0 &&
		manager->GetBlockId(glm::ivec3(lastBlockPos.x, lastBlockPos.y, floor(pos.z - 0.2f))) == 0)
        )
    {
        newPos.z += m_Speed.z;
    }
    else
    {
        m_Speed.z = 0;
    }
    return newPos;
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
		maxZ = slope.z == 0 ? INFINITY : (floor(startPosition.z) + 1 - startPosition.z) / slope.z;
    else 
		maxZ = slope.z == 0 ? INFINITY : -(startPosition.z - floor(startPosition.z)) / slope.z;

	float deltaX, deltaY, deltaZ; // how far we can travel to move one unit in a specific direction
	deltaX = slope.x == 0 ? INFINITY : 1 / slope.x * stepX;
	deltaY = slope.y == 0 ? INFINITY : 1 / slope.y * stepY;
	deltaZ = slope.z == 0 ? INFINITY : 1 / slope.z * stepZ;

    glm::ivec3 lastOutput;
	glm::ivec3 output = block;
    bool isFirst = true;

    m_IsPointing = false;
    m_IsPointingTop = false;
	while(glm::distance(glm::vec3(output), startPosition) <= 10)
	{
        if(m_World->m_ChunkMgr.GetBlockId(output) != 0)
        {
            m_PointedBlock = output;
            m_IsPointing = true;
            if(!isFirst)
            {
                m_IsPointingTop = true;
                m_PointedBlockTop = lastOutput;
            }
            return;
        }
        lastOutput = output;
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
        isFirst = false;
	}
}
