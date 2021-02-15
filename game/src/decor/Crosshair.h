#pragma once
#include "../graphics/Renderer.h"

class Crosshair {
public:
	Crosshair(Renderer* renderer);
	void Draw();
private:
	Renderer* m_Renderer;
	VertexArray VAO;
	VertexBuffer VB;
};
