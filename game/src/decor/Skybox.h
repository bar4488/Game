#pragma once
#include "../graphics/Renderer.h"

class Skybox {
public:
	Skybox(Renderer* renderer);
	void Draw();
private:
	Renderer* m_Renderer;
	VertexArray VAO;
	VertexBuffer VB;
	IndexBuffer IB;
};
