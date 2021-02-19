#pragma once

#include "glm/glm.hpp"

class Frustum
{
public:
	Frustum(glm::mat4 projection);
	Frustum();
	bool CheckPoint(glm::vec3 position);
	bool CheckRect(glm::vec3 bottomPoint, float width, float height);
	bool CheckSphere(glm::vec3 center, float radius);
private:
	glm::vec4 m_Planes[6];
};

