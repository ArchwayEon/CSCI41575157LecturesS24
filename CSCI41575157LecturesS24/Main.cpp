#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GraphicsStructures.h"
#include "Generator.h"

// Eek! A global mouse!
MouseParams mouse;

static void OnWindowSizeChanged(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

static void OnMouseMove(GLFWwindow* window, double mouseX, double mouseY)
{
	mouse.x = mouseX;
	mouse.y = mouseY;

	float xPercent = static_cast<float>(mouse.x / mouse.windowWidth);
	float yPercent = static_cast<float>(mouse.y / mouse.windowHeight);

	mouse.spherical.theta = 90.0f - (xPercent * 180); // left/right
	mouse.spherical.phi = 180.0f - (yPercent * 180); // up/down
}

static void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	mouse.fieldOfView -= static_cast<float>(yoffset * 2);
	if (mouse.fieldOfView < 1.0f) mouse.fieldOfView = 1.0f;
	if (mouse.fieldOfView > 60.0f) mouse.fieldOfView = 60.0f;
}

static void ProcessInput(
	GLFWwindow* window, double elapsedSeconds, 
	glm::vec3& axis, glm::mat4& cameraFrame, bool& lookWithMouse)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		lookWithMouse = !lookWithMouse;
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		axis = glm::vec3(0.0f, 1.0f, 0.0f);
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		axis = glm::vec3(1.0f, 0.0f, 0.0f);
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		axis = glm::vec3(0.0f, 0.0f, 1.0f);
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		glm::vec3 forward = -cameraFrame[2];
		glm::vec3 position = cameraFrame[3];
		forward = forward * static_cast<float>(10.0f * elapsedSeconds);
		position = position + forward;
		cameraFrame[3] = glm::vec4(position, 1.0f);
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 toLeft = -cameraFrame[0];
		glm::vec3 position = cameraFrame[3];
		toLeft = toLeft * static_cast<float>(10.0f * elapsedSeconds);
		position = position + toLeft;
		cameraFrame[3] = glm::vec4(position, 1.0f);
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		glm::vec3 backward = cameraFrame[2];
		glm::vec3 position = cameraFrame[3];
		backward = backward * static_cast<float>(10.0f * elapsedSeconds);
		position = position + backward;
		cameraFrame[3] = glm::vec4(position, 1.0f);
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 toRight = cameraFrame[0];
		glm::vec3 position = cameraFrame[3];
		toRight = toRight * static_cast<float>(10.0f * elapsedSeconds);
		position = position + toRight;
		cameraFrame[3] = glm::vec4(position, 1.0f);
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		glm::vec3 yAxis = cameraFrame[1];
		float turnDelta = static_cast<float>(-90.0f * elapsedSeconds);
		cameraFrame = glm::rotate(cameraFrame, glm::radians(turnDelta), yAxis);
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		glm::vec3 yAxis = cameraFrame[1];
		float turnDelta = static_cast<float>(90.0f * elapsedSeconds);
		cameraFrame = glm::rotate(cameraFrame, glm::radians(turnDelta), yAxis);
		return;
	}
}

static void Log(std::stringstream& log, const std::vector<char>& message)
{
	std::copy(message.begin(), message.end(), std::ostream_iterator<char>(log, ""));
}

static unsigned int CompileShaderSource(
	const std::string& shaderSource, int type, std::stringstream& logger, Result& result)
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

		Log(logger, infoLog);
		result.isSuccess = false;
		result.message = logger.str();
		return -1;
	}
	result.isSuccess = true;
	result.message = "Success!";
	return shaderId;
}

static Result CreateShaderProgram(
	std::string& vertexSource, std::string& fragmentSource, unsigned int& program)
{
	std::stringstream logger;
	Result result;

	unsigned int vertexShader =
		CompileShaderSource(vertexSource, GL_VERTEX_SHADER, logger, result);
	if (result.isSuccess == false) return result;

	unsigned int fragmentShader =
		CompileShaderSource(fragmentSource, GL_FRAGMENT_SHADER, logger, result);
	if (result.isSuccess == false) return result;

	// Time to link the shaders together into a program.

	program = glCreateProgram();

	// Attach our shaders to our program
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	int isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		Log(logger, infoLog);
		result.isSuccess = false;
		result.message = logger.str();
		return result;
	}

	// Always detach shaders after a successful link.
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	result.isSuccess = true;
	result.message = "Successfully created the shader!";
	return result;
}

static glm::mat4 CreateViewMatrix(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up)
{
	glm::vec3 right = glm::cross(direction, up);
	right = glm::normalize(right);

	glm::vec3 vUp = glm::cross(right, direction);
	vUp = glm::normalize(vUp);

	glm::mat4 view(1.0f);
	view[0] = glm::vec4(right, 0.0f);
	view[1] = glm::vec4(up, 0.0f);
	view[2] = glm::vec4(direction, 0.0f);
	view[3] = glm::vec4(position, 1.0f);
	return glm::inverse(view);
}

