#include "Ray.h"

void Ray::Create(
	float screenX, float screenY, 
	const glm::mat4& proj, const glm::mat4& view)
{
	glm::mat4 projInverse = glm::inverse(proj);
	glm::mat4 viewInverse = glm::inverse(view);

	glm::vec4 rayDirClip = glm::vec4(screenX, screenY, -1.0f, 1.0f);
	glm::vec4 rayDirE = projInverse * rayDirClip;
	rayDirE.z = -1.0f;
	rayDirE.w = 0.0f;
	glm::vec4 rayDirW = glm::normalize(viewInverse * rayDirE);
	rayDir = rayDirW;

	glm::vec4 rayStartS = glm::vec4(screenX, screenY, 1.0f, 1.0f);
	glm::vec4 rayStartE = projInverse * rayStartS;
	rayStartE.z = 1.0f;
	rayStartE.w = 0.0f;
	glm::vec4 rayStartW = viewInverse * rayStartE;
	rayStart = glm::vec3(rayStartW);
}

Intersection Ray::GetIntersectionWithPlane(const GeometricPlane& plane) const
{
	GeometricLine line;
	line.SetDirection(rayDir);
	return plane.GetIntersectionWithLine(line);
}
