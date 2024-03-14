#include "Renderer.h"
#include <glad/glad.h> 

Renderer::Renderer(std::shared_ptr<IVertexArray> va) : 
	shader(0), vertexSize(0), vao(0)
{
	glGenVertexArrays(1, &vao);
	this->va = va;
}

void Renderer::AddObject(std::shared_ptr<GraphicsObject> object)
{
	object->shaderProgram = shader->GetShaderProgram();
	object->vbo = va->StaticAllocateVertexBuffer(vao, object);
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
		va->SendObjectUniforms(object, shader);
		glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
		va->EnableAttributes();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, object->textureId);
		glDrawArrays(object->primitive, 0, (int)object->numberOfVertices);
	}
}