static void EnableAttribute(int attribIndex, int elementCount, int sizeInBytes, void* offset)
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

static void EnablePCNTAttributes()
{
	// Positions
	EnableAttribute(0, 3, sizeof(VertexDataPCNT), (void*)0);
	// Colors
	EnableAttribute(1, 4, sizeof(VertexDataPCNT), (void*)(sizeof(float) * 3));
	// Normals
	EnableAttribute(2, 3, sizeof(VertexDataPCNT), (void*)(sizeof(float) * 7));
	// Texture Coords
	EnableAttribute(3, 2, sizeof(VertexDataPCNT), (void*)(sizeof(float) * 10));
}

static void EnablePCTAttributes()
{
	// Positions
	EnableAttribute(0, 3, sizeof(VertexDataPCT), (void*)0);
	// Colors
	EnableAttribute(1, 3, sizeof(VertexDataPCT), (void*)(sizeof(float) * 3));
	// Texture Coords
	EnableAttribute(2, 2, sizeof(VertexDataPCT), (void*)(sizeof(float) * 6));
}

static void EnablePCAttributes()
{
	// Positions
	EnableAttribute(0, 3, sizeof(VertexDataPC), (void*)0);
	// Colors
	EnableAttribute(1, 3, sizeof(VertexDataPC), (void*)(sizeof(float) * 3));
}

static void Trim(std::string& str)
{
	const std::string delimiters = " \f\n\r\t\v";
	str.erase(str.find_last_not_of(delimiters) + 1);
	str.erase(0, str.find_first_not_of(delimiters));
}

static std::string ReadFromFile(const std::string& filePath)
{
	std::stringstream ss;
	std::ifstream fin{};
	fin.open(filePath.c_str());
	if (fin.fail()) {
		ss << "Could not open: " << filePath << std::endl;
		return ss.str();
	}

	std::string line;
	while (!fin.eof()) {
		getline(fin, line);
		Trim(line);
		if (line != "") { // Skip blank lines
			ss << line << std::endl;
		}
	}
	fin.close();
	return ss.str();
}

static unsigned char* LoadTextureDataFromFile(
	const std::string& filePath, int& width, int& height, int& numChannels)
{
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data =
		stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);
	return data;
}

static unsigned int Create2DTexture(
	unsigned char* textureData, unsigned int width, unsigned int height)
{
	// Generate the texture id
	unsigned int textureId;
	glGenTextures(1, &textureId);
	// Select the texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	// Apply texture parameters 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// Send the texture to the GPU 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);
	return textureId;
}

static unsigned int CreateTextureFromFile(const std::string& filePath)
{
	int textureWidth, textureHeight, numChannels;
	unsigned char* textureData =
		LoadTextureDataFromFile(
			filePath, textureWidth, textureHeight, numChannels);
	unsigned int textureId =
		Create2DTexture(textureData, textureWidth, textureHeight);
	stbi_image_free(textureData);
	textureData = nullptr;
	return textureId;
}

void PointAt(glm::mat4& referenceFrame, const glm::vec3& point)
{
	glm::vec3 position = referenceFrame[3];
	glm::vec3 zAxis = glm::normalize(point - position);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 xAxis = glm::normalize(glm::cross(up, zAxis));
	glm::vec3 yAxis = glm::cross(zAxis, xAxis);
	referenceFrame[0] = glm::vec4(xAxis, 0.0f);
	referenceFrame[1] = glm::vec4(yAxis, 0.0f);
	referenceFrame[2] = glm::vec4(zAxis, 0.0f);
}

std::string GetOpenGLError()
{
	std::stringstream log;
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		log << err << std::endl;
	}
	return log.str();
}

