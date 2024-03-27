#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GeometricLine.h"
#include "GeometricPlane.h"
#include <vector>

class GraphicsObject;
class BoundingBox;

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

	void SetStart(glm::vec3 start) { rayStart = start; }
	void SetDirection(glm::vec3 dir) { rayDir = glm::normalize(dir); }

	const glm::vec3& GetStart() const { return rayStart; }
	const glm::vec3& GetDirection() const { return rayDir; }

	glm::vec3 GetPosition(float offset) const {
		return rayStart + (offset * rayDir);
	}

	Intersection GetIntersectionWithPlane(const GeometricPlane& plane) const;
	Intersection GetIntersectionWithBoundingBox(
		const BoundingBox& boundingBox) const;
	bool IsIntersectingObject(const GraphicsObject& object) const;
	//bool IsPointAlongRay(glm::vec3 point) const;
};

