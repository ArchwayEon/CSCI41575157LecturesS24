#pragma once
#include <glm/glm.hpp>
#include "GeometricPlane.h"
#include <vector>

class Ray;

class BoundingBox
{
public:
	static const int FRONT = 0;
	static const int BACK = 1;
	static const int LEFT = 2;
	static const int RIGHT = 3;
	static const int TOP = 4;
	static const int BOTTOM = 5;

protected:
	float width = 1.0f, height = 1.0f, depth = 1.0f;
	glm::mat4 referenceFrame;
	glm::mat4 invReferenceFrame;
	GeometricPlane planes[6];
	std::vector<Intersection> intersections;
	glm::vec3 intersectionPoint{};

public:
	BoundingBox();

	void SetReferenceFrame(glm::mat4 referenceFrame) {
		this->referenceFrame = referenceFrame;
		invReferenceFrame = glm::inverse(this->referenceFrame);
	}

	const std::vector<Intersection>& GetIntersections() const {
		return intersections;
	}

	void Create(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
	
	bool IsIntersectingWithRay(const Ray& ray);
	const glm::vec3& GetIntersectionPoint() const {
		return intersectionPoint;
	}

	const Intersection& GetIntersection(int whichPlane) {
		return intersections[whichPlane];
	}
};

