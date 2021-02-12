//
// Created by bar44 on 11/1/2020.
//

#ifndef GOME_TEXTURE_H
#define GOME_TEXTURE_H

#include <string>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

class Texture {
private:
    unsigned int m_RendererID;
    std::string m_FilePath;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP;
public:
    Texture(const std::string& path);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    inline unsigned int GetWidth() { return m_Width; };
    inline unsigned int GetHeight() { return m_Height; };
};


#endif //GOME_TEXTURE_H
