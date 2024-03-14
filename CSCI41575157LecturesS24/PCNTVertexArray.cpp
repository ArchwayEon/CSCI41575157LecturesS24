#include "PCNTVertexArray.h"
#include <glad/glad.h> 

PCNTVertexArray::PCNTVertexArray() : IVertexArray()
{
}

void PCNTVertexArray::Render()
{
}

unsigned int PCNTVertexArray::StaticAllocateVertexBuffer(
	std::shared_ptr<GraphicsObject> object)
{
	glBindVertexArray(vao);
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, 
		object->sizeOfVertexBuffer, object->vertexDataPCNT, 
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] object->vertexDataPCNT;
	object->vertexDataPCNT = nullptr;
	glBindVertexArray(0);
	return vbo;
}
