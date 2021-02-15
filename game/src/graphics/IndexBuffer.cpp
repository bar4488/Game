//
// Created by bar44 on 10/29/2020.
//

#include "IndexBuffer.h"
#include "gl_utils.h"

IndexBuffer::IndexBuffer(const void *data, unsigned int count, unsigned int type, GLenum usage)
    : m_Count(count),
        m_Type(type)
{
    glGenBuffers(1, &m_RendererID);
    IndexBuffer::SetData(data, count, type, usage);
}

IndexBuffer::IndexBuffer(): m_Count(0) {
    glGenBuffers(1, &m_RendererID);
}

IndexBuffer::IndexBuffer(unsigned int rendererId)
    : m_RendererID(rendererId),
    m_Count()
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

unsigned int IndexBuffer::GetType() const {
    return m_Type;
}


void IndexBuffer::SetData(const void * data, unsigned int count, unsigned int type, GLenum usage) {
    m_Count = count;
    m_Type = type;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * glutls::GetSizeOfType(type), data, usage);
}


