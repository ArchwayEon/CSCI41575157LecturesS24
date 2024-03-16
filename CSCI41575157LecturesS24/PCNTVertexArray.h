#pragma once
#include "IVertexArray.h"
#include "Shader.h"
class PCNTVertexArray : public IVertexArray
{
protected:
	std::vector<VertexDataPCNT> vertexData;
public:
	PCNTVertexArray();
	~PCNTVertexArray() = default;

	void RenderObject() override;

	unsigned int AllocateVertexBuffer(unsigned int vao) override;

	void EnableAttributes() override;

	void SendObjectUniforms(std::shared_ptr<Shader> shader) override;

	void Generate(IVertexDataParams& params) override;
};

