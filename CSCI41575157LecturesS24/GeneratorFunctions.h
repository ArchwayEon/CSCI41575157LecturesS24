#pragma once

#include "GraphicsStructures.h"

VertexDataPCNT* CreateCubeVertexData();

VertexDataPCT* CreateXYPlanePCT(
	float width = 1.0f, float height = 1.0f,
	glm::vec3 color = { 1.0f, 1.0f, 1.0f },
	glm::vec2 tex = { 1.0f, 1.0f });

VertexDataPCNT* CreateXZPlanePCNT(
	float width, float depth,
	glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f },
	float repeatS = 1.0f, float repeatT = 1.0f);


