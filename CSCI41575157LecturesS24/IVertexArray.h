#pragma once
#include <cstddef>
#include <memory>
#include <vector>
#include "GraphicsStructures.h"

class IVertexArray
{
protected:
	unsigned int vao;
	unsigned int shaderProgram;
	std::size_t vertexSize;
	std::vector<std::shared_ptr<GraphicsObject>> objects;

public:
	IVertexArray();
	virtual ~IVertexArray() = default;

	void SetShaderProgram(unsigned int sp) { shaderProgram = sp; }
	unsigned int GetShaderProgram() const { return shaderProgram; }

	void SetVertexSize(std::size_t vs) { vertexSize = vs; }
	std::size_t GetVertexSize() const { return vertexSize; }

	void AddObject(std::shared_ptr<GraphicsObject> object);

	virtual void Render() = 0;

protected:
	virtual unsigned int StaticAllocateVertexBuffer(
		std::shared_ptr<GraphicsObject> object) = 0;
};

