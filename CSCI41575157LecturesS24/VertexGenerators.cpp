#include "VertexGenerators.h"

void IVertexGenerator::GenerateLinesIndexDataConnected()
{
	indexData.clear();
	unsigned short nextIndex;
	std::size_t vertexCount = GetNumberOfVertices();
	for (unsigned short index = 0; index < vertexCount; index++) {
		indexData.push_back(index);
		nextIndex = (index + 1) % static_cast<unsigned short>(vertexCount);
		indexData.push_back(nextIndex);
	}
}

void IVertexGenerator::GenerateLinesIndexDataUnconnected()
{
	indexData.clear();
	unsigned short nextIndex;
	std::size_t vertexCount = GetNumberOfVertices();
	for (unsigned short index = 0; index < vertexCount - 1; index++) {
		indexData.push_back(index);
		nextIndex = index + 1;
		indexData.push_back(nextIndex);
	}
}

void IVertexGenerator::GenerateLinesIndexDataForBezierSurface(int steps)
{
	indexData.clear();
	unsigned short index = 0;
	unsigned short nextIndex;
	for (int col = 0; col < steps; col++) {
		for (int row = 0; row < steps - 1; row++) {
			index = (col * steps) + row;
			indexData.push_back(index);
			nextIndex = index + 1;
			indexData.push_back(nextIndex);
		}
	}
	for (int row = 0; row < steps; row++) {
		for (int col = 0; col < steps - 1; col++) {
			index = (col * steps) + row;
			indexData.push_back(index);
			nextIndex = (col + 1) * steps + row;
			indexData.push_back(nextIndex);
		}
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
		VertexDataPC data = { {}, {x, y, 0.0f}, cp.color };
		vertexData.push_back(data);
	}
	GenerateIndices();
}

void PCCircleGenerator::GenerateIndices(int type, int steps)
{
	GenerateLinesIndexDataConnected();
}

void PCSpirographGenerator::GenerateVertices(IVertexDataParams& params)
{
	SpirographParams& sp = reinterpret_cast<SpirographParams&>(params);
	vertexData.clear();
	float x, y, thetaRadians;
	float totalDegrees = 360.0f * sp.revolutions;
	if (sp.k == 0.0f) sp.k = .001f;
	float q = (1 - sp.k) / sp.k;
	for (float theta = 1; theta < totalDegrees; theta += sp.steps) {
		thetaRadians = glm::radians(theta);
		x = sp.R * (((1 - sp.k) * cosf(thetaRadians)) + 
			(sp.l * sp.k * cosf(q * thetaRadians)));
		y = sp.R * (((1 - sp.k) * sinf(thetaRadians)) - 
			(sp.l * sp.k * sinf(q * thetaRadians)));
		vertexData.push_back({ {}, {x, y, 0.0f}, sp.color });
	}
	GenerateIndices();
}

void PCSpirographGenerator::GenerateIndices(int type, int steps)
{
	GenerateLinesIndexDataUnconnected();
}

void PCLinearBezierGenerator::GenerateVertices(IVertexDataParams& params)
{
	LinearBezierParams& lbp = reinterpret_cast<LinearBezierParams&>(params);
	vertexData.clear();
	glm::vec3 l{};
	float tick = 1.0f / lbp.steps;
	for (float t = 0; t <= 1; t += tick) {
		float coef = 1 - t;
		l = coef * lbp.p0 + t * lbp.p1;
		vertexData.push_back({ {}, {l.x, l.y, l.z}, lbp.color });
	}
	GenerateIndices();
}

void PCLinearBezierGenerator::GenerateIndices(int type, int steps)
{
	GenerateLinesIndexDataUnconnected();
}

void PCQuadraticBezierGenerator::GenerateVertices(IVertexDataParams& params)
{
	QuadraticBezierParams& qbp = 
		reinterpret_cast<QuadraticBezierParams&>(params);
	vertexData.clear();
	glm::vec3 q{};
	float tick = 1.0f / qbp.steps;
	for (float t = 0; t <= 1; t += tick) {
		float coef = 1 - t;
		float coefSq = coef * coef;
		q = (coefSq * qbp.p0) + (2 * coef * t * qbp.p1) + (t * t * qbp.p2);
		vertexData.push_back({ {}, {q.x, q.y, q.z}, qbp.color });
	}
	GenerateIndices();
}

void PCQuadraticBezierGenerator::GenerateIndices(int type, int steps)
{
	GenerateLinesIndexDataUnconnected();
}

