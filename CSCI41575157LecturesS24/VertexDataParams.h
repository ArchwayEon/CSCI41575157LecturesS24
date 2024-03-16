#pragma once
#include <glm/glm.hpp>

struct IVertexDataParams {
};

struct CircleParams : public IVertexDataParams {
	float radius;
	glm::vec3 color;
	int steps;
};

struct SpirographParams : public IVertexDataParams {
	int steps = 10;
	float R = 4.0f;
	float l = 0.955f;
	float k = 0.55f;
	float revolutions = 20.0f;
	glm::vec3 color;
};

struct LinearBezierParams : public IVertexDataParams {
	int steps = 10;
	glm::vec3 color{};
	glm::vec3 p0{};
	glm::vec3 p1{};
};

struct QuadraticBezierParams : public IVertexDataParams {
	int steps = 10;
	glm::vec3 color{};
	glm::vec3 p0{};
	glm::vec3 p1{};
	glm::vec3 p2{};
};

struct QuadraticBezierMParams : public IVertexDataParams {
	int steps = 10;
	glm::vec3 color{};
	glm::mat3 pM{};
};

struct CubicBezierParams : public IVertexDataParams {
	int steps = 10;
	glm::vec3 color{};
	glm::vec3 p0{};
	glm::vec3 p1{};
	glm::vec3 p2{};
	glm::vec3 p3{};
};

struct CubicBezierMParams : public IVertexDataParams {
	int steps = 10;
	glm::vec3 color{};
	glm::mat4 pM{};
};

struct BezierPatchParams : public IVertexDataParams {
	int steps = 10;
	glm::vec3 color{};
	glm::vec3 cpBezier[4][4]{};
	int indexType = 1; // 1 = continous line, 2 = criss cross
};
