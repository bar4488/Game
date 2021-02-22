#include "Frustum.h"

glm::vec4 NormalizePlane(glm::vec4 plane) {
	float magnitude = 1 / glm::sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
	return {
		plane.x * magnitude,
		plane.y * magnitude,
		plane.z * magnitude,
		plane.w * magnitude,
	};
}

Frustum::Frustum(glm::mat4 projection)
{
	glm::vec3 n;
	//RIGHT
	m_Planes[0] = {
		projection[0][3] + projection[0][0],
		projection[1][3] + projection[1][0],
		projection[2][3] + projection[2][0],
		projection[3][3] + projection[3][0],
	};
	//LEFT
	m_Planes[1] = {
		projection[0][3] - projection[0][0],
		projection[1][3] - projection[1][0],
		projection[2][3] - projection[2][0],
		projection[3][3] - projection[3][0],
	};
	//TOP
	m_Planes[2] = {
		projection[0][3] - projection[0][1],
		projection[1][3] - projection[1][1],
		projection[2][3] - projection[2][1],
		projection[3][3] - projection[3][1],
	};
	//BOTTOM
	m_Planes[3] = {
		projection[0][3] + projection[0][1],
		projection[1][3] + projection[1][1],
		projection[2][3] + projection[2][1],
		projection[3][3] + projection[3][1],
	};
	//NEAR
	m_Planes[4] = {
		projection[0][3] + projection[0][2],
		projection[1][3] + projection[1][2],
		projection[2][3] + projection[2][2],
		projection[3][3] + projection[3][2],
	};
	//FAR
	m_Planes[5] = {
		projection[0][3] - projection[0][2],
		projection[1][3] - projection[1][2],
		projection[2][3] - projection[2][2],
		projection[3][3] - projection[3][2],
	};
	m_Planes[0] = NormalizePlane(m_Planes[0]);
	m_Planes[1] = NormalizePlane(m_Planes[1]);
	m_Planes[2] = NormalizePlane(m_Planes[2]);
	m_Planes[3] = NormalizePlane(m_Planes[3]);
	m_Planes[4] = NormalizePlane(m_Planes[4]);
	m_Planes[5] = NormalizePlane(m_Planes[5]);
}

Frustum::Frustum()
{
}

bool Frustum::CheckPoint(glm::vec3 position)
{
    for (char i = 0; i < 6; i++) {
        if (glm::dot(m_Planes[i], glm::vec4(position, 1.0f)) < 0.0f) {
            return false;
        }
    }
    return true;
}

bool Frustum::CheckRect(glm::vec3 bottomPoint, float width, float height)
{
	glm::vec3 center = bottomPoint + glm::vec3(width / 2.0, height / 2.0, width / 2.0);
	// TODO: CAN BE OPTIMIZED!
	// Check if any one point of the cube is in the view frustum.
	for (char i = 0; i < 6; i++)
	{
		if (glm::dot(m_Planes[i], glm::vec4((center.x - width / 2.0), (center.y - height / 2.0), (center.z - width / 2.0), 1.0f)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(m_Planes[i], glm::vec4((center.x + width / 2.0), (center.y - height / 2.0), (center.z - width / 2.0), 1.0f)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(m_Planes[i], glm::vec4((center.x - width / 2.0), (center.y + height / 2.0), (center.z - width / 2.0), 1.0f)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(m_Planes[i], glm::vec4((center.x + width / 2.0), (center.y + height / 2.0), (center.z - width / 2.0), 1.0f)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(m_Planes[i], glm::vec4((center.x - width / 2.0), (center.y - height / 2.0), (center.z + width / 2.0), 1.0f)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(m_Planes[i], glm::vec4((center.x + width / 2.0), (center.y - height / 2.0), (center.z + width / 2.0), 1.0f)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(m_Planes[i], glm::vec4((center.x - width / 2.0), (center.y + height / 2.0), (center.z + width / 2.0), 1.0f)) >= 0.0f)
		{
			continue;
		}

		if (glm::dot(m_Planes[i], glm::vec4((center.x + width / 2.0), (center.y + height / 2.0), (center.z + width / 2.0), 1.0f)) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}
bool Frustum::CheckSphere(glm::vec3 center, float radius)
{
    for (char i = 0; i < 6; i++) {
        if (glm::dot(m_Planes[i], glm::vec4(center, 1.0f)) + radius < 0.0f) {
            return false;
        }
    }
    return true;
}
