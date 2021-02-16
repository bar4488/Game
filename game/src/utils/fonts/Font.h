#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <unordered_map>

#include "glm/vec2.hpp"

class FreeType;

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

class Font
{
public:
	Font(FreeType *ft, std::string font_path, unsigned int size);
	void SetSize(unsigned int size);
	Character GetCharacter(char c);
private:
	void LoadAscii();
	std::unordered_map<char, Character> m_characters;
	FT_Face m_face_;
};
