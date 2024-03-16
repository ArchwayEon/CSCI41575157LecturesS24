#pragma once
#include "IVertexArray.h"
class PCTVertexArray :  public IVertexArray
{
protected:
	std::vector<VertexDataPCT> vertexData;
public:
	PCTVertexArray();
	~PCTVertexArray() = default;

	void RenderObject() override;

	unsigned int AllocateVertexBuffer(unsigned int vao) override;

	void EnableAttributes() override;

	void SendObjectUniforms(std::shared_ptr<Shader> shader) override;

	void Generate(IVertexDataParams& params) override;
};