static unsigned int AllocateVertexBufferPCNT(GraphicsObject& object)
{
	glBindVertexArray(object.vao);
	glGenBuffers(1, &object.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glBufferData(
		GL_ARRAY_BUFFER, object.sizeOfVertexBuffer, object.vertexDataPCNT, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] object.vertexDataPCNT;
	object.vertexDataPCNT = nullptr;
	glBindVertexArray(0);
	return object.vbo;
}

static unsigned int AllocateVertexBufferPCT(GraphicsObject& object)
{
	glBindVertexArray(object.vao);
	glGenBuffers(1, &object.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glBufferData(
		GL_ARRAY_BUFFER, object.sizeOfVertexBuffer, object.vertexDataPCT, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] object.vertexDataPCT;
	object.vertexDataPCT = nullptr;
	glBindVertexArray(0);
	return object.vbo;
}

static unsigned int AllocateVertexBufferPC(GraphicsObject& object)
{
	glBindVertexArray(object.vao);
	glGenBuffers(1, &object.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	if (object.isDynamic == false) {
		glBufferData(
			GL_ARRAY_BUFFER, object.sizeOfVertexBuffer, 
			object.vertexDataPC.data(), GL_STATIC_DRAW);
	}
	else {
		glBufferData(
			GL_ARRAY_BUFFER, object.maxSizeOfVertexBuffer, 
			nullptr, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	object.vertexDataPC.clear();
	glBindVertexArray(0);
	return object.vbo;
}

static unsigned int AllocateIndexBuffer(GraphicsObject& object)
{
	glBindVertexArray(object.vao);
	glGenBuffers(1, &object.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.ibo);
	if (object.isDynamic == false) {
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, object.sizeOfIndexBuffer,
			object.indexData.data(), GL_STATIC_DRAW);
	}
	else {
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, object.maxSizeOfIndexBuffer,
			nullptr, GL_STATIC_DRAW);
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	object.indexData.clear();
	glBindVertexArray(0);
	return object.ibo;
}

static void RenderObjectPCNT(
	GraphicsObject& object, LightingShaderLocation& location,
	glm::mat4& projection, glm::mat4& view,
	Light& globalLight, Light& localLight,
	glm::vec3& cameraPosition, unsigned int primitive)
{
	glUseProgram(object.shaderProgram);
	glUniformMatrix4fv(
		location.projectionLoc, 1, GL_FALSE,
		glm::value_ptr(projection));
	glUniformMatrix4fv(location.viewLoc, 1, GL_FALSE,
		glm::value_ptr(view));
	glUniform3fv(
		location.globalLightPositionLoc, 1,
		glm::value_ptr(globalLight.position));
	glUniform3fv(
		location.globalLightColorLoc, 1,
		glm::value_ptr(globalLight.color));
	glUniform1f(
		location.globalLightIntensityLoc,
		globalLight.intensity);
	glUniform3fv(
		location.localLightPositionLoc, 1,
		glm::value_ptr(localLight.position));
	glUniform3fv(
		location.localLightColorLoc, 1,
		glm::value_ptr(localLight.color));
	glUniform1f(
		location.localLightIntensityLoc,
		localLight.intensity);
	glUniform1f(
		location.localLightAttenuationLoc,
		localLight.attenuationCoef);
	glUniform3fv(
		location.viewPositionLoc, 1,
		glm::value_ptr(cameraPosition));

	glBindVertexArray(object.vao);
	glUniformMatrix4fv(
		location.worldLoc, 1, GL_FALSE,
		glm::value_ptr(object.referenceFrame));
	glUniform1f(
		location.materialAmbientLoc,
		object.material.ambientIntensity);
	glUniform1f(
		location.materialSpecularLoc,
		object.material.specularIntensity);
	glUniform1f(
		location.materialShininessLoc,
		object.material.shininess);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	EnablePCNTAttributes();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object.textureId);
	glDrawArrays(primitive, 0, (int)object.numberOfVertices);
}

static void RenderObjectPCT(
	GraphicsObject& object, BasicShaderLocation& location,
	glm::mat4& projection, glm::mat4& view, unsigned int primitive)
{
	glUseProgram(object.shaderProgram);
	glUniformMatrix4fv(
		location.projectionLoc, 1, GL_FALSE,
		glm::value_ptr(projection));
	glUniformMatrix4fv(location.viewLoc, 1, GL_FALSE,
		glm::value_ptr(view));

	glBindVertexArray(object.vao);
	glUniformMatrix4fv(
		location.worldLoc, 1, GL_FALSE,
		glm::value_ptr(object.referenceFrame));
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	EnablePCTAttributes();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object.textureId);
	glDrawArrays(primitive, 0, (int)object.numberOfVertices);
}

static void RenderObjectPC(
	GraphicsObject& object, BasicShaderLocation& location,
	glm::mat4& projection, glm::mat4& view, unsigned int primitive)
{
	glBindVertexArray(object.vao);
	glUseProgram(object.shaderProgram);
	glUniformMatrix4fv(
		location.projectionLoc, 1, GL_FALSE,
		glm::value_ptr(projection));
	glUniformMatrix4fv(location.viewLoc, 1, GL_FALSE,
		glm::value_ptr(view));

	glUniformMatrix4fv(
		location.worldLoc, 1, GL_FALSE,
		glm::value_ptr(object.referenceFrame));
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	if (object.isDynamic) {
		glBufferSubData(
			GL_ARRAY_BUFFER, 0, 
			object.sizeOfVertexBuffer, 
			object.vertexDataPC.data());
	}
	EnablePCAttributes();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.ibo);
	if (object.isDynamic) {
		glBufferSubData(
			GL_ELEMENT_ARRAY_BUFFER, 0,
			object.sizeOfIndexBuffer,
			object.indexData.data());
	}
	glDrawElements(
		primitive, (int)object.indexData.size(), GL_UNSIGNED_SHORT, nullptr);
}

void SetUpDynamicPCGraphicsObject(
	GraphicsObject& object, PCData& pcData,
	unsigned int vao, unsigned int shaderProgram, 
	std::size_t maxVertexCount)
{
	object.vertexDataPC = pcData.vertexData;
	object.indexData = pcData.indexData;
	object.sizeOfVertexBuffer =
		object.vertexDataPC.size() * sizeof(VertexDataPC);
	object.numberOfVertices = object.vertexDataPC.size();
	object.sizeOfIndexBuffer = 
		object.indexData.size() * sizeof(unsigned short);
	object.numberOfIndices = object.indexData.size();
	object.vao = vao;
	object.shaderProgram = shaderProgram;
	object.isDynamic = true;
	object.maxSizeOfVertexBuffer = maxVertexCount * sizeof(VertexDataPC);
	object.maxSizeOfIndexBuffer =
		object.maxSizeOfVertexBuffer * 2 * sizeof(unsigned short);
	object.vbo = AllocateVertexBufferPC(object);
	object.ibo = AllocateIndexBuffer(object);
 }

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(1200, 800, "ETSU Computing Interactive Graphics", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 1200, 800);
	glfwSetFramebufferSizeCallback(window, OnWindowSizeChanged);
	glfwSetCursorPosCallback(window, OnMouseMove);
	glfwSetScrollCallback(window, OnScroll);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMaximizeWindow(window);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);

	// Cull back faces and use counter-clockwise winding of front faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	std::string vertexSource1 = ReadFromFile("lighting.vert.glsl");
	std::string fragmentSource1 = ReadFromFile("lighting.frag.glsl");
	std::string vertexSource2 = ReadFromFile("basic.vert.glsl");
	std::string fragmentSource2 = ReadFromFile("basic.frag.glsl");
	std::string pcVertexSource = ReadFromFile("pc.vert.glsl");
	std::string pcFragmentSource = ReadFromFile("pc.frag.glsl");

	unsigned int lightingShaderProgram;
	Result result1 = CreateShaderProgram(vertexSource1, fragmentSource1, lightingShaderProgram);
	unsigned int textureShaderProgram;
	Result result2 = CreateShaderProgram(vertexSource2, fragmentSource2, textureShaderProgram);
	unsigned int pcShaderProgram;
	Result resultPC =
		CreateShaderProgram(pcVertexSource, pcFragmentSource, pcShaderProgram);
	std::string message = result1.message + result2.message + resultPC.message;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");

	// Get the uniform locations
	LightingShaderLocation lightingLocation;
	lightingLocation.projectionLoc =
		glGetUniformLocation(lightingShaderProgram, "projection");
	lightingLocation.viewLoc =
		glGetUniformLocation(lightingShaderProgram, "view");
	lightingLocation.worldLoc =
		glGetUniformLocation(lightingShaderProgram, "world");
	BasicShaderLocation textureLocation;
	textureLocation.projectionLoc =
		glGetUniformLocation(textureShaderProgram, "projection");
	textureLocation.viewLoc =
		glGetUniformLocation(textureShaderProgram, "view");
	textureLocation.worldLoc =
		glGetUniformLocation(textureShaderProgram, "world");
	BasicShaderLocation pcLocation;
	pcLocation.projectionLoc =
		glGetUniformLocation(pcShaderProgram, "projection");
	pcLocation.viewLoc =
		glGetUniformLocation(pcShaderProgram, "view");
	pcLocation.worldLoc =
		glGetUniformLocation(pcShaderProgram, "world");

	// Create the texture data
	unsigned char* textureData = new unsigned char[] {
		0, 0, 0, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 0, 255,
			0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 0, 255,
			0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 0, 255,
			0, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 0, 0, 0, 255
		};

	unsigned int customTextureId = Create2DTexture(textureData, 4, 4);
	delete[] textureData;
	textureData = nullptr;

	unsigned int lightingVAO;
	glGenVertexArrays(1, &lightingVAO);

	GraphicsObject cube;
	cube.vertexDataPCNT = CreateCubeVertexData();
	cube.sizeOfVertexBuffer = 36 * sizeof(VertexDataPCNT);
	cube.numberOfVertices = 36;
	cube.vao = lightingVAO;
	cube.shaderProgram = lightingShaderProgram;
	cube.vbo = AllocateVertexBufferPCNT(cube);
	cube.textureId = customTextureId;
	cube.material.ambientIntensity = 0.1f;
	cube.material.specularIntensity = 0.5f;
	cube.material.shininess = 16.0f;
	cube.referenceFrame[3] = glm::vec4(0.0f, 0.0f, -25.0f, 1.0f);

	GraphicsObject floor;
	floor.vertexDataPCNT =
		CreateXZPlanePCNT(50.0f, 50.0f, { 1.0f, 1.0f, 1.0f, 1.0f }, 5.0f, 5.0f);
	floor.sizeOfVertexBuffer = 6 * sizeof(VertexDataPCNT);
	floor.numberOfVertices = 6;
	floor.vao = lightingVAO;
	floor.shaderProgram = lightingShaderProgram;
	floor.vbo = AllocateVertexBufferPCNT(floor);
	floor.textureId = CreateTextureFromFile("stone-road-texture.jpg");
	floor.referenceFrame[3] = glm::vec4(0.0f, -5.0f, 0.0f, 1.0f);
	floor.material.ambientIntensity = 0.1f;
	floor.material.specularIntensity = 0.5f;
	floor.material.shininess = 16.0f;

	unsigned int basicTextureVAO;
	glGenVertexArrays(1, &basicTextureVAO);

	GraphicsObject lightBulb;
	lightBulb.vertexDataPCT = CreateXYPlanePCT();
	lightBulb.sizeOfVertexBuffer = 6 * sizeof(VertexDataPCT);
	lightBulb.numberOfVertices = 6;
	lightBulb.vao = basicTextureVAO;
	lightBulb.shaderProgram = textureShaderProgram;
	lightBulb.vbo = AllocateVertexBufferPCT(lightBulb);
	lightBulb.textureId = CreateTextureFromFile("lightbulb.png");

	unsigned int pcVAO;
	glGenVertexArrays(1, &pcVAO);

	GraphicsObject circle;
	int circleSteps = 10;
	float circleRadius = 5.0f;
	PCData circlePCData = 
		CreateXYCirclePC(circleRadius, { 1.0f, 1.0f, 1.0f }, circleSteps);
	SetUpDynamicPCGraphicsObject(circle, circlePCData, pcVAO, pcShaderProgram, 360);
	circle.referenceFrame[3] = glm::vec4(-20.0f, 0.0f, -10.0f, 1.0f);

	GraphicsObject spirograph;
	int spirographSteps = 10;
	float spirographR = 4.0f;
	float spirographl = 0.955f;
	float spirographk = 0.55f;
	float revolutions = 20.0f;
	PCData spirographPCData =
		CreateXYSpirographPC(
			spirographR, spirographl, spirographk, 
			{ 1.0f, 1.0f, 1.0f }, revolutions, spirographSteps);
	SetUpDynamicPCGraphicsObject(
		spirograph, spirographPCData, pcVAO, pcShaderProgram, 360 * (int)revolutions);
	spirograph.referenceFrame[3] = glm::vec4(-10.0f, 0.0f, -10.0f, 1.0f);

	GraphicsObject linearBezier;
	int linearBezierSteps = 10;
	glm::vec3 lbP0(-5.0f, 0.0f, 0.0f);
	glm::vec3 lbP1(5.0f, 0.0f, 0.0f);
	PCData linearBezierPCData =
		CreateLinearBezierPC(lbP0, lbP1, { 1.0f, 1.0f, 1.0f }, linearBezierSteps);
	SetUpDynamicPCGraphicsObject(
		linearBezier, linearBezierPCData, pcVAO, pcShaderProgram, 50);
	linearBezier.referenceFrame[3] = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);

	GraphicsObject quadraticBezier;
	int quadraticBezierSteps = 10;
	glm::vec3 qbP0(-5.0f, 0.0f, 0.0f);
	glm::vec3 qbP1(0.0f, 8.0f, 0.0f);
	glm::vec3 qbP2(5.0f, -8.0f, 0.0f);
	PCData quadraticBezierPCData =
		CreateQuadraticBezierPC(
			qbP0, qbP1, qbP2, { 1.0f, 1.0f, 1.0f }, quadraticBezierSteps);
	SetUpDynamicPCGraphicsObject(
		quadraticBezier, quadraticBezierPCData, pcVAO, pcShaderProgram, 50);
	quadraticBezier.referenceFrame[3] = glm::vec4(10.0f, 0.0f, -10.0f, 1.0f);

	GraphicsObject quadraticBezierM;
	int quadraticBezierMSteps = 10;
	glm::mat3 pM{};
	pM[0] = { -5.0f, 0.0f, 0.0f };
	pM[1] = { 0.0f, 8.0f, 0.0f };
	pM[2] = { 5.0f, -8.0f, 0.0f };
	PCData quadraticBezierPCMatData =
		CreateQuadraticBezierPCMat(
			pM, { 1.0f, 1.0f, 0.0f }, quadraticBezierMSteps);
	SetUpDynamicPCGraphicsObject(
		quadraticBezierM, quadraticBezierPCMatData, pcVAO, pcShaderProgram, 50);
	quadraticBezierM.referenceFrame[3] = glm::vec4(20.0f, 0.0f, -10.0f, 1.0f);

	GraphicsObject cubicBezier;
	int cubicBezierSteps = 20;
	glm::vec3 cbP0(-5.0f, 0.0f, 0.0f);
	glm::vec3 cbP1(0.0f, 8.0f, 0.0f);
	glm::vec3 cbP2(5.0f, -8.0f, 0.0f);
	glm::vec3 cbP3(5.0f, 0.0f, 0.0f);
	PCData cubicBezierPCData =
		CreateCubicBezierPC(
			cbP0, cbP1, cbP2, cbP3, { 1.0f, 1.0f, 1.0f }, cubicBezierSteps);
	SetUpDynamicPCGraphicsObject(
		cubicBezier, cubicBezierPCData, pcVAO, pcShaderProgram, 50);
	cubicBezier.referenceFrame[3] = glm::vec4(-20.0f, 0.0f, 0.0f, 1.0f);

	GraphicsObject cubicBezierM;
	int cubicBezierMSteps = 20;
	glm::mat4 cpM{};
	cpM[0] = { -5.0f, 0.0f, 0.0f, 1.0f };
	cpM[1] = { 0.0f, 8.0f, 0.0f, 1.0f };
	cpM[2] = { 5.0f, -8.0f, 0.0f, 1.0f };
	cpM[3] = { 5.0f, 0.0f, 0.0f, 1.0f };
	PCData cubicBezierPCMatData =
		CreateCubicBezierPCMat(
			cpM, { 1.0f, 1.0f, 0.0f }, cubicBezierMSteps);
	SetUpDynamicPCGraphicsObject(
		cubicBezierM, cubicBezierPCMatData, pcVAO, pcShaderProgram, 50);
	cubicBezierM.referenceFrame[3] = glm::vec4(-8.0f, 0.0f, 0.0f, 1.0f);

	GraphicsObject bezierPatch;
	glm::vec3 cpBezier[4][4];
	cpBezier[0][0] = { -10, 1,-10 };
	cpBezier[0][1] = { -5,  3,-10 };
	cpBezier[0][2] = {  5, -3,-10 };
	cpBezier[0][3] = {  10, 2,-10 };
	cpBezier[1][0] = { -10, 0,-5 };
	cpBezier[1][1] = { -5,  3,-5 };
	cpBezier[1][2] = {  5, -3,-5 };
	cpBezier[1][3] = {  10,-3,-5 };
	cpBezier[2][0] = { -10, 2, 5 };
	cpBezier[2][1] = { -5,  3, 5 };
	cpBezier[2][2] = {  5, -3, 5 };
	cpBezier[2][3] = {  10, 1, 5 };
	cpBezier[3][0] = { -10,-2, 10 };
	cpBezier[3][1] = { -5,  3, 10 };
	cpBezier[3][2] = {  5, -3, 10 };
	cpBezier[3][3] = {  10,-2, 10 };
	int bezierPatchSteps = 20;
	PCData bezierPatchPCData =
		CreateBezierPatch(cpBezier, { 0.0f, 1.0f, 0.0f }, bezierPatchSteps);
	SetUpDynamicPCGraphicsObject(
		bezierPatch, bezierPatchPCData, pcVAO, pcShaderProgram, 500);
	bezierPatch.referenceFrame[3] = glm::vec4(-15.0f, 2.0f, 15.0f, 1.0f);

	GraphicsObject bezierPatchX;
	int bezierPatchStepsX = 20;
	PCData bezierPatchXPCData =
		CreateBezierPatchCrissCross(
			cpBezier, { 0.0f, 1.0f, 1.0f }, bezierPatchStepsX);
	SetUpDynamicPCGraphicsObject(
		bezierPatchX, bezierPatchXPCData, pcVAO, pcShaderProgram, 
		bezierPatchStepsX * bezierPatchStepsX);
	bezierPatchX.referenceFrame[3] = glm::vec4(15.0f, 2.0f, 15.0f, 1.0f);

	float cubeYAngle = 0;
	float cubeXAngle = 0;
	float cubeZAngle = 0;
	float left = -20.0f;
	float right = 20.0f;
	float bottom = -20.0f;
	float top = 20.0f;
	int width, height;

	float aspectRatio;
	float nearPlane = 1.0f;
	float farPlane = 100.0f;
	float fieldOfView = 60;

	glm::vec3 cameraPosition;
	glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

	glm::mat4 view;
	glm::mat4 projection;

	glm::vec3 clearColor = { 0.0f, 0.0f, 0.0f };

	lightingLocation.materialAmbientLoc =
		glGetUniformLocation(lightingShaderProgram, "materialAmbientIntensity");
	lightingLocation.materialSpecularLoc =
		glGetUniformLocation(lightingShaderProgram, "materialSpecularIntensity");
	lightingLocation.materialShininessLoc =
		glGetUniformLocation(lightingShaderProgram, "materialShininess");

	// Lights
	Light globalLight{};
	globalLight.position = glm::vec3(100.0f, 100.0f, 0.0f);
	globalLight.color = glm::vec3(1.0f, 1.0f, 1.0f); // White light
	globalLight.intensity = 0.05f;
	lightingLocation.globalLightPositionLoc =
		glGetUniformLocation(lightingShaderProgram, "globalLightPosition");
	lightingLocation.globalLightColorLoc =
		glGetUniformLocation(lightingShaderProgram, "globalLightColor");
	lightingLocation.globalLightIntensityLoc =
		glGetUniformLocation(lightingShaderProgram, "globalLightIntensity");
	Light localLight{};
	localLight.position = glm::vec3(0.0f, 0.0f, -17.0f);
	localLight.color = glm::vec3(1.0f, 1.0f, 1.0f); // White light
	localLight.intensity = 0.5f;
	localLight.attenuationCoef = 0.0f;
	lightingLocation.localLightPositionLoc =
		glGetUniformLocation(lightingShaderProgram, "localLightPosition");
	lightingLocation.localLightColorLoc =
		glGetUniformLocation(lightingShaderProgram, "localLightColor");
	lightingLocation.localLightIntensityLoc =
		glGetUniformLocation(lightingShaderProgram, "localLightIntensity");
	lightingLocation.localLightAttenuationLoc =
		glGetUniformLocation(lightingShaderProgram, "localLightAttenuationCoef");
	lightingLocation.viewPositionLoc =
		glGetUniformLocation(lightingShaderProgram, "viewPosition");

	glm::mat4 lookFrame(1.0f);
	glm::mat4 cameraFrame(1.0f);
	cameraFrame[3] = glm::vec4(0.0f, 3.0f, 30.0f, 1.0f);
	glm::vec3 cameraForward;
	glm::vec3 axis(0.0f, 1.0f, 0.0f);
	float speed = 90.0f;
	double elapsedSeconds;
	float deltaAngle;
	bool lookWithMouse = false;
	bool resetCameraPosition = false;
	bool correctGamma = false;
	bool showCircle = true;
	bool showSpirograph = true;
	Timer timer;
	while (!glfwWindowShouldClose(window)) {
		elapsedSeconds = timer.GetElapsedTimeInSeconds();
		ProcessInput(window, elapsedSeconds, axis, cameraFrame, lookWithMouse);
		glfwGetWindowSize(window, &width, &height);
		mouse.windowWidth = width;
		mouse.windowHeight = height;

		if (correctGamma) {
			glEnable(GL_FRAMEBUFFER_SRGB);
		}
		else {
			glDisable(GL_FRAMEBUFFER_SRGB);
		}

		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		deltaAngle = static_cast<float>(speed * elapsedSeconds);
		cube.referenceFrame = glm::rotate(cube.referenceFrame, glm::radians(deltaAngle), axis);

		if (resetCameraPosition) {
			cameraFrame = glm::mat4(1.0f);
			cameraFrame[3] = glm::vec4(0.0f, 3.0f, 30.0f, 1.0f);
			resetCameraPosition = false;
			lookWithMouse = false;
		}
		if (lookWithMouse) {
			lookFrame = mouse.spherical.ToMat4();
			cameraFrame[0] = lookFrame[0];
			cameraFrame[1] = lookFrame[1];
			cameraFrame[2] = lookFrame[2];
		}

		cameraPosition = cameraFrame[3];
		cameraForward = -cameraFrame[2];
		cameraTarget = cameraPosition + cameraForward;
		cameraUp = cameraFrame[1];
		view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

		lightBulb.referenceFrame[3] = glm::vec4(localLight.position, 1.0f);
		PointAt(lightBulb.referenceFrame, cameraPosition);

		if (width >= height) {
			aspectRatio = width / (height * 1.0f);
		}
		else {
			aspectRatio = height / (width * 1.0f);
		}
		projection = glm::perspective(
			glm::radians(mouse.fieldOfView), aspectRatio, nearPlane, farPlane);

		// Render the object
		if (result1.isSuccess) {
			RenderObjectPCNT(
				cube, lightingLocation, projection, view,
				globalLight, localLight, cameraPosition, GL_TRIANGLES);
			RenderObjectPCNT(
				floor, lightingLocation, projection, view,
				globalLight, localLight, cameraPosition, GL_TRIANGLES);
		}

		if (result2.isSuccess)
		{
			RenderObjectPCT(
				lightBulb, textureLocation, projection, view, GL_TRIANGLES);
		}

		if (resultPC.isSuccess)
		{
			if (showCircle) {
				GenerateXYCirclePCVertexData(
					circle.vertexDataPC, circleRadius, { 1.0f, 1.0f, 1.0f }, circleSteps);
				GenerateLinesIndexDataConnected(
					circle.indexData, circle.vertexDataPC.size());
				RenderObjectPC(
					circle, pcLocation, projection, view, GL_LINES);
			}

			if (showSpirograph) {
				GenerateXYSpirographPCVertexData(
					spirograph.vertexDataPC,
					spirographR, spirographl, spirographk,
					{ 1.0f, 1.0f, 1.0f }, revolutions, spirographSteps);
				GenerateLinesIndexDataUnconnected(
					spirograph.indexData, spirograph.vertexDataPC.size());
				RenderObjectPC(
					spirograph, pcLocation, projection, view, GL_LINES);
			}


			GenerateLinearBezierPC(
				linearBezier.vertexDataPC, lbP0, lbP1, { 1.0f, 1.0f, 1.0f }, 
				linearBezierSteps);
			GenerateLinesIndexDataUnconnected(
				linearBezier.indexData, linearBezier.vertexDataPC.size());
			RenderObjectPC(
				linearBezier, pcLocation, projection, view, GL_LINES);

			GenerateQuadraticBezierPC(
				quadraticBezier.vertexDataPC, qbP0, qbP1, qbP2, 
				{ 1.0f, 1.0f, 1.0f }, quadraticBezierSteps);
			GenerateLinesIndexDataUnconnected(
				quadraticBezier.indexData, quadraticBezier.vertexDataPC.size());
			RenderObjectPC(
				quadraticBezier, pcLocation, projection, view, GL_LINES);

			GenerateQuadraticBezierPCMat(
				quadraticBezierM.vertexDataPC, pM, { 1.0f, 1.0f, 0.0f }, quadraticBezierMSteps);
			GenerateLinesIndexDataUnconnected(
				quadraticBezierM.indexData, quadraticBezierM.vertexDataPC.size());
			RenderObjectPC(
				quadraticBezierM, pcLocation, projection, view, GL_LINES);

			GenerateCubicBezierPC(
				cubicBezier.vertexDataPC, cbP0, cbP1, cbP2, cbP3,
				{ 1.0f, 1.0f, 1.0f }, cubicBezierSteps);
			GenerateLinesIndexDataUnconnected(
				cubicBezier.indexData, cubicBezier.vertexDataPC.size());
			RenderObjectPC(
				cubicBezier, pcLocation, projection, view, GL_LINES);

			GenerateCubicBezierPCMat(
				cubicBezierM.vertexDataPC, cpM, { 1.0f, 1.0f, 0.0f }, cubicBezierMSteps);
			GenerateLinesIndexDataUnconnected(
				cubicBezierM.indexData, cubicBezierM.vertexDataPC.size());
			RenderObjectPC(
				cubicBezierM, pcLocation, projection, view, GL_LINES);

			GenerateBezierPatch(
				bezierPatch.vertexDataPC, cpBezier, { 0.0f, 1.0f, 0.0f }, bezierPatchSteps);
			GenerateLinesIndexDataUnconnected(
				bezierPatch.indexData, bezierPatch.vertexDataPC.size());
			RenderObjectPC(
				bezierPatch, pcLocation, projection, view, GL_LINES);

			GenerateBezierPatch(
				bezierPatchX.vertexDataPC, cpBezier, { 0.0f, 1.0f, 1.0f }, 
				bezierPatchStepsX);
			GenerateLinesIndexDataForBezierSurface(
				bezierPatchX.indexData, bezierPatchX.vertexDataPC.size(), 
				bezierPatchStepsX);
			RenderObjectPC(
				bezierPatchX, pcLocation, projection, view, GL_LINES);
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Computing Interactive Graphics");
		ImGui::Text(message.c_str());
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("Elapsed seconds: %.3f", elapsedSeconds);
		ImGui::Text("Mouse: (%.0f, %.0f)", mouse.x, mouse.y);
		ImGui::Text("Field of View: %.0f", mouse.fieldOfView);
		ImGui::Text("Theta:%.1f, Phi:%.1f)",
			mouse.spherical.theta, mouse.spherical.phi);
		ImGui::ColorEdit3("Background color", (float*)&clearColor.r);
		ImGui::SliderFloat("Speed", &speed, 0, 360);
		ImGui::Checkbox("Use mouse to look", &lookWithMouse);
		ImGui::Checkbox("Reset camera position", &resetCameraPosition);
		ImGui::Checkbox("Correct gamma", &correctGamma);
		ImGui::Checkbox("Show Circle", &showCircle);
		ImGui::Checkbox("Show Spirograph", &showSpirograph);
		ImGui::SliderFloat("Radius", &circleRadius, 1, 10);
		ImGui::SliderInt("Steps", &circleSteps, 10, 120);
		ImGui::SliderFloat("Spirograph R", &spirographR, 1, 10);
		ImGui::SliderFloat("Spirograph l", &spirographl, 0, 1);
		ImGui::SliderFloat("Spirograph k", &spirographk, 0, 1);
		ImGui::SliderFloat("Revolutions", &revolutions, 1, 30);
		ImGui::DragFloat3("LB Point 1", &lbP0.x, 0.1f);
		ImGui::DragFloat3("LB Point 2", &lbP1.x, 0.1f);
		ImGui::DragFloat3("QB Point 1", &qbP0.x, 0.1f);
		ImGui::DragFloat3("QB Point 2", &qbP1.x, 0.1f);
		ImGui::DragFloat3("QB Point 3", &qbP2.x, 0.1f);
		ImGui::SliderInt("Patch Steps", &bezierPatchSteps, 5, 20);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

