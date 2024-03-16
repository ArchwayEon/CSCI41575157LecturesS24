#pragma once
#include <memory>

class IVertexArray; // Forward declaration

class GraphicsObject {
public:
	std::shared_ptr<IVertexArray> vertexArray = nullptr;
	//VertexDataPCNT* vertexDataPCNT = nullptr;
	//VertexDataPCT* vertexDataPCT = nullptr;
	//std::vector<VertexDataPC> vertexDataPC;
	//std::vector<unsigned short> indexData;
	//std::size_t sizeOfVertexBuffer = 0;
	std::size_t maxSizeOfVertexBuffer = 0;
	//std::size_t sizeOfIndexBuffer = 0;
	std::size_t maxSizeOfIndexBuffer = 0;
	//std::size_t numberOfVertices = 0;
	//std::size_t numberOfIndices = 0;
	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int ibo = 0;
	bool isDynamic = false;
	bool isVisible = true;
	unsigned int shaderProgram = 0;
	unsigned int textureId = 0;
	unsigned int primitive = 0;
	glm::mat4 referenceFrame = glm::mat4(1.0f);
	Material material{};
};
