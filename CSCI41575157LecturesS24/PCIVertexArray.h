#pragma once
#include "IVertexArray.h"
class PCIVertexArray :   public IVertexArray
{
protected:
	std::vector<VertexDataPC> vertexData;
	std::vector<glm::vec3> worldPositions;
	unsigned int instanceVBO = 0;
public:
	PCIVertexArray(std::vector<glm::vec3> worldPositions);
	~PCIVertexArray() = default;

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

