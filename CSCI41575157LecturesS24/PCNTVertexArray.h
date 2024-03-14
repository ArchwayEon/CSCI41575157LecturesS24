#pragma once
#include "IVertexArray.h"
#include "Shader.h"
class PCNTVertexArray : public IVertexArray
{
protected:
public:
	PCNTVertexArray();
	~PCNTVertexArray() = default;

	void RenderObject(std::shared_ptr<GraphicsObject> object) override;

	unsigned int AllocateVertexBuffer(
		unsigned int vao, std::shared_ptr<GraphicsObject> object) override;

	void EnableAttributes() override;

	void SendObjectUniforms(
		std::shared_ptr<GraphicsObject> object, 
		std::shared_ptr<Shader> shader) override;
};

