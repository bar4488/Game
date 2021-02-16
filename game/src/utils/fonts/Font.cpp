#include "Font.h"

#include <iostream>
#include <GL/glew.h>



#include "FreeType.h"

Font::Font(FreeType* ft, std::string font_path, unsigned int size)
{
	if(FT_New_Face(ft->m_Ft, font_path.c_str(), 0, &m_face_))
	{
		std::cout << "ERROR:FREETYPE: failed to load font " << font_path << "!\n";
	}
	SetSize(size);
	LoadAscii();
}

void Font::LoadAscii()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(m_face_, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			m_face_->glyph->bitmap.width,
			m_face_->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			m_face_->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture, 
			glm::ivec2(m_face_->glyph->bitmap.width, m_face_->glyph->bitmap.rows),
			glm::ivec2(m_face_->glyph->bitmap_left, m_face_->glyph->bitmap_top),
			m_face_->glyph->advance.x
		};
		m_characters.insert(std::pair<char, Character>(c, character));
	}
}

void Font::SetSize(unsigned int size)
{
	FT_Set_Pixel_Sizes(m_face_, 0, size);
}

Character Font::GetCharacter(char c)
{
	return m_characters[c];
}
