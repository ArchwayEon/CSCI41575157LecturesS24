#include "PCTVertexArray.h"
#include <glad/glad.h> 
#include "GraphicsObject.h"

PCTVertexArray::PCTVertexArray() : IVertexArray()
{
}

void PCTVertexArray::RenderObject()
{
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	EnableAttributes();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->textureId);
	glDrawArrays(object->primitive, 0, (int)object->numberOfVertices);
}

unsigned int PCTVertexArray::AllocateVertexBuffer(unsigned int vao)
{
	glBindVertexArray(vao);
	glGenBuffers(1, &object->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertexData.size() * sizeof(VertexDataPCT),
		vertexData.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vertexData.clear();
	glBindVertexArray(0);
	return object->vbo;
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

void PCTVertexArray::SendObjectUniforms(std::shared_ptr<Shader> shader)
{
	shader->SendMat4Uniform("world", object->referenceFrame);
}

void PCTVertexArray::Generate(IVertexDataParams& params)
{
	IVertexArray::Generate(params);
	auto& data = reinterpret_cast<std::vector<VertexDataPCT>&>(
		generator->GetVertexData());
	vertexData = data;
	data.clear();
}
