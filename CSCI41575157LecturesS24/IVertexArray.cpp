#include "IVertexArray.h"
#include <glad/glad.h> 

IVertexArray::IVertexArray() 
{

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


