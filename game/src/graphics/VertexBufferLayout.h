//
// Created by bar44 on 10/29/2020.
//

#ifndef GOME_VERTEXBUFFERLAYOUT_H
#define GOME_VERTEXBUFFERLAYOUT_H

#include "vector"
#include "GL/glew.h"
#include <iostream>

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned int location;
    bool normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type) {
            case GL_FLOAT:
                return sizeof(float);
            case GL_UNSIGNED_INT:
                return sizeof(unsigned int);
            case GL_UNSIGNED_BYTE:
                return sizeof(unsigned char);
            case GL_BYTE:
                return sizeof(char);
        }
    }
};

class VertexBufferLayout {
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
    VertexBufferLayout();

    template<typename T>
    void Push(unsigned int count, unsigned int location) {
        //ERROR
        std::cerr << "VertexBufferLayout ERROR! unknown type!";
    }

    template<>
    void Push<unsigned int>(unsigned int count, unsigned int location) {
        m_Elements.push_back({GL_UNSIGNED_INT, count, location, false});
        m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
    }

    template<>
    void Push<float>(unsigned int count, unsigned int location) {
        m_Elements.push_back({GL_FLOAT, count, location, false});
        m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
    }

    template<>
    void Push<unsigned char>(unsigned int count, unsigned int location) {
        m_Elements.push_back({GL_UNSIGNED_BYTE, count, location, false});
        m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
    }

    template<>
    void Push<char>(unsigned int count, unsigned int location) {
        m_Elements.push_back({GL_BYTE, count, location, false});
        m_Stride += count * VertexBufferElement::GetSizeOfType(GL_BYTE);
    }

    inline std::vector<VertexBufferElement> GetElement() const { return m_Elements; }
    inline unsigned int GetStride() const { return m_Stride; }
};

#endif //GOME_VERTEXBUFFERLAYOUT_H
