#include "Generator.h"

VertexDataPCNT* CreateCubeVertexData(void)
{
	// Front face
	VertexDataPCNT A = { {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} };
	VertexDataPCNT B = { {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} };
	VertexDataPCNT C = { { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} };
	VertexDataPCNT D = { { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} };
	// Right face
	VertexDataPCNT E = { { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} };
	VertexDataPCNT F = { { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} };
	VertexDataPCNT G = { { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} };
	VertexDataPCNT H = { { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} };
	// Back face
	VertexDataPCNT I = { { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} };
	VertexDataPCNT J = { { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} };
	VertexDataPCNT K = { {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} };
	VertexDataPCNT L = { {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} };
	// Left face
	VertexDataPCNT M = { {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} };
	VertexDataPCNT N = { {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} };
	VertexDataPCNT O = { {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} };
	VertexDataPCNT P = { {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} };
	// Top face
	VertexDataPCNT Q = { {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} };
	VertexDataPCNT R = { {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} };
	VertexDataPCNT S = { { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} };
	VertexDataPCNT T = { { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} };
	// Bottom face
	VertexDataPCNT U = { { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} };
	VertexDataPCNT V = { { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} };
	VertexDataPCNT W = { {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} };
	VertexDataPCNT X = { {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} };

	// 3 vertex per triangle, 2 triangles per face, 6 faces
	// 3 * 2 * 6 = 36 vertices
	VertexDataPCNT* vertexData{ new VertexDataPCNT[36]{
		// Front face
		A, B, C, A, C, D,
		// Right face
		E, F, G, E, G, H,
		// Back face
		I, J, K, I, K, L,
		// Left face
		M, N, O, M, O, P,
		// Top face
		Q, R, S, Q, S, T,
		// Bottom face
		U, V, W, U, W, X
	} };
	return vertexData;
}

VertexDataPCT* CreateXYPlanePCT(
	float width, float height, glm::vec3 color, glm::vec2 tex)
{
	float hw = width / 2;
	float hh = height / 2;
	// Front face
	VertexDataPCT A = { {-hw, hh, 0.0f}, color, {0.0f, tex.t} };
	VertexDataPCT B = { {-hw,-hh, 0.0f}, color, {0.0f, 0.0f} };
	VertexDataPCT C = { { hw,-hh, 0.0f}, color, {tex.s, 0.0f} };
	VertexDataPCT D = { { hw, hh, 0.0f}, color, {tex.s, tex.t} };

	VertexDataPCT* vertexData{ new VertexDataPCT[6]{
		A, B, C, A, C, D
	} };
	return vertexData;
}

VertexDataPCNT* CreateXZPlanePCNT(
	float width, float depth, glm::vec4 color, float repeatS, float repeatT)
{
	float x = width / 2;
	float z = depth / 2;
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	// Front face
	VertexDataPCNT A = { {-x, 0.0f,-z}, color, normal, {0.0f, repeatT} };
	VertexDataPCNT B = { {-x, 0.0f, z}, color, normal, {0.0f, 0.0f} };
	VertexDataPCNT C = { { x, 0.0f, z}, color, normal, {repeatS, 0.0f} };
	VertexDataPCNT D = { { x, 0.0f,-z}, color, normal, {repeatS, repeatT} };

	VertexDataPCNT* vertexData{ new VertexDataPCNT[6]{
		A, B, C, A, C, D
	} };
	return vertexData;
}

void GenerateXYCirclePCVertexData(
	std::vector<VertexDataPC>& data, float radius, glm::vec3 color, int steps)
{
	data.clear();
	float x, y, thetaRadians;
	for (float theta = 0; theta < 360; theta += steps) {
		thetaRadians = glm::radians(theta);
		x = radius * cosf(thetaRadians);
		y = radius * sinf(thetaRadians);
		data.push_back({ {x, y, 0.0f}, color });
	}
}

void GenerateLinesIndexDataConnected(
	std::vector<unsigned short>& data, std::size_t vertexCount)
{
	data.clear();
	unsigned short nextIndex;
	for (unsigned short index = 0; index < vertexCount; index++) {
		data.push_back(index);
		nextIndex = (index + 1) % static_cast<unsigned short>(vertexCount);
		data.push_back(nextIndex);
	}
}

