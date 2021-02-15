//
// Created by bar44 on 10/29/2020.
//
#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Program.h"
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
	void DrawElements(IndexBuffer& ib);
	void LoadProgram(std::string name, std::string vertexPath, std::string fragmentPath);
	void LoadTexture(std::string name, std::string texturePath);
	void BindTexture(std::string name, unsigned int slot);
	Program& GetProgramByName(std::string name);
	Frustum &GetFrustrum();
public:
	glm::mat4 m_ViewProjection;
	glm::mat4 m_Projection;
	glm::mat4 m_View;
	glm::vec3 m_CameraPosition;
private:
    std::unordered_map<std::string, Program*> m_ProgramsMap;
	std::unordered_map<std::string, Texture*> m_Textures;
	int m_Width, m_Height;
	VertexArray m_SkyboxVAO;
	VertexBuffer m_SkyboxVB;
	IndexBuffer m_SkyboxIB;
	VertexArray m_CrosshairVAO;
	VertexBuffer m_CrosshairVB;
	IndexBuffer m_CrosshairIB;
	Frustum m_Frustum;
};
