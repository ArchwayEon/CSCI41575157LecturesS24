#pragma once
#include <glm/glm.hpp>

struct IVertexDataParams {
};

struct CircleParams : public IVertexDataParams {
	float radius;
	glm::vec3 color;
	int steps;
};
