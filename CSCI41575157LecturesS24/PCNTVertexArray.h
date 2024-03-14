#pragma once
#include "IVertexArray.h"
class PCNTVertexArray : public IVertexArray
{
protected:
public:
	PCNTVertexArray();
	~PCNTVertexArray() = default;

	void Render() override;

	unsigned int StaticAllocateVertexBuffer(
		unsigned int vao, std::shared_ptr<GraphicsObject> object) override;

	void EnableAttributes() override;
};

