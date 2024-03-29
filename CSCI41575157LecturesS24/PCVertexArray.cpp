#include "PCVertexArray.h"
#include <glad/glad.h> 
#include "GraphicsObject.h"

PCVertexArray::PCVertexArray() : IVertexArray()
{
}

void PCVertexArray::RenderObject()
{
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	if (object->isDynamic) {
		glBufferSubData(
			GL_ARRAY_BUFFER, 0,
			vertexData.size() * sizeof(VertexDataPC),
			vertexData.data());
	}
	EnableAttributes();
	int indexDataSize = static_cast<int>(GetIndexDataSize());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ibo);
	if (object->isDynamic) {
		glBufferSubData(
			GL_ELEMENT_ARRAY_BUFFER, 0,
			indexDataSize,
			indexData.data());
	}
	glDrawElements(
		object->primitive, 
		indexDataSize, GL_UNSIGNED_SHORT, nullptr);
}

unsigned int PCVertexArray::AllocateVertexBuffer(unsigned int vao)
{
	glBindVertexArray(vao);
	glGenBuffers(1, &object->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	if (object->isDynamic == false) {
		glBufferData(
			GL_ARRAY_BUFFER, 
			vertexData.size() * sizeof(VertexDataPC),
			vertexData.data(), 
			GL_STATIC_DRAW);
		vertexData.clear();
	}
	else {
		glBufferData(
			GL_ARRAY_BUFFER, 
			GetMaxSizeOfVertices(sizeof(VertexDataPC)),
			nullptr, 
			GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return object->vbo;
}

unsigned int PCVertexArray::AllocateIndexBuffer(unsigned int vao)
{
	glBindVertexArray(vao);
	glGenBuffers(1, &object->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ibo);
	if (object->isDynamic == false) {
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, 
			GetIndexDataSize(),
			indexData.data(), 
			GL_STATIC_DRAW);
		indexData.clear();
	}
	else {
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, 
			GetMaxSizeOfIndices(),
			nullptr, 
			GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return object->ibo;
}

void PCVertexArray::EnableAttributes()
{
	// Positions
	EnableAttribute(0, 3, sizeof(VertexDataPC), (void*)0);
	// Colors
	EnableAttribute(1, 3, sizeof(VertexDataPC), (void*)(sizeof(float) * 3));
}

void PCVertexArray::SendObjectUniforms(std::shared_ptr<Shader> shader)
{
	shader->SendMat4Uniform("world", object->referenceFrame.GetMatrix());
}

void PCVertexArray::Generate(IVertexDataParams& params)
{
	IVertexArray::Generate(params);
	auto& data = reinterpret_cast<std::vector<VertexDataPC>&>(
		generator->GetVertexData());
	vertexData = data;
	indexData = generator->GetIndexData();
	data.clear();
	generator->GetIndexData().clear();
}

