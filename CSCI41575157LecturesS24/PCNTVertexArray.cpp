#include "PCNTVertexArray.h"
#include <glad/glad.h> 
#include "GraphicsObject.h"
#include <vector>
#include <algorithm>

PCNTVertexArray::PCNTVertexArray() : IVertexArray()
{
}

void PCNTVertexArray::RenderObject()
{
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	EnableAttributes();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->textureId);
	glDrawArrays(object->primitive, 0, (int)object->numberOfVertices);
}

unsigned int PCNTVertexArray::AllocateVertexBuffer(unsigned int vao)
{
	glBindVertexArray(vao);
	glGenBuffers(1, &object->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertexData.size() * sizeof(VertexDataPCNT),
		vertexData.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vertexData.clear();
	glBindVertexArray(0);
	return object->vbo;
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

void PCNTVertexArray::SendObjectUniforms(std::shared_ptr<Shader> shader)
{
	shader->SendMat4Uniform("world", object->referenceFrame);
	shader->SendFloatUniform(
		"materialAmbientIntensity", object->material.ambientIntensity);
	shader->SendFloatUniform(
		"materialSpecularIntensity", object->material.specularIntensity);
	shader->SendFloatUniform(
		"materialShininess", object->material.shininess);
}

void PCNTVertexArray::Generate(IVertexDataParams& params)
{
	IVertexArray::Generate(params);
	auto& data = reinterpret_cast<std::vector<VertexDataPCNT>&>(
		generator->GetVertexData());
	vertexData = data;
	data.clear();
}
