#include "GeometricLine.h"

GeometricLine::GeometricLine()
{
}

void GeometricLine::SetDirection(glm::vec3 start, glm::vec3 end)
{
	direction = glm::normalize(end - start);
	startPoint = start;
}

glm::vec3 GeometricLine::GetPosition(float offset) const
{
	return startPoint + (offset * direction);
}
