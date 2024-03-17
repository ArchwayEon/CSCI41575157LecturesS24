#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GeometricLine.h"
#include "GeometricPlane.h"
class Ray
{
protected:
	glm::vec3 rayStart{};
	glm::vec3 rayDir{};

public:
	Ray() = default;
	~Ray() = default;
	void Create(
		float screenX, float screenY, 
		const glm::mat4& proj, const glm::mat4& view);

	const glm::vec3& GetStart() const { return rayStart; }
	const glm::vec3& GetDirection() const { return rayDir; }

	Intersection GetIntersectionWithPlane(const GeometricPlane& plane) const;
};

