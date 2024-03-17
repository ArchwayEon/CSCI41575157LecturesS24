#pragma once
#include <glm/glm.hpp>

class GeometricLine;

// Vince, J. (2014). Mathematics for Computer Graphics, Springer

struct Intersection{
	bool isIntersecting = false;
	glm::vec3 point{};
};

class GeometricPlane
{
protected:
	glm::vec3 normal;
	float distanceFromOrigin;

public:
	GeometricPlane();
	~GeometricPlane() = default;

	void SetNormal(glm::vec3 normal) { this->normal = normal; }
	void SetDistanceFromOrigin(float distance){ 
		distanceFromOrigin = fabs(distance);
	}

	Intersection GetIntersectionWithLine(const GeometricLine& line) const;
};

