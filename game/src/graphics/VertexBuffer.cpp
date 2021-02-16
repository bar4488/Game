//
// Created by bar44 on 10/29/2020.
//

#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void *data, unsigned int size, GLenum usage) {
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    VertexBuffer::SetData(data, size, usage);
}

VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &m_RendererID);
}

VertexBuffer::VertexBuffer(unsigned int rendererId)
    : m_RendererID(rendererId)
{
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const void *data, unsigned int size, GLenum usage) {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void VertexBuffer::SubData(const void* data, unsigned offset, unsigned size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

