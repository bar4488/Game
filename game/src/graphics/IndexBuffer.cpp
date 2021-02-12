//
// Created by bar44 on 10/29/2020.
//

#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const void *data, unsigned int count, GLenum usage)
    : m_Count(count){
    glGenBuffers(1, &m_RendererID);
    IndexBuffer::SetData(data, count, usage);
}

IndexBuffer::IndexBuffer(): m_Count(0) {
    glGenBuffers(1, &m_RendererID);
}

IndexBuffer::IndexBuffer(unsigned int rendererId)
    : m_RendererID(rendererId),
    m_Count(0)
{
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int IndexBuffer::GetCount() const {
    return m_Count;
}

void IndexBuffer::SetData(const void * data, unsigned int count, GLenum usage) {
    m_Count = count;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, usage);
}


