#pragma once
#include <GL/glew.h>

class TextureBuffer
{
public:
	explicit TextureBuffer(GLenum format);
    void SetData(const void * data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
    void SubData(const void * data, unsigned int offset, unsigned int size);

    void Bind(unsigned int slot) const;
    void Unbind();
private:
	unsigned int m_TextureID;
	unsigned int m_BufferID;
};
