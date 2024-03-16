#include "PCIVertexArray.h"
#include <glad/glad.h> 
#include "GraphicsObject.h"

PCIVertexArray::PCIVertexArray(std::vector<glm::vec3> worldPositions) : 
	IVertexArray()
{
	this->worldPositions = worldPositions;
}

void PCIVertexArray::RenderObject()
{
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	if (object->isDynamic) {
		glBufferSubData(
			GL_ARRAY_BUFFER, 0,
			vertexData.size() * sizeof(VertexDataPC),
			vertexData.data());
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	if (object->isDynamic) {
		glBufferSubData(
			GL_ARRAY_BUFFER, 0,
			worldPositions.size() * sizeof(glm::vec3) * object->instances,
			worldPositions.data());
	}

	int indexDataSize = static_cast<int>(GetIndexDataSize());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ibo);
	if (object->isDynamic) {
		glBufferSubData(
			GL_ELEMENT_ARRAY_BUFFER, 0,
			indexDataSize,
			indexData.data());
	}
	glDrawElementsInstanced(
		object->primitive,
		indexDataSize, 
		GL_UNSIGNED_SHORT, 
		nullptr,
		object->instances);
}

unsigned int PCIVertexArray::AllocateVertexBuffer(unsigned int vao)
{
	glBindVertexArray(vao);
	glGenBuffers(1, &object->vbo);
	glGenBuffers(1, &instanceVBO);
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
		// Positions
		EnableAttribute(0, 3, sizeof(VertexDataPC), (void*)0);
		// Colors
		EnableAttribute(1, 3, sizeof(VertexDataPC), (void*)(sizeof(float) * 3));
		
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(
			GL_ARRAY_BUFFER,
			worldPositions.size() * sizeof(glm::vec3) * object->instances,
			nullptr,
			GL_DYNAMIC_DRAW);
		// World Positions
		EnableAttribute(2, 3, sizeof(glm::vec3), (void*)0);
		glVertexAttribDivisor(2, 1);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return object->vbo;
}

unsigned int PCIVertexArray::AllocateIndexBuffer(unsigned int vao)
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

void PCIVertexArray::EnableAttributes()
{
	// Handled in RenderObject
}

void PCIVertexArray::SendObjectUniforms(std::shared_ptr<Shader> shader)
{
	shader->SendMat4Uniform("world", object->referenceFrame);
}

void PCIVertexArray::Generate(IVertexDataParams& params)
{
	IVertexArray::Generate(params);
	auto& data = reinterpret_cast<std::vector<VertexDataPC>&>(
		generator->GetVertexData());
	vertexData = data;
	indexData = generator->GetIndexData();
	data.clear();
	generator->GetIndexData().clear();
}