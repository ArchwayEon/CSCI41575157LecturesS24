#pragma once
#include "IVertexArray.h"
class PCVertexArray :  public IVertexArray
{
protected:
	std::vector<VertexDataPC> vertexData;
public:
	PCVertexArray();
	~PCVertexArray() = default;

	void RenderObject() override;

	unsigned int AllocateVertexBuffer(unsigned int vao) override;
	unsigned int AllocateIndexBuffer(unsigned int vao) override;

	void EnableAttributes() override;

	void SendObjectUniforms(std::shared_ptr<Shader> shader) override;

	void SetAsDynamicGraphicsObject(
		std::shared_ptr<GraphicsObject> object,
		long long maxVertexCount) override;

	std::size_t GetVertexSize() override {
		return sizeof(VertexDataPC);
	}

	void Generate(IVertexDataParams& params) override;
};

