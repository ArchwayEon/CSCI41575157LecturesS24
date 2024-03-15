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

void GenerateXYCirclePCVertexData(
	std::vector<VertexDataPC>& data,
	float radius, glm::vec3 color, int steps);

void GenerateLinesIndexDataConnected(
	std::vector<unsigned short>& data, std::size_t vertexCount);

void GenerateLinesIndexDataUnconnected(
	std::vector<unsigned short>& data, std::size_t vertexCount);

void GenerateLinesIndexDataForBezierSurface(
	std::vector<unsigned short>& data, std::size_t vertexCount, int steps);

PCData CreateXYCirclePC(float radius, glm::vec3 color, int steps = 10);

void GenerateXYSpirographPCVertexData(
	std::vector<VertexDataPC>& data,
	float R, float l, float k, glm::vec3 color,
	float revolutions, int steps);

PCData CreateXYSpirographPC(
	float R, float l, float k, glm::vec3 color,
	float revolutions = 1, int steps = 10);

void GenerateLinearBezierPC(
	std::vector<VertexDataPC>& data,
	glm::vec3 p0, glm::vec3 p1, glm::vec3 color, int steps);

PCData CreateLinearBezierPC(
	glm::vec3 p0, glm::vec3 p1, glm::vec3 color, int steps = 10);

void GenerateQuadraticBezierPC(
	std::vector<VertexDataPC>& data,
	glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 color, int steps = 10);

PCData CreateQuadraticBezierPC(
	glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 color, int steps = 10);

void GenerateQuadraticBezierPCMat(
	std::vector<VertexDataPC>& data,
	const glm::mat3& pointMat, glm::vec3 color, int steps = 10);

PCData CreateQuadraticBezierPCMat(
	glm::mat3 pointMat, glm::vec3 color, int steps = 10);

void GenerateCubicBezierPC(
	std::vector<VertexDataPC>& data,
	glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
	glm::vec3 color, int steps);

PCData CreateCubicBezierPC(
	glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
	glm::vec3 color, int steps = 10);

void GenerateCubicBezierPCMat(
	std::vector<VertexDataPC>& data,
	const glm::mat4& pointMat, glm::vec3 color, int steps);

PCData CreateCubicBezierPCMat(
	glm::mat4 pointMat, glm::vec3 color, int steps = 10);

void GenerateBezierPatch(
	std::vector<VertexDataPC>& data,
	glm::vec3 cp[][4], glm::vec3 color, int steps);

PCData CreateBezierPatch(
	glm::vec3 points[][4], glm::vec3 color, int steps = 10);

PCData CreateBezierPatchCrissCross(
	glm::vec3 points[][4], glm::vec3 color, int steps = 10);
