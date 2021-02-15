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
}

void Renderer::BeginDraw(glm::mat4 View, glm::vec3 cameraPos, glm::vec3 cameraDir) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_View = View;
	m_NTView = glm::mat4(glm::mat3(View));
	m_Projection = glm::perspective(glm::radians(45.0f),
		(float)m_Width / (float)m_Height, 0.1f,
		2000.0f);
	m_ViewProjection = m_Projection * m_View;
	m_CameraPosition = cameraPos;
	m_Frustum = Frustum(m_ViewProjection);
}

void Renderer::DrawElements(VertexArray& vb, IndexBuffer& ib)
{
	vb.Bind();
	ib.Bind();
	glDrawElements(GL_TRIANGLES, ib.GetCount(), ib.GetType(), nullptr);
	vb.Unbind();
	ib.Unbind();
}

void Renderer::DrawLines(VertexArray& va, unsigned int first, unsigned int count, float width)
{
	va.Bind();
	glLineWidth(width);
	glDrawArrays(GL_LINES, first, count);
	va.Unbind();
}

void Renderer::EndDraw(glm::vec3 cameraDir) {
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

void Renderer::EnableCap(unsigned int c)
{
	glEnable(c);
}

void Renderer::DisableCap(unsigned int c)
{
	glDisable(c);
}

// returns an orthogonal projection, centered at 0 with ratio of the renderers screen and of size 'size'
glm::mat4 Renderer::GetOrthoProjection(float size)
{
	return glm::ortho(-1 * size * (float)m_Width / (float)m_Height, size * (float)m_Width / (float)m_Height, -1 * size, size);
}

Program* Renderer::GetProgramByName(std::string name)
{
	return m_ProgramsMap[name];
}

Frustum& Renderer::GetFrustrum()
{
	return m_Frustum;
}

Renderer::~Renderer() {
	m_Textures.erase(m_Textures.begin());
}