void GenerateLinesIndexDataUnconnected(
	std::vector<unsigned short>& data, std::size_t vertexCount)
{
	data.clear();
	unsigned short nextIndex;
	for (unsigned short index = 0; index < vertexCount - 1; index++) {
		data.push_back(index);
		nextIndex = index + 1;
		data.push_back(nextIndex);
	}
}

void GenerateLinesIndexDataForBezierSurface(
	std::vector<unsigned short>& data, std::size_t vertexCount, int steps)
{
	data.clear();
	unsigned short index = 0;
	unsigned short nextIndex;
	for (int col = 0; col < steps; col++) {
		for (int row = 0; row < steps - 1; row++) {
			index = (col * steps) + row;
			data.push_back(index);
			nextIndex = index + 1;
			data.push_back(nextIndex);
		}
	}
	for (int row = 0; row < steps; row++) {
		for (int col = 0; col < steps - 1; col++) {
			index = (col * steps) + row;
			data.push_back(index);
			nextIndex = (col + 1) * steps + row;
			data.push_back(nextIndex);
		}
	}
}

PCData CreateXYCirclePC(float radius, glm::vec3 color, int steps)
{
	PCData pcData{};
	GenerateXYCirclePCVertexData(pcData.vertexData, radius, color, steps);
	GenerateLinesIndexDataConnected(
		pcData.indexData, pcData.vertexData.size());
	return pcData;
}

void GenerateXYSpirographPCVertexData(
	std::vector<VertexDataPC>& data,
	float R, float l, float k, glm::vec3 color,
	float revolutions, int steps)
{
	data.clear();
	float x, y, thetaRadians;
	float totalDegrees = 360.0f * revolutions;
	if (k == 0.0f) k = .001f;
	float q = (1 - k) / k;
	for (float theta = 1; theta < totalDegrees; theta += steps) {
		thetaRadians = glm::radians(theta);
		x = R * (((1 - k) * cosf(thetaRadians)) + (l * k * cosf(q * thetaRadians)));
		y = R * (((1 - k) * sinf(thetaRadians)) - (l * k * sinf(q * thetaRadians)));
		data.push_back({ {x, y, 0.0f}, color });
	}
}

PCData CreateXYSpirographPC(
	float R, float l, float k, glm::vec3 color, float revolutions, int steps)
{
	PCData pcData{};
	GenerateXYSpirographPCVertexData(
		pcData.vertexData, R, l, k, color, revolutions, steps);
	GenerateLinesIndexDataUnconnected(
		pcData.indexData, pcData.vertexData.size());
	return pcData;
}


void GenerateLinearBezierPC(
	std::vector<VertexDataPC>& data,
	glm::vec3 p0, glm::vec3 p1, glm::vec3 color, int steps)
{
	data.clear();
	glm::vec3 l{};
	float tick = 1.0f / steps;
	for (float t = 0; t <= 1; t += tick) {
		float coef = 1 - t;
		l = coef * p0 + t * p1;
		data.push_back({ {l.x, l.y, l.z}, color });
	}
}

PCData CreateLinearBezierPC(
	glm::vec3 p0, glm::vec3 p1, glm::vec3 color, int steps)
{
	PCData pcData{};
	GenerateLinearBezierPC(pcData.vertexData, p0, p1, color, steps);
	GenerateLinesIndexDataUnconnected(
		pcData.indexData, pcData.vertexData.size());
	return pcData;
}

void GenerateQuadraticBezierPC(
	std::vector<VertexDataPC>& data,
	glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 color, int steps)
{
	data.clear();
	glm::vec3 q{};
	float tick = 1.0f / steps;
	for (float t = 0; t <= 1; t += tick) {
		float coef = 1 - t;
		float coefSq = coef * coef;
		q = (coefSq * p0) + (2 * coef * t * p1) + (t * t * p2);
		data.push_back({ {q.x, q.y, q.z}, color });
	}
}

PCData CreateQuadraticBezierPC(
	glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 color, int steps)
{
	PCData pcData{};
	GenerateQuadraticBezierPC(pcData.vertexData, p0, p1, p2, color, steps);
	GenerateLinesIndexDataUnconnected(
		pcData.indexData, pcData.vertexData.size());
	return pcData;
}

