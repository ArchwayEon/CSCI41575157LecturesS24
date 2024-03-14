#pragma once
#include "IVertexArray.h"
#include <memory>
#include <vector>
#include "Shader.h"

class Renderer
{
protected:
	unsigned int vao;
	std::shared_ptr<Shader> shader;
	std::size_t vertexSize;

	std::shared_ptr<IVertexArray> va;
	std::vector<std::shared_ptr<GraphicsObject>> objectMap;

public:
	Renderer(std::shared_ptr<IVertexArray> va);

	void SetShaderProgram(std::shared_ptr<Shader> sp) { shader = sp; }
	std::shared_ptr<Shader> GetShaderProgram() const { return shader; }

	void SetVertexSize(std::size_t vs) { vertexSize = vs; }
	std::size_t GetVertexSize() const { return vertexSize; }

	void AddObject(std::shared_ptr<GraphicsObject> object);

	void Select();

	void Send(const std::string& uniformName, const glm::mat4& mat);
	void Send(const std::string& uniformName, const glm::vec3& vec);
	void Send(const std::string& uniformName, float value);

	void Render();
};

