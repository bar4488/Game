//
// Created by bar44 on 10/29/2020.
//

#include "Renderer.h"
#include "GL/glew.h"
#include "../Chunk.h"
#include <glm/glm.hpp>
#include <glm\ext\matrix_transform.hpp>
#include <glm\ext\matrix_clip_space.hpp>
#include <iostream>
#include "../constants.h"

#include <ft2build.h>
#include <freetype/freetype.h>  

using namespace std;

Renderer::Renderer(int width, int height) :
	m_Textures(),
	m_SkyboxVAO(),
	m_SkyboxVB(),
	m_SkyboxIB(),
	m_CrosshairVAO(),
	m_CrosshairVB(),
	m_Width(width),
	m_Height(height),
	m_Frustum() {
	Texture* texture = new Texture("res/textures/dirt.png");
	m_Textures["dirt"] = texture;

	LoadProgram("block", "res/shaders/block_vertex.shader", "res/shaders/block_fragment.shader");
	LoadProgram("skybox", "res/shaders/skybox_vertex.shader", "res/shaders/skybox_fragment.shader");
	LoadProgram("crosshair", "res/shaders/crosshair_vertex.shader", "res/shaders/crosshair_fragment.shader");

	VertexBufferLayout vb_layout;

	m_SkyboxVB.SetData(&skybox_vertices[0], sizeof(skybox_vertices), GL_STATIC_DRAW);
	m_SkyboxIB.SetData(&skybox_indices[0], 6 * 6, GL_UNSIGNED_BYTE);
	vb_layout.Push<char>(3, 0);
	vb_layout.Push<char>(3, 1);
	m_SkyboxVAO.AddBuffer(m_SkyboxVB, vb_layout);

	m_CrosshairVB.SetData(&crosshair_vertices, sizeof(crosshair_vertices), GL_STATIC_DRAW);
	vb_layout.Clear();
	vb_layout.Push<float>(3, 0);
	vb_layout.Push<float>(3, 1);
	m_CrosshairVAO.AddBuffer(m_CrosshairVB, vb_layout);
}

void Renderer::BeginDraw(glm::mat4 View, glm::vec3 cameraPos, glm::vec3 cameraDir) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_View = View;
	m_Projection = glm::perspective(glm::radians(45.0f),
		(float)m_Width / (float)m_Height, 0.1f,
		2000.0f);
	m_ViewProjection = m_Projection * m_View;
	m_CameraPosition = cameraPos;
	m_Frustum = Frustum(m_ViewProjection);
}

void Renderer::DrawElements(IndexBuffer& ib)
{
	ib.Bind();
	glDrawElements(GL_TRIANGLES, ib.GetCount(), ib.GetType(), nullptr);
	ib.Unbind();
}

void Renderer::EndDraw(glm::vec3 cameraDir) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
		(float)m_Width / (float)m_Height, 0.1f,
		2000.0f);
	glm::mat4 no_transform_view = glm::mat4(glm::mat3(m_View));
	glm::mat4 skybox_vp = Projection * no_transform_view;

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	Program* skyboxShader = m_ProgramsMap["skybox"];
	skyboxShader->Bind();
	skyboxShader->SetUniformMatrix4fv("VP", 1, GL_FALSE, &skybox_vp[0][0]);
	skyboxShader->SetUniform3f("lightDir", 0.2f, 1.0f, 0.7f);
	skyboxShader->SetUniform3f("lightColor", 0.8f, 0.8f, 0.0f);
	m_SkyboxVAO.Bind();
	DrawElements(m_SkyboxIB);

	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	glm::mat4 crosshair_m = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1.5));
	glm::mat4 crosshair_p = glm::ortho(-1 * (float)m_Width / (float)m_Height, (float)m_Width / (float)m_Height, -1.0f, 1.0f);
	glm::mat4 crosshair_vp = crosshair_p * no_transform_view;
	glDisable(GL_DEPTH_TEST);
	m_ProgramsMap["crosshair"]->Bind();
	m_ProgramsMap["crosshair"]->SetUniformMatrix4fv("view", 1, GL_FALSE, &crosshair_vp[0][0]);
	m_CrosshairVAO.Bind();
	m_CrosshairVB.Bind();
	glLineWidth(3.0f);
	glDrawArrays(GL_LINES, 0, 6);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::LoadProgram(std::string name, std::string vertexPath, std::string fragmentPath)
{
	m_ProgramsMap[name] = new Program(vertexPath, fragmentPath);
}

void Renderer::LoadTexture(std::string name, std::string texturePath)
{
	m_Textures[name] = new Texture(texturePath);
}

void Renderer::BindTexture(std::string name, unsigned int slot)
{
	m_Textures[name]->Bind(slot);
}

Program& Renderer::GetProgramByName(std::string name)
{
	return *m_ProgramsMap[name];
}

Frustum& Renderer::GetFrustrum()
{
	return m_Frustum;
}

Renderer::~Renderer() {
	m_Textures.erase(m_Textures.begin());
}
