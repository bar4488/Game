//
// Created by bar44 on 10/29/2020.
//

#ifndef GOME_VERTEXBUFFERLAYOUT_H
#define GOME_VERTEXBUFFERLAYOUT_H

#include "vector"
#include "GL/glew.h"
#include <iostream>
#include "gl_utils.h"

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned int location;
    bool normalized;
};

class VertexBufferLayout {
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
    VertexBufferLayout();

    void Clear() {
        m_Stride = 0;
        m_Elements.clear();
    }

    template<typename T>
    void Push(unsigned int count, unsigned int location) {
        //ERROR
        std::cerr << "VertexBufferLayout ERROR! unknown type!";
    }

    template<>
    void Push<unsigned int>(unsigned int count, unsigned int location) {
        m_Elements.push_back({GL_UNSIGNED_INT, count, location, false});
        m_Stride += count * glutls::GetSizeOfType(GL_UNSIGNED_INT);
    }

    template<>
    void Push<float>(unsigned int count, unsigned int location) {
        m_Elements.push_back({GL_FLOAT, count, location, false});
        m_Stride += count * glutls::GetSizeOfType(GL_FLOAT);
    }

    template<>
    void Push<unsigned char>(unsigned int count, unsigned int location) {
        m_Elements.push_back({GL_UNSIGNED_BYTE, count, location, false});
        m_Stride += count * glutls::GetSizeOfType(GL_UNSIGNED_BYTE);
    }

    template<>
    void Push<char>(unsigned int count, unsigned int location) {
        m_Elements.push_back({GL_BYTE, count, location, false});
        m_Stride += count * glutls::GetSizeOfType(GL_BYTE);
    }

    template<>
    void Push<int>(unsigned int count, unsigned int location) {
        m_Elements.push_back({GL_INT, count, location, false});
        m_Stride += count * glutls::GetSizeOfType(GL_INT);
    }

    inline std::vector<VertexBufferElement> GetElement() const { return m_Elements; }
    inline unsigned int GetStride() const { return m_Stride; }
};

#endif //GOME_VERTEXBUFFERLAYOUT_H
