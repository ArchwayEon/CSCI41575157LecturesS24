#include "Renderer.h"
#include <glad/glad.h>
#include "GraphicsObject.h"

Renderer::Renderer() :
	shader(0), vertexSize(0), vao(0)
{
	glGenVertexArrays(1, &vao);
}

void Renderer::AddObject(std::shared_ptr<GraphicsObject> object)
{
	object->shaderProgram = shader->GetShaderProgram();
	object->vertexArray->AllocateVertexBuffer(vao);
	object->vertexArray->AllocateIndexBuffer(vao);
	objectMap.push_back(object);
}

void Renderer::Select()
{
	glBindVertexArray(vao);
	glUseProgram(shader->GetShaderProgram());
}

void Renderer::Send(const std::string& uniformName, const glm::mat4& mat)
{
	shader->SendMat4Uniform(uniformName, mat);
}

void Renderer::Send(const std::string& uniformName, const glm::vec3& vec)
{
	shader->SendVec3Uniform(uniformName, vec);
}

void Renderer::Send(const std::string& uniformName, float value)
{
	shader->SendFloatUniform(uniformName, value);
}

void Renderer::Render()
{
	for (const auto& object : objectMap) {
		if (object->isVisible) {
			object->vertexArray->SendObjectUniforms(shader);
			object->vertexArray->RenderObject();
		}
	}
}

void Renderer::SetObjectsVisibility(bool visibility)
{
	for (const auto& object : objectMap) {
		object->isVisible = visibility;
	}
}
