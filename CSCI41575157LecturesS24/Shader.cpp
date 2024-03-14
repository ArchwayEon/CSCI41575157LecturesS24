#include "Shader.h"
#include <glad/glad.h> 
#include <vector>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
{
	this->vertexSource = vertexSource;
	this->fragmentSource = fragmentSource;
	init();
	CreateShaderProgram();
}

Shader::Shader()
{
	SetDefaultSource();
	init();
	CreateShaderProgram();
}

void Shader::AddUniform(const std::string& uniformName)
{
	if (uniformMap.contains(uniformName)) return;
	unsigned int location = glGetUniformLocation(shaderProgram, uniformName.c_str());
	uniformMap[uniformName] = location;
}

void Shader::SendMat4Uniform(const std::string& uniformName, const glm::mat4& mat)
{
	if (uniformMap.contains(uniformName) == false) return;
	glUniformMatrix4fv(uniformMap[uniformName], 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SendIntUniform(const std::string& uniformName, int value)
{
	if (uniformMap.contains(uniformName) == false) return;
	glUniform1i(uniformMap[uniformName], value);
}

void Shader::SendVec3Uniform(
	const std::string& uniformName, const glm::vec3& vec)
{
	if (uniformMap.contains(uniformName) == false) return;
	glUniform3fv(uniformMap[uniformName], 1, glm::value_ptr(vec));
}

void Shader::SendFloatUniform(const std::string& uniformName, float value)
{
	if (uniformMap.contains(uniformName) == false) return;
	glUniform1f(uniformMap[uniformName], value);
}

void Shader::SetDefaultSource()
{
	vertexSource =
		"#version 430\n"
		"layout(location = 0) in vec3 position;\n"
		"layout(location = 1) in vec3 color;\n"
		"out vec4 fragColor;\n"
		"uniform mat4 world;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = projection * view * world * vec4(position, 1.0);\n"
		"   fragColor = vec4(color, 1.0);\n"
		"}\n";

	fragmentSource =
		"#version 430\n"
		"in vec4 fragColor;\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"   color = fragColor;\n"
		"}\n";
}

void Shader::init()
{
	this->shaderProgram = 0;
}

unsigned int Shader::CompileShaderSource(int type, const std::string& shaderSource)
{
	unsigned shaderId = glCreateShader(type);

	// Send the vertex shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	const char* source = (const char*)shaderSource.c_str();
	glShaderSource(shaderId, 1, &source, 0);

	glCompileShader(shaderId);

	int isCompiled = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		int maxLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<char> infoLog(maxLength);
		glGetShaderInfoLog(shaderId, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(shaderId);

		Log(infoLog);
		return -1;
	}
	Log("Successfully compiled the shader!");
	return shaderId;
}

void Shader::CreateShaderProgram()
{
	unsigned int vertexShader =
		CompileShaderSource(GL_VERTEX_SHADER, vertexSource);
	if (vertexShader == -1) return;

	unsigned int fragmentShader =
		CompileShaderSource(GL_FRAGMENT_SHADER, fragmentSource);
	if (fragmentShader == -1) return;

	// Time to link the shaders together into a program.

	shaderProgram = glCreateProgram();

	// Attach our shaders to our program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Link our program
	glLinkProgram(shaderProgram);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	int isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(shaderProgram);
		// Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		Log(infoLog);

		//std::copy(infoLog.begin(), infoLog.end(), std::ostream_iterator<char>(logger, ""));
	}

	// Always detach shaders after a successful link.
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	Log("Successfully created the shader!");
}
