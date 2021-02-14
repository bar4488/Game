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
	m_Textures[1] = texture;

	LoadProgram("block", "res/shaders/block_vertex.shader", "res/shaders/block_fragment.shader");
	LoadProgram("skybox", "res/shaders/skybox_vertex.shader", "res/shaders/skybox_fragment.shader");
	LoadProgram("crosshair", "res/shaders/crosshair_vertex.shader", "res/shaders/crosshair_fragment.shader");

	VertexBufferLayout vb_layout;

	m_SkyboxVB.SetData(&skybox_vertices[0], sizeof(skybox_vertices), GL_STATIC_DRAW);
	m_SkyboxIB.SetData(&skybox_indices[0], 6 * 6, sizeof(unsigned char));
	vb_layout.Push<char>(3, 0);
	vb_layout.Push<char>(3, 1);
	m_SkyboxVAO.AddBuffer(m_SkyboxVB, vb_layout);

	m_CrosshairVB.SetData(&crosshair_vertices, sizeof(crosshair_vertices), GL_STATIC_DRAW);
	vb_layout.Clear();
	vb_layout.Push<float>(2, 0);
	m_CrosshairVAO.AddBuffer(m_CrosshairVB, vb_layout);
}

void Renderer::BeginDraw(glm::mat4 View, glm::vec3 cameraPos, glm::vec3 cameraDir) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_View = View;
	m_Projection = glm::perspective(glm::radians(45.0f),
		(float)m_Width / (float)m_Height, 0.1f,
		2000.0f);
	glm::mat4 vp = m_Projection * m_View;
	m_Frustum = Frustum(vp);
	Shader* blockShader = m_ProgramsMap["block"];
	blockShader->Bind();
	m_Textures[1]->Bind(0);
	blockShader->SetUniform1i("tex", 0);
	blockShader->SetUniformMatrix4fv("VP", 1, GL_FALSE, &vp[0][0]);
	blockShader->SetUniform3f("lightDir", 0.2f, 1.0f, 0.7f);
	blockShader->SetUniformVec3("viewPos", cameraPos);
	blockShader->SetUniform3f("lightColor", 0.8f, 0.8f, 0.0f);
}
void Renderer::DrawChunk(Chunk& chunk) {
	// frustum culling
	// extract planes from mvp matrix
	if (chunk.GetVisibleBlocksCount() != 0 && m_Frustum.CheckRect(chunk.GetPositionWorldSpace() + glm::vec3(CHUNK_SIZE / 2, CHUNK_HEIGHT / 2, CHUNK_SIZE / 2), CHUNK_SIZE, CHUNK_HEIGHT)) {
		glm::mat4 model = glm::translate(glm::mat4(1.0), chunk.GetPositionWorldSpace());
		m_ProgramsMap["block"]->SetUniformMatrix4fv("M", 1, GL_FALSE, &model[0][0]);
		chunk.Bind();
		glDrawElements(GL_TRIANGLES, chunk.GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
		chunk.Unbind();
	}
}

void Renderer::LoadProgram(std::string name, std::string vertexPath, std::string fragmentPath)
{
	m_ProgramsMap[name] = new Shader(vertexPath, fragmentPath);
}

void Renderer::EndDraw(glm::vec3 cameraDir) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
		(float)m_Width / (float)m_Height, 0.1f,
		2000.0f);
	glm::mat4 skybox_vp = Projection * glm::mat4(glm::mat3(m_View));

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	Shader* skyboxShader = m_ProgramsMap["skybox"];
	skyboxShader->Bind();
	skyboxShader->SetUniformMatrix4fv("VP", 1, GL_FALSE, &skybox_vp[0][0]);
	skyboxShader->SetUniform3f("lightDir", 0.2f, 1.0f, 0.7f);
	skyboxShader->SetUniform3f("lightColor", 0.8f, 0.8f, 0.0f);
	m_SkyboxVAO.Bind();
	m_SkyboxVB.Bind();
	m_SkyboxIB.Bind();
	glDrawElements(GL_TRIANGLES, m_SkyboxIB.GetCount(), GL_UNSIGNED_BYTE, nullptr);

	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);
	m_ProgramsMap["crosshair"]->Bind();
	m_ProgramsMap["crosshair"]->SetUniform1f("width", m_Width);
	m_ProgramsMap["crosshair"]->SetUniform1f("height", m_Height);
	m_CrosshairVAO.Bind();
	m_CrosshairVB.Bind();
	glLineWidth(3.0f);
	glDrawArrays(GL_LINES, 0, 4);
	glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {
	m_Textures.erase(m_Textures.begin());
}
