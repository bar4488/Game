//
// Created by bar44 on 10/29/2020.
//

#ifndef GOME_RENDERER_H
#define GOME_RENDERER_H

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../Chunk.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Frustum.h"

#include <ft2build.h>
#include <freetype/freetype.h>  

class Renderer {
public:
	Renderer(int width, int height);
	~Renderer();

	void BeginDraw(glm::mat4 View, glm::vec3 cameraPos, glm::vec3 cameraDir);
	void EndDraw(glm::vec3 cameraDir);
	void DrawChunk(Chunk& chunk);
	void LoadProgram(std::string name, std::string vertexPath, std::string fragmentPath);
private:
    std::unordered_map<std::string, Shader*> m_ProgramsMap;
	int m_Width, m_Height;
	VertexArray m_SkyboxVAO;
	VertexBuffer m_SkyboxVB;
	IndexBuffer m_SkyboxIB;
	VertexArray m_CrosshairVAO;
	VertexBuffer m_CrosshairVB;
	IndexBuffer m_CrosshairIB;
	std::unordered_map<int, Texture*> m_Textures;
	glm::mat4 m_Projection;
	glm::mat4 m_View;
	glm::mat4 m_Model;
	Frustum m_Frustum;
};


#endif //GOME_RENDERER_H
