//
// Created by bar44 on 10/29/2020.
//

#ifndef GOME_VERTEXBUFFER_H
#define GOME_VERTEXBUFFER_H


#include "GL/glew.h"
#include "GLFW/glfw3.h"

class VertexBuffer {
private:
    unsigned int m_RendererID;
public:
    explicit VertexBuffer(unsigned int rendererId);
    VertexBuffer(const void * data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
    VertexBuffer();
    ~VertexBuffer();

    void SetData(const void * data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
    void SubData(const void * data, unsigned int offset, unsigned int size);

    void Bind() const;
    void Unbind();
};


#endif //GOME_VERTEXBUFFER_H
