#include "VertexGenerators.h"

void IVertexGenerator::GenerateLinesIndexDataConnected()
{
	indexData.clear();
	unsigned short nextIndex;
	unsigned short vertexCount = GetNumberOfVertices();
	for (unsigned short index = 0; index < vertexCount; index++) {
		indexData.push_back(index);
		nextIndex = (index + 1) % static_cast<unsigned short>(vertexCount);
		indexData.push_back(nextIndex);
	}
}

void PCCircleGenerator::GenerateVertices(IVertexDataParams& params)
{
	CircleParams& cp = reinterpret_cast<CircleParams&>(params);
	vertexData.clear();
	float x, y, thetaRadians;
	for (float theta = 0; theta < 360; theta += cp.steps) {
		thetaRadians = glm::radians(theta);
		x = cp.radius * cosf(thetaRadians);
		y = cp.radius * sinf(thetaRadians);
		vertexData.push_back({ {}, {x, y, 0.0f}, cp.color });
	}
	GenerateIndices();
}

void PCCircleGenerator::GenerateIndices()
{
	GenerateLinesIndexDataConnected();
}


