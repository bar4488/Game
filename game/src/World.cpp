#include "World.h"

World::World(Renderer* renderer, GameConfiguration* gameConf, KeyboardMgr* keyboard) :
	m_Player(glm::vec3(-4, 0, 0), glm::vec3(1, 0, 0), &m_ChunkMgr),
	m_GameConfiguration(gameConf),
	m_Configuration(),
	m_KeyboardMgr(keyboard),
	m_Renderer(renderer),
	m_ChunkMgr(renderer, gameConf, glm::vec3(-4, 0,0)),
	m_Skybox(renderer),
	m_Crosshair(renderer)
{
	auto pos = m_ChunkMgr.GetChunkByPosition(m_Player.GetCurrentChunkPosition());
	m_Player.m_Position.y = pos->GetHeight() + 3;
	m_Renderer->LoadTexture("dirt", "res/textures/dirt.png");
	m_Renderer->LoadProgram("block", "res/shaders/block_vertex.shader", "res/shaders/block_fragment.shader");
}

glm::ivec2 GetNextBlock(glm::vec2 startPosition, glm::vec2 slope, glm::ivec2 block)
{
	if(slope == glm::zero<glm::vec2>())
	{
		return block;
	}
	else
	{
		if(abs(slope.x) >= abs(slope.y))
		{
			slope = slope / abs(slope.x);
			if(slope.x > 0)
			{
				// the y position of the line when its x is exactly on the block
				float yPos = startPosition.y - (startPosition.x - (block.x+1)) * slope.y;
				if(yPos < block.y + 1 && yPos >= block.y)
				{
					return block + glm::ivec2(1, 0);
				}
				else if(yPos >= block.y)
				{
					return block + glm::ivec2(0, 1);
				}
				else
				{
					return block + glm::ivec2(0, -1);
				}
			}
			if(slope.x < 0 )
			{
				// the y position of the line when its x is exactly on the block
				float yPos = startPosition.y + (startPosition.x - block.x) * slope.y;
				if(yPos < block.y + 1 && yPos >= block.y)
				{
					return block - glm::ivec2(1, 0);
				}
				else if(yPos >= block.y)
				{
					return block + glm::ivec2(0, 1);
				}
				else
				{
					return block + glm::ivec2(0, -1);
				}
			}
		}
		else
		{
			glm::ivec2 r = GetNextBlock(glm::vec2(startPosition.y, startPosition.x), glm::vec2(slope.y, slope.x), glm::ivec2(block.y, block.x));
			return glm::ivec2(r.y, r.x);
		}
	}
}

glm::ivec3 GetNextBlock3d(glm::vec3 startPosition, glm::vec3 slope, glm::ivec3 block)
{
	glm::ivec2 nextXY = GetNextBlock(startPosition, slope, block);
	glm::ivec2 nextZY = GetNextBlock(glm::vec2(startPosition.z, startPosition.y), glm::vec2(slope.z, slope.y), glm::ivec2(block.z, block.y));
	if(nextXY.y < nextZY.y)
	{
		return glm::ivec3(nextXY, block.z);
	}
	else if(nextZY.y < nextXY.y)
	{
		return glm::ivec3(block.x, nextZY.y, nextZY.x);
	}
	else
	{
		return glm::ivec3(nextXY, nextZY.x);
	}
}


void World::Update()
{
	// update the player
	m_Player.Update(m_GameConfiguration->window, m_GameConfiguration->width, m_GameConfiguration->height);
	glm::ivec2 pc = m_Player.GetCurrentChunkPosition();
	m_ChunkMgr.SetCurrenChunk(pc);

	if(m_Player.m_IsPointing)
	{
		if(m_KeyboardMgr->IsPressStarted(GLFW_KEY_Q))
		{
			m_ChunkMgr.SetBlockId(m_Player.m_PointedBlock, 0);
		}
	}
	if(m_Player.m_IsPointingTop)
	{
		if(m_KeyboardMgr->IsPressStarted(GLFW_KEY_E))
		{
			m_ChunkMgr.SetBlockId(m_Player.m_PointedBlockTop, 1);
		}
	}
}

void World::Draw()
{
	m_Renderer->BeginDraw(m_Player.GetViewProjection(), m_Player.m_Position, m_Player.GetViewDirection());
	m_ChunkMgr.Draw();
	if(m_Player.m_IsPointing)
	{
		m_Renderer->DrawCube(m_Player.m_PointedBlock, m_Renderer->m_ViewProjection, glm::vec3(1, 0, 0));
	}
	m_Skybox.Draw();
	m_Crosshair.Draw();

	m_Renderer->EndDraw(m_Player.GetViewDirection());
}


World::~World() 
{
}

void World::DrawSkybox()
{
}

void World::DrawCrosshair()
{
}
