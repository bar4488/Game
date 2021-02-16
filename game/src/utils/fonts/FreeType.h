#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>


#include "Font.h"
#include "../../graphics/VertexArray.h"
#include "../../graphics/VertexBuffer.h"
#include "glm/vec3.hpp"

class Renderer;

class FreeType
{
public:
	FreeType(Renderer* renderer);
	void RenderText(Font* font, std::string text, float scale, glm::vec2 pos, glm::vec3 color);
	FT_Library m_Ft;
private:
	Renderer* m_renderer;
	VertexBuffer VB;
	VertexArray VAO;
};
