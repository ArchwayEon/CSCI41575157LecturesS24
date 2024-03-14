#include "IVertexArray.h"
#include <glad/glad.h> 

IVertexArray::IVertexArray() : shaderProgram(0), vertexSize(0)
{
	glGenVertexArrays(1, &vao);
}

void IVertexArray::AddObject(std::shared_ptr<GraphicsObject> object)
{
	object->shaderProgram = this->shaderProgram;
	object->vbo = StaticAllocateVertexBuffer(object);
	objects.push_back(object);
}
