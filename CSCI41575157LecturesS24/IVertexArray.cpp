#include "IVertexArray.h"
#include <glad/glad.h> 
#include "GraphicsObject.h"

IVertexArray::IVertexArray() 
{
}

void IVertexArray::SetAsDynamicGraphicsObject(
	int maxNumberOfVertices, int maxNumberOfIndices)
{
	object->isDynamic = true;
	this->maxNumberOfVertices = maxNumberOfVertices;
	this->maxNumberOfIndices = maxNumberOfIndices;
}

void IVertexArray::Generate(IVertexDataParams& params)
{
	if (generator != nullptr) {
		generator->GenerateVertices(params);
	}
}

void IVertexArray::Generate()
{
	if (generator != nullptr) {
		generator->GenerateVertices();
	}
}

void IVertexArray::EnableAttribute(
	int attribIndex, int elementCount, int sizeInBytes, void* offset)
{
	glEnableVertexAttribArray(attribIndex);
	glVertexAttribPointer(
		attribIndex,
		elementCount,
		GL_FLOAT,
		GL_FALSE,
		sizeInBytes, // The number of bytes to the next element
		offset       // Byte offset of the first position in the array
	);
}


