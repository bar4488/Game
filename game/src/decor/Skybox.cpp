#include "Skybox.h"
#include "../constants.h"

Skybox::Skybox(Renderer* renderer):
	m_Renderer(renderer),
	VB(),
	IB(),
	VAO()
{
	m_Renderer->LoadProgram("skybox", "res/shaders/skybox_vertex.shader", "res/shaders/skybox_fragment.shader");


	VB.SetData(&skybox_vertices[0], sizeof(skybox_vertices), GL_STATIC_DRAW);
	IB.SetData(&skybox_indices[0], 6 * 6, GL_UNSIGNED_BYTE);

	VertexBufferLayout vb_layout;
	vb_layout.Push<char>(3, 0);
	vb_layout.Push<char>(3, 1);
	VAO.AddBuffer(VB, vb_layout);
}

void Skybox::Draw()
{
	glm::mat4 skybox_vp = m_Renderer->m_Projection * m_Renderer->m_NTView;

	m_Renderer->DisableCap(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	Program* skyboxProgram = m_Renderer->GetProgramByName("skybox");
	skyboxProgram->Bind();
	skyboxProgram->SetUniformMatrix4fv("VP", 1, GL_FALSE, &skybox_vp[0][0]);
	skyboxProgram->SetUniform3f("lightDir", 0.2f, 1.0f, 0.7f);
	skyboxProgram->SetUniform3f("lightColor", 0.8f, 0.8f, 0.0f);
	m_Renderer->DrawElements(VAO, IB);

	glDepthFunc(GL_LESS);
	m_Renderer->EnableCap(GL_CULL_FACE);
}
