#include "FreeType.h"

#include <iostream>

#include "../../graphics/Renderer.h"


FreeType::FreeType(Renderer* renderer):
    m_renderer(renderer),
	VB(nullptr, sizeof(float) * 4 * 6, GL_DYNAMIC_DRAW),
	VAO()
{
	if (FT_Init_FreeType(&m_Ft))
	{
		std::cout << "ERROR:FREETYPE: could not initialize freetype library" << std::endl;
	}

	m_renderer->LoadProgram("text", "res/shaders/text_vertex.shader", "res/shaders/text_fragment.shader");
	// initialize buffer and vao
	VertexBufferLayout vl;
	vl.Push<float>(2, 0);
	vl.Push<float>(2, 1);
	VAO.AddBuffer(VB, vl);
}

void FreeType::RenderText(Font* font, std::string text, float scale, glm::vec2 pos, glm::vec3 color)
{
	Program* text_shader = m_renderer->GetProgramByName("text");
	text_shader->Bind();
    glm::mat4 projection = m_renderer->GetOrthoProjection();
	text_shader->SetUniformMatrix4fv("projection", 1, false, &projection[0][0]);
	text_shader->SetUniformVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    VAO.Bind();

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = font->GetCharacter(*c);

        float xpos = pos.x + ch.Bearing.x * scale;
        float ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        VB.Bind();
        VB.SubData(&vertices[0], 0, sizeof(vertices));
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        pos.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
