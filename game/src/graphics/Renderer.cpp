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

using namespace std;

Renderer::Renderer(int width, int height) :
	m_Width(width),
	m_Height(height),
	m_FreeType(this)
{
	m_CubeVB.SetData(normal_cube_vertices, sizeof(normal_cube_vertices), GL_STATIC_DRAW);
	m_CubeIB.SetData(skybox_indices, 24, GL_UNSIGNED_INT, GL_STATIC_DRAW);
	VertexBufferLayout vl;
	
	vl.Push<int>(3, 0);
	m_CubeVAO.AddBuffer(m_CubeVB, vl);
	LoadProgram("cube", "cube_vertex.shader", "cube_fragment.shader");
}

void Renderer::BeginDraw(glm::mat4 View, glm::vec3 cameraPos, glm::vec3 cameraDir) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_View = View;
	m_NTView = glm::mat4(glm::mat3(View));
	m_Projection = glm::perspective(glm::radians(45.0f),
		(float)m_Width / (float)m_Height, 0.1f,
		2000.0f);
	m_InverseProjection = glm::inverse(m_Projection);
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

void Renderer::DrawText(std::string font, std::string text, float scale, glm::vec2 position, glm::vec3 color)
{
	DisableCap(GL_DEPTH_TEST);
	m_FreeType.RenderText(m_Fonts[font], text, scale, position, color);
	EnableCap(GL_DEPTH_TEST);
}

void Renderer::DrawText3D(std::string font, std::string text, float scale, glm::vec2 position, glm::vec3 color)
{
	std::cout << "ERROR: Drawing 3d text is not implemented!\n";
}

void Renderer::DrawLines(VertexArray& va, unsigned int first, unsigned int count, float width)
{
	va.Bind();
	glLineWidth(width);
	glDrawArrays(GL_LINES, first, count);
	va.Unbind();
}

void Renderer::DrawCube(glm::vec3 position, glm::mat4 mvp, glm::vec3 color)
{
	m_CubeVAO.Bind();
	m_CubeIB.Bind();
	Program* program = GetProgramByName("cube");
	program->Bind();
	program->SetUniformVec3("u_color", color);
	program->SetUniformMatrix4fv("MVP", 1, false, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, nullptr);
	m_CubeVAO.Unbind();
}

void Renderer::EndDraw(glm::vec3 cameraDir) {
}

void Renderer::LoadProgram(std::string name, std::string vertexPath, std::string fragmentPath)
{
	std::cout << "loading program " << name << "...\n";
	m_ProgramsMap[name] = new Program(vertexPath, fragmentPath);
	if (m_ProgramsMap[name]->m_Success == true)
		std::cout << "program loaded successfully\n";
	else
		std::cout << "could not load program!\n";
}

void Renderer::LoadTexture(std::string name, std::string texturePath)
{
	m_Textures[name] = new Texture(texturePath);
}

void Renderer::LoadFont(std::string name, std::string font_path, unsigned int size)
{
	m_Fonts[name] = new Font(&m_FreeType, font_path, size);
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

glm::mat4 Renderer::GetOrthoProjection()
{
	return glm::ortho(0.0f, static_cast<float>(m_Width), 0.0f, static_cast<float>(m_Height));
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

Frustum& Renderer::GetFrustum()
{
	return m_Frustum;
}

Renderer::~Renderer() {
}
