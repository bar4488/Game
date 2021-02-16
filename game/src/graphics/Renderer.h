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

#include "../utils/fonts/Font.h"
#include "../utils/fonts/FreeType.h"

class Renderer {
public:
	Renderer(int width, int height);
	~Renderer();

	void BeginDraw(glm::mat4 View, glm::vec3 cameraPos, glm::vec3 cameraDir);
	void EndDraw(glm::vec3 cameraDir);
	void DrawElements(VertexArray& vb, IndexBuffer& ib);
	void DrawText(std::string font, std::string text, float scale, glm::vec2 position, glm::vec3 color);
	void DrawText3D(std::string font, std::string text, float scale, glm::vec2 position, glm::vec3 color);
	void DrawLines(VertexArray& va, unsigned int first, unsigned int count, float width);

	void LoadProgram(std::string name, std::string vertexPath, std::string fragmentPath);
	void LoadTexture(std::string name, std::string texturePath);
	void LoadFont(std::string name, std::string font_path, unsigned int size);

	void BindTexture(std::string name, unsigned int slot);
	void EnableCap(unsigned int c);
	void DisableCap(unsigned int c);
	glm::mat4 GetOrthoProjection();
	glm::mat4 GetOrthoProjection(float size);
	Program* GetProgramByName(std::string name);
	Frustum& GetFrustum();
public:
	glm::mat4 m_ViewProjection;
	glm::mat4 m_Projection;
	glm::mat4 m_View;
	glm::mat4 m_NTView;
	glm::vec3 m_CameraPosition;
private:
    std::unordered_map<std::string, Program*> m_ProgramsMap;
	std::unordered_map<std::string, Texture*> m_Textures;
	std::unordered_map<std::string, Font*> m_Fonts;
	int m_Width, m_Height;
	VertexArray m_SkyboxVAO;
	VertexBuffer m_SkyboxVB;
	IndexBuffer m_SkyboxIB;
	VertexArray m_CrosshairVAO;
	VertexBuffer m_CrosshairVB;
	IndexBuffer m_CrosshairIB;
	Frustum m_Frustum;
	FreeType m_FreeType;
};
