#include "PCVertexArray.h"
#include <glad/glad.h> 

PCVertexArray::PCVertexArray() : IVertexArray()
{
}

void PCVertexArray::RenderObject(std::shared_ptr<GraphicsObject> object)
{
	glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
	if (object->isDynamic) {
		glBufferSubData(
			GL_ARRAY_BUFFER, 0,
			object->sizeOfVertexBuffer,
			object->vertexDataPC.data());
	}
	EnableAttributes();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ibo);
	if (object->isDynamic) {
		glBufferSubData(
			GL_ELEMENT_ARRAY_BUFFER, 0,
			object->sizeOfIndexBuffer,
			object->indexData.data());
	}
	glDrawElements(
		object->primitive, 
		(int)object->indexData.size(), GL_UNSIGNED_SHORT, nullptr);
}

unsigned int PCVertexArray::AllocateVertexBuffer(
	unsigned int vao, std::shared_ptr<GraphicsObject> object)
{
	glBindVertexArray(vao);
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (object->isDynamic == false) {
		glBufferData(
			GL_ARRAY_BUFFER, object->sizeOfVertexBuffer,
			object->vertexDataPC.data(), GL_STATIC_DRAW);
	}
	else {
		glBufferData(
			GL_ARRAY_BUFFER, object->maxSizeOfVertexBuffer,
			nullptr, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	object->vertexDataPC.clear();
	glBindVertexArray(0);
	return vbo;
}

unsigned int PCVertexArray::AllocateIndexBuffer(
	unsigned int vao, std::shared_ptr<GraphicsObject> object)
{
	glBindVertexArray(vao);
	glGenBuffers(1, &object->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ibo);
	if (object->isDynamic == false) {
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, object->sizeOfIndexBuffer,
			object->indexData.data(), GL_STATIC_DRAW);
	}
	else {
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, object->maxSizeOfIndexBuffer,
			nullptr, GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	object->indexData.clear();
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

void PCVertexArray::SendObjectUniforms(
	std::shared_ptr<GraphicsObject> object, std::shared_ptr<Shader> shader)
{
	shader->SendMat4Uniform("world", object->referenceFrame);
}

void PCVertexArray::SetUpDynamicGraphicsObject(
	std::shared_ptr<GraphicsObject> object, 
	PCData& pcData, std::size_t maxVertexCount)
{
	object->isDynamic = true;
	object->vertexDataPC = pcData.vertexData;
	object->indexData = pcData.indexData;
	object->sizeOfVertexBuffer =
		object->vertexDataPC.size() * sizeof(VertexDataPC);
	object->numberOfVertices = object->vertexDataPC.size();
	object->sizeOfIndexBuffer =
		object->indexData.size() * sizeof(unsigned short);
	object->numberOfIndices = object->indexData.size();
	object->maxSizeOfVertexBuffer = maxVertexCount * sizeof(VertexDataPC);
	object->maxSizeOfIndexBuffer =
		object->maxSizeOfVertexBuffer * 2 * sizeof(unsigned short);
}

void PCVertexArray::SetAsDynamicGraphicsObject(
	std::shared_ptr<GraphicsObject> object, std::size_t maxVertexCount)
{
	std::vector<VertexDataPC>& vertexData = 
		reinterpret_cast<std::vector<VertexDataPC>&>
			(object->vertexArray->GetVertexData());
	object->isDynamic = true;
	object->vertexDataPC = vertexData;
		//object->vertexArray->GetGenerator()->GetVertexData();
	object->indexData = object->vertexArray->GetIndexData();
	object->sizeOfVertexBuffer = object->vertexArray->GetVertexDataSize();
	object->numberOfVertices = object->vertexArray->GetNumberOfVertices();
	object->sizeOfIndexBuffer = object->vertexArray->GetIndexDataSize();
	object->numberOfIndices = object->vertexArray->GetNumberOfIndices();
	object->maxSizeOfVertexBuffer = 
		maxVertexCount * object->vertexArray->GetVertexSize();
	object->maxSizeOfIndexBuffer =
		object->maxSizeOfVertexBuffer * 2 * object->vertexArray->GetIndexSize();
}

