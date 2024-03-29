#include "ReferenceFrame.h"
#include <glm/gtc/matrix_transform.hpp>

void ReferenceFrame::PointAt(const glm::vec3& target)
{
	glm::vec3 position = matrix4[3];
	glm::vec3 zAxis = glm::normalize(target - position);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 xAxis = glm::normalize(glm::cross(up, zAxis));
	glm::vec3 yAxis = glm::cross(zAxis, xAxis);
	matrix4[0] = glm::vec4(xAxis, 0.0f);
	matrix4[1] = glm::vec4(yAxis, 0.0f);
	matrix4[2] = glm::vec4(zAxis, 0.0f);
}

void ReferenceFrame::SetPosition(glm::vec3 position)
{
	matrix4[3].x = position.x;
	matrix4[3].y = position.y;
	matrix4[3].z = position.z;
}

void ReferenceFrame::RotateX(float deltaDegrees)
{
	matrix4 = glm::rotate(
		matrix4, glm::radians(deltaDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
}

void ReferenceFrame::RotateY(float deltaDegrees)
{
	matrix4 = glm::rotate(
		matrix4, glm::radians(deltaDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
}

void ReferenceFrame::RotateZ(float deltaDegrees)
{
	matrix4 = glm::rotate(
		matrix4, glm::radians(deltaDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
}
