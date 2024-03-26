#pragma once
#include <glm/glm.hpp>
#include "GeometricPlane.h"
#include <vector>

class Ray;

class BoundingBox
{
public:
	const int FRONT = 0;
	const int BACK = 1;
	const int LEFT = 2;
	const int RIGHT = 3;
	const int TOP = 4;
	const int BOTTOM = 5;

protected:
	float width = 1.0f, height = 1.0f, depth = 1.0f;
	glm::mat4 referenceFrame;
	glm::mat4 invReferenceFrame;
	GeometricPlane planes[6];

public:
	BoundingBox();

	void SetReferenceFrame(glm::mat4 referenceFrame) {
		this->referenceFrame = referenceFrame;
		invReferenceFrame = glm::inverse(this->referenceFrame);
	}

	void Create(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
	
	std::vector<Intersection> GetRayIntersections(const Ray& ray) const;
};

