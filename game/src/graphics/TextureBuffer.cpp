#include "TextureBuffer.h"

TextureBuffer::TextureBuffer(GLenum format)
{
	glGenTextures(1, &m_TextureID);
	glGenBuffers(1, &m_BufferID);
	glBindBuffer(GL_TEXTURE_BUFFER, m_BufferID);
	glBindTexture(GL_TEXTURE_BUFFER, m_TextureID);
	glTexBuffer(GL_TEXTURE_BUFFER, format, m_BufferID);

}

void TextureBuffer::SetData(const void* data, unsigned int size, GLenum usage)
{
    glBindBuffer(GL_TEXTURE_BUFFER, m_BufferID);
    glBufferData(GL_TEXTURE_BUFFER, size, data, usage);
}
void TextureBuffer::SubData(const void* data, unsigned int offset, unsigned int size)
{
    glBindBuffer(GL_TEXTURE_BUFFER, m_BufferID);
    glBufferSubData(GL_TEXTURE_BUFFER, offset, size, data);
}

void TextureBuffer::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_BUFFER, m_TextureID);
}

void TextureBuffer::Unbind()
{
	glBindTexture(GL_TEXTURE_BUFFER, 0);
}

