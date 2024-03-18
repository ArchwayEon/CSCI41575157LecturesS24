#pragma once
#include "IVertexArray.h"
class PCIVertexArray :   public IVertexArray
{
protected:
	std::vector<VertexDataPC> vertexData;
	std::vector<glm::vec3> worldPositions;
	std::vector<glm::vec3> instanceColors;
	unsigned int wpVBO = 0;
	unsigned int icVBO = 0;
public:
	PCIVertexArray(std::vector<glm::vec3> worldPositions);
	~PCIVertexArray() = default;

	void SetWorldPositions(std::vector<glm::vec3> worldPositions) {
		this->worldPositions = worldPositions;
	}

	void SetInstanceColors(std::vector<glm::vec3> instanceColors) {
		this->instanceColors = instanceColors;
	}

	void RenderObject() override;

	unsigned int AllocateVertexBuffer(unsigned int vao) override;
	unsigned int AllocateIndexBuffer(unsigned int vao) override;

	void EnableAttributes() override;

	void SendObjectUniforms(std::shared_ptr<Shader> shader) override;

	std::size_t GetVertexSize() override {
		return sizeof(VertexDataPC);
	}

	void Generate(IVertexDataParams& params) override;
};

