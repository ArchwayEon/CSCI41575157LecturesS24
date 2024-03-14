#pragma once
#include <memory>
#include "GraphicsStructures.h"
#include "Shader.h"

class IVertexArray
{
public:
	IVertexArray();
	virtual ~IVertexArray() = default;

	virtual void Render() = 0;

	virtual unsigned int StaticAllocateVertexBuffer(
		unsigned int vao, std::shared_ptr<GraphicsObject> object) = 0;

	virtual void EnableAttributes() = 0;

	virtual void SendObjectUniforms(
		std::shared_ptr<GraphicsObject> object, 
		std::shared_ptr<Shader> shader) = 0;

protected:
	void EnableAttribute(
		int attribIndex, int elementCount, int sizeInBytes, void* offset);
};

