//
// Created by bar44 on 10/29/2020.
//

#ifndef GOME_INDEXBUFFER_H
#define GOME_INDEXBUFFER_H

#include "GL/glew.h"
#include "GLFW/glfw3.h"

class IndexBuffer {
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const void* data, unsigned int count, GLenum usage = GL_STATIC_DRAW);
	IndexBuffer();
	IndexBuffer(unsigned int rendererId);
	~IndexBuffer();

	void SetData(const void* data, unsigned int count, GLenum usage = GL_STATIC_DRAW);
	void Bind() const;
	void Unbind() const;
	unsigned int GetCount() const;
};



#endif //GOME_INDEXBUFFER_H
