#pragma once
#include <string>
#include <sstream>
#include <unordered_map>
#include <glm/glm.hpp>
#include "BaseObject.h"

class Shader : public BaseObject
{
private:
	std::string vertexSource;
	std::string fragmentSource;
	unsigned int shaderProgram;
	std::unordered_map<std::string, unsigned int> uniformMap;

public:
	Shader(const std::string& vertexSource, const std::string& fragmentSource);
	Shader();
	~Shader() = default;

	inline const std::string& GetVertexSource() const { return vertexSource; }
	inline const std::string& GetFragmentSource() const { return fragmentSource; }
	inline unsigned int GetShaderProgram() const { return shaderProgram; }
	inline const bool IsCreated() const { return shaderProgram != 0; }

	void AddUniform(const std::string& uniformName);
	void SendMat4Uniform(const std::string& uniformName, const glm::mat4& mat);
	void SendIntUniform(const std::string& uniformName, int value);
	void SendVec3Uniform(const std::string& uniformName, const glm::vec3& vec);
	void SendFloatUniform(const std::string& uniformName, float value);

private:
	void SetDefaultSource();
	void init();

	unsigned int CompileShaderSource(int type, const std::string& shaderSource);
	void CreateShaderProgram();
};

