#include "World.h"

World::World(Renderer* renderer, GameConfiguration* gameConf) :
	m_Player(glm::vec3(-4, 0, 0), glm::vec3(1, 0, 0)),
	m_GameConfiguration(gameConf),
	m_Configuration(),
	m_Renderer(renderer),
	m_ChunkMgr(renderer, gameConf, m_Player.GetCurrentChunkPosition()),
	m_Skybox(renderer),
	m_Crosshair(renderer)
{
	auto pos = m_ChunkMgr.GetChunkByPosition(m_Player.GetCurrentChunkPosition());
	m_Player.m_Position.y = pos->GetHeight() + 3;
	m_Renderer->LoadTexture("dirt", "res/textures/dirt.png");
	m_Renderer->LoadProgram("block", "res/shaders/block_vertex.shader", "res/shaders/block_fragment.shader");
}

void World::Update()
{
	// update the player
	m_Player.Update(m_GameConfiguration->window, m_GameConfiguration->width, m_GameConfiguration->height);
	glm::ivec2 pc = m_Player.GetCurrentChunkPosition();
	m_ChunkMgr.SetCurrenChunk(pc);
}

void World::Draw()
{
	m_Renderer->BeginDraw(m_Player.GetViewProjection(), m_Player.m_Position, m_Player.GetViewDirection());
	m_ChunkMgr.Draw();
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
