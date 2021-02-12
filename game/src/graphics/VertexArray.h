//
// Created by bar44 on 10/29/2020.
//

#ifndef GOME_VERTEXARRAY_H
#define GOME_VERTEXARRAY_H

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"


class VertexArray {
private:
    unsigned int m_RendererID;
public:
    VertexArray();
    VertexArray(unsigned int rendererId);
    ~VertexArray();

    void Bind() const;
    void Unbind() const;
    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const;

};


#endif //GOME_VERTEXARRAY_H
