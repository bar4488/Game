//
// Created by bar44 on 10/29/2020.
//

#include "VertexArray.h"
#include "vector"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_RendererID);
    glBindVertexArray(m_RendererID);
}

VertexArray::VertexArray(unsigned int rendererId)
    : m_RendererID(rendererId)
{
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::Bind() const {
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const {
    glBindVertexArray(0);
}

void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout) const {
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElement();
    unsigned long offset = 0;
    for (auto element: elements){
        glEnableVertexAttribArray(element.location);
        glVertexAttribPointer(
            element.location,
            element.count,
            element.type,
            element.normalized,
            layout.GetStride(),
            (const void *)offset
        );
        offset += element.count * glutls::GetSizeOfType(element.type);
    }
}