void PCQuadraticBezierMGenerator::GenerateVertices(IVertexDataParams& params)
{
	QuadraticBezierMParams& qbp =
		reinterpret_cast<QuadraticBezierMParams&>(params);
	vertexData.clear();
	glm::mat3 CM{};
	CM[0] = glm::vec3(1, -2, 1);
	CM[1] = glm::vec3(-2, 2, 0);
	CM[2] = glm::vec3(1, 0, 0);
	glm::vec3 tv = { 0, 0, 1 };
	glm::vec3 q{};
	float tick = 1.0f / qbp.steps;
	for (float t = 0; t <= 1; t += tick) {
		tv[0] = t * t;
		tv[1] = t;
		float coef = 1 - t;
		float coefSq = coef * coef;
		q = qbp.pM * CM * tv;
		vertexData.push_back({ {}, {q.x, q.y, q.z}, qbp.color });
	}
	GenerateIndices();
}

void PCQuadraticBezierMGenerator::GenerateIndices(int type, int steps)
{
	GenerateLinesIndexDataUnconnected();
}

void PCCubicBezierGenerator::GenerateVertices(IVertexDataParams& params)
{
	CubicBezierParams& cbp =
		reinterpret_cast<CubicBezierParams&>(params);
	vertexData.clear();
	glm::vec3 c{};
	float tick = 1.0f / cbp.steps;
	for (float t = 0; t <= 1; t += tick) {
		float coef = 1 - t;
		float coef2 = coef * coef;
		float coef3 = coef * coef * coef;
		c = (coef3 * cbp.p0)
			+ (3 * coef2 * t * cbp.p1)
			+ (3 * coef * t * t * cbp.p2)
			+ (t * t * t * cbp.p3);
		vertexData.push_back({ {}, {c.x, c.y, c.z}, cbp.color });
	}
	GenerateIndices();
}

void PCCubicBezierGenerator::GenerateIndices(int type, int steps)
{
	GenerateLinesIndexDataUnconnected();
}

void PCCubicBezierMGenerator::GenerateVertices(IVertexDataParams& params)
{
	CubicBezierMParams& cbmp =
		reinterpret_cast<CubicBezierMParams&>(params);
	vertexData.clear();
	glm::mat4 CM{};
	CM[0] = glm::vec4(-1, 3, -3, 1);
	CM[1] = glm::vec4(3, -6, 3, 0);
	CM[2] = glm::vec4(-3, 3, 0, 0);
	CM[3] = glm::vec4(1, 0, 0, 0);
	glm::vec4 tv = { 0, 0, 0, 1 };
	glm::vec4 c{};
	float tick = 1.0f / cbmp.steps;
	for (float t = 0; t <= 1; t += tick) {
		tv[0] = t * t * t;
		tv[1] = t * t;
		tv[2] = t;
		c = cbmp.pM * CM * tv;
		vertexData.push_back({ {}, {c.x, c.y, c.z}, cbmp.color });
	}
	GenerateIndices();
}

void PCCubicBezierMGenerator::GenerateIndices(int type, int steps)
{
	GenerateLinesIndexDataUnconnected();
}

void PCBezierPatchGenerator::GenerateVertices(IVertexDataParams& params)
{
	BezierPatchParams& bpp =
		reinterpret_cast<BezierPatchParams&>(params);
	vertexData.clear();
	glm::mat4 CM{};
	CM[0] = glm::vec4(-1, 3, -3, 1);
	CM[1] = glm::vec4(3, -6, 3, 0);
	CM[2] = glm::vec4(-3, 3, 0, 0);
	CM[3] = glm::vec4(1, 0, 0, 0);
	glm::mat4 Px{}, Py{}, Pz{};
	for (auto row = 0; row < 4; row++) {
		for (auto col = 0; col < 4; col++) {
			Px[row][col] = bpp.cpBezier[row][col].x;
			Py[row][col] = bpp.cpBezier[row][col].y;
			Pz[row][col] = bpp.cpBezier[row][col].z;
		}
	}
	glm::vec4 sv = { 0, 0, 0, 1 };
	glm::vec4 tv = { 0, 0, 0, 1 };
	float x, y, z;
	float tick = 1.0f / bpp.steps;
	for (float s = 0; s <= 1; s += tick) {
		sv[0] = s * s * s;
		sv[1] = s * s;
		sv[2] = s;
		for (float t = 0; t <= 1; t += tick) {
			tv[0] = t * t * t;
			tv[1] = t * t;
			tv[2] = t;
			x = glm::dot(sv, CM * Px * CM * tv);
			y = glm::dot(sv, CM * Py * CM * tv);
			z = glm::dot(sv, CM * Pz * CM * tv);
			vertexData.push_back({ {}, { x, y, z }, bpp.color });
		}
	}
	GenerateIndices(bpp.indexType, bpp.steps);
}

void PCBezierPatchGenerator::GenerateIndices(int type, int steps)
{
	if (type == 1) GenerateLinesIndexDataUnconnected();
	else if (type == 2) GenerateLinesIndexDataForBezierSurface(steps);
}
