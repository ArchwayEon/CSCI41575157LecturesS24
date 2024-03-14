#pragma once
#include <memory>
#include "GraphicsStructures.h"
#include "Shader.h"

class IVertexArray
{
protected:

public:
	IVertexArray();
	virtual ~IVertexArray() = default;

	virtual void RenderObject(std::shared_ptr<GraphicsObject> object) = 0;

	virtual unsigned int AllocateVertexBuffer(
		unsigned int vao, std::shared_ptr<GraphicsObject> object) = 0;
	virtual unsigned int AllocateIndexBuffer(
		unsigned int vao, std::shared_ptr<GraphicsObject> object) {
		return 0;
	};

	virtual void EnableAttributes() = 0;

	virtual void SendObjectUniforms(
		std::shared_ptr<GraphicsObject> object, std::shared_ptr<Shader> shader) = 0;

	virtual void SetUpDynamicGraphicsObject(
		std::shared_ptr<GraphicsObject> object, PCData& pcData, std::size_t maxVertexCount) {};

protected:
	void EnableAttribute(
		int attribIndex, int elementCount, int sizeInBytes, void* offset);
};

