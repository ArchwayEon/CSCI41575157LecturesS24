#include "PCTVertexArray.h"
#include <glad/glad.h> 

PCTVertexArray::PCTVertexArray() : IVertexArray()
{
}

void PCTVertexArray::RenderObject(std::shared_ptr<GraphicsObject> object)
{
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	EnableAttributes();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->textureId);
	glDrawArrays(object->primitive, 0, (int)object->numberOfVertices);
}

unsigned int PCTVertexArray::AllocateVertexBuffer(
	unsigned int vao, std::shared_ptr<GraphicsObject> object)
{
	glBindVertexArray(vao);
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		object->sizeOfVertexBuffer, object->vertexDataPCT,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] object->vertexDataPCT;
	object->vertexDataPCT = nullptr;
	glBindVertexArray(0);
	return vbo;
}

void PCTVertexArray::EnableAttributes()
{
	// Positions
	EnableAttribute(0, 3, sizeof(VertexDataPCT), (void*)0);
	// Colors
	EnableAttribute(1, 3, sizeof(VertexDataPCT), (void*)(sizeof(float) * 3));
	// Texture Coords
	EnableAttribute(2, 2, sizeof(VertexDataPCT), (void*)(sizeof(float) * 6));
}

void PCTVertexArray::SendObjectUniforms(std::shared_ptr<GraphicsObject> object, std::shared_ptr<Shader> shader)
{
	shader->SendMat4Uniform("world", object->referenceFrame);
}
