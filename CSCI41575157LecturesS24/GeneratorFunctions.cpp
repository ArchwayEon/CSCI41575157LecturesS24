#include "GeneratorFunctions.h"

VertexDataPCNT* CreateCubeVertexData(void)
{
	// Front face
	VertexDataPCNT A = { {}, {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} };
	VertexDataPCNT B = { {}, {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} };
	VertexDataPCNT C = { {}, { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} };
	VertexDataPCNT D = { {}, { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} };
	// Right face
	VertexDataPCNT E = { {}, { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} };
	VertexDataPCNT F = { {}, { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} };
	VertexDataPCNT G = { {}, { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} };
	VertexDataPCNT H = { {}, { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} };
	// Back face
	VertexDataPCNT I = { {}, { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} };
	VertexDataPCNT J = { {}, { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} };
	VertexDataPCNT K = { {}, {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} };
	VertexDataPCNT L = { {}, {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} };
	// Left face
	VertexDataPCNT M = { {}, {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} };
	VertexDataPCNT N = { {}, {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} };
	VertexDataPCNT O = { {}, {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} };
	VertexDataPCNT P = { {}, {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} };
	// Top face
	VertexDataPCNT Q = { {}, {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} };
	VertexDataPCNT R = { {}, {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} };
	VertexDataPCNT S = { {}, { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} };
	VertexDataPCNT T = { {}, { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} };
	// Bottom face
	VertexDataPCNT U = { {}, { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} };
	VertexDataPCNT V = { {}, { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} };
	VertexDataPCNT W = { {}, {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} };
	VertexDataPCNT X = { {}, {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} };

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
	VertexDataPCT A = { {}, {-hw, hh, 0.0f}, color, {0.0f, tex.t} };
	VertexDataPCT B = { {}, {-hw,-hh, 0.0f}, color, {0.0f, 0.0f} };
	VertexDataPCT C = { {}, { hw,-hh, 0.0f}, color, {tex.s, 0.0f} };
	VertexDataPCT D = { {}, { hw, hh, 0.0f}, color, {tex.s, tex.t} };

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
	VertexDataPCNT A = { {}, {-x, 0.0f,-z}, color, normal, {0.0f, repeatT} };
	VertexDataPCNT B = { {}, {-x, 0.0f, z}, color, normal, {0.0f, 0.0f} };
	VertexDataPCNT C = { {}, { x, 0.0f, z}, color, normal, {repeatS, 0.0f} };
	VertexDataPCNT D = { {}, { x, 0.0f,-z}, color, normal, {repeatS, repeatT} };

	VertexDataPCNT* vertexData{ new VertexDataPCNT[6]{
		A, B, C, A, C, D
	} };
	return vertexData;
}

