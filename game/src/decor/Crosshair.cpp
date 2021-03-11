#include "Crosshair.h"
#include "../constants.h"
#include <glm\glm.hpp>
#include <glm\ext\matrix_clip_space.hpp>
#include <glm\ext\matrix_transform.hpp>

Crosshair::Crosshair(Renderer* renderer) :
	m_Renderer(renderer),
	VAO(),
	VB()
{
	m_Renderer->LoadProgram("crosshair", "res/shaders/crosshair_vertex.shader", "res/shaders/crosshair_fragment.shader");

	VB.SetData(&crosshair_vertices[0], sizeof(crosshair_vertices), GL_STATIC_DRAW);

	VertexBufferLayout vb_layout;
	vb_layout.Push<float>(3, 0);
	vb_layout.Push<float>(3, 1);
	VAO.AddBuffer(VB, vb_layout);
}

void Crosshair::Draw()
{
	glm::mat4 crosshair_p = m_Renderer->GetOrthoProjection(1.0f);
	glm::mat4 crosshair_vp = crosshair_p * m_Renderer->m_NTView;
	m_Renderer->DisableCap(GL_DEPTH_TEST);
	Program* program = m_Renderer->GetProgramByName("crosshair");
	program->Bind();
	program->SetUniform<glm::mat4>("view", crosshair_vp);
	m_Renderer->DrawLines(VAO, 0, 6, 3.0f);
	m_Renderer->EnableCap(GL_DEPTH_TEST);
}

