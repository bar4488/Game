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

#include <ft2build.h>
#include <freetype/freetype.h>  

using namespace std;

Renderer::Renderer(int width, int height) :
	m_Textures(),
	m_BlockShader("res/shaders/vertex.shader", "res/shaders/fragment.shader"),
	m_Width(width),
	m_Height(height) {
	Texture* texture = new Texture("res/textures/dirt.png");
	m_Textures[1] = texture;
}

glm::mat4 view;
void Renderer::BeginDraw(glm::mat4 View) {
	view = View;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Renderer::DrawChunk(Chunk& chunk) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
		(float)m_Width / (float)m_Height, 0.1f,
		0.0f);
	glm::mat4 vp = Projection * view;
	glm::mat4 model = glm::translate(glm::mat4(1.0), chunk.GetPositionWorldSpace());

	m_BlockShader.Bind();
	m_Textures[1]->Bind(0);
	m_BlockShader.SetUniform1i("tex", 0);
	m_BlockShader.SetUniformMatrix4fv("VP", 1, GL_FALSE, &vp[0][0]);
	m_BlockShader.SetUniform3f("lightDir", 0.2f, 1.0f, 0.7f);
	m_BlockShader.SetUniform3f("lightColor", 0.8f, 0.8f, 0.0f);
	m_BlockShader.SetUniformMatrix4fv("M", 1, GL_FALSE, &model[0][0]);
	chunk.Bind();
	glDrawElements(GL_TRIANGLES, chunk.GetIndicesCount(), GL_UNSIGNED_INT, nullptr);

}

Renderer::~Renderer() {
	m_Textures.erase(m_Textures.begin());
}
