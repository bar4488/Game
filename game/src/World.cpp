#include "World.h"

World::World(Renderer* renderer, GameConfiguration* gameConf) :
	m_Player(glm::vec3(-4, 0, 0), glm::vec3(1, 0, 0)),
	m_GameConfiguration(gameConf),
	m_Configuration(),
	m_Renderer(renderer),
	m_ChunkMgr(renderer, gameConf, m_Player.GetCurrentChunkPosition())
{
}

void World::Update()
{
	// update the player
	m_Player.Update(m_GameConfiguration->window, m_GameConfiguration->width, m_GameConfiguration->height);
	glm::vec3 pc = m_Player.GetCurrentChunkPosition();
	m_ChunkMgr.SetCurrenChunk(pc);
}

void World::Draw()
{
	m_Renderer->BeginDraw(m_Player.GetViewProjection(), m_Player.m_Position, m_Player.GetViewDirection());
	m_ChunkMgr.Draw();
	m_Renderer->EndDraw(m_Player.GetViewDirection());
}


World::~World() 
{
}
