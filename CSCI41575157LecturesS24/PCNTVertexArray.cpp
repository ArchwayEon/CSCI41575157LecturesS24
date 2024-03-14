#include "PCNTVertexArray.h"
#include <glad/glad.h> 

PCNTVertexArray::PCNTVertexArray() : IVertexArray()
{
}

void PCNTVertexArray::RenderObject(std::shared_ptr<GraphicsObject> object)
{
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	EnableAttributes();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->textureId);
	glDrawArrays(object->primitive, 0, (int)object->numberOfVertices);
}

unsigned int PCNTVertexArray::AllocateVertexBuffer(
	unsigned int vao, std::shared_ptr<GraphicsObject> object)
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

void PCNTVertexArray::EnableAttributes()
{
	// Positions
	EnableAttribute(0, 3, sizeof(VertexDataPCNT), (void*)0);
	// Colors
	EnableAttribute(1, 4, sizeof(VertexDataPCNT), (void*)(sizeof(float) * 3));
	// Normals
	EnableAttribute(2, 3, sizeof(VertexDataPCNT), (void*)(sizeof(float) * 7));
	// Texture Coords
	EnableAttribute(3, 2, sizeof(VertexDataPCNT), (void*)(sizeof(float) * 10));
}

void PCNTVertexArray::SendObjectUniforms(std::shared_ptr<GraphicsObject> object, std::shared_ptr<Shader> shader)
{
	shader->SendMat4Uniform("world", object->referenceFrame);
	shader->SendFloatUniform(
		"materialAmbientIntensity", object->material.ambientIntensity);
	shader->SendFloatUniform(
		"materialSpecularIntensity", object->material.specularIntensity);
	shader->SendFloatUniform(
		"materialShininess", object->material.shininess);
}
