#pragma once
#include "IVertexArray.h"
class PCNTVertexArray : public IVertexArray
{
protected:
public:
	PCNTVertexArray();
	~PCNTVertexArray() = default;

	void Render() override;

protected:
	unsigned int StaticAllocateVertexBuffer(
		std::shared_ptr<GraphicsObject> object) override;
};

