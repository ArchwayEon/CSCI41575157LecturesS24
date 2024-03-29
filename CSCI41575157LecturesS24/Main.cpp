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
#include <algorithm>

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include "GraphicsStructures.h"
#include <unordered_map>
#include "Renderer.h"
#include "PCNTVertexArray.h"
#include "Shader.h"
#include "PCTVertexArray.h"
#include "PCVertexArray.h"
#include "GraphicsObject.h"
#include "PCIVertexArray.h"
#include "Ray.h"
#include "Create.h"

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

	mouse.nsx = xPercent * 2.0 - 1.0;
	mouse.nsy = -(yPercent * 2.0 - 1.0);
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

static void EnableAttribute(
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

//static unsigned char* LoadTextureDataFromFile(
//	const std::string& filePath, int& width, int& height, int& numChannels)
//{
//	//stbi_set_flip_vertically_on_load(true);
//	//unsigned char* data =
//	//	stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);
//	//return data;
//}

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

//static unsigned int CreateTextureFromFile(const std::string& filePath)
//{
//	int textureWidth, textureHeight, numChannels;
//	unsigned char* textureData =
//		LoadTextureDataFromFile(
//			filePath, textureWidth, textureHeight, numChannels);
//	unsigned int textureId =
//		Create2DTexture(textureData, textureWidth, textureHeight);
//	stbi_image_free(textureData);
//	textureData = nullptr;
//	return textureId;
//}

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

	// Create the shaders
	std::string lightingVertexSource = ReadFromFile("lighting.vert.glsl");
	std::string lightingFragmentSource = ReadFromFile("lighting.frag.glsl");
	std::shared_ptr<Shader> lightingShader =
		std::make_shared<Shader>(lightingVertexSource, lightingFragmentSource);

	std::string basicVertexSource = ReadFromFile("basic.vert.glsl");
	std::string basicFragmentSource = ReadFromFile("basic.frag.glsl");
	std::shared_ptr<Shader> basicPCTShader =
		std::make_shared<Shader>(basicVertexSource, basicFragmentSource);

	std::string pcVertexSource = ReadFromFile("pc.vert.glsl");
	std::string pcFragmentSource = ReadFromFile("pc.frag.glsl");
	std::shared_ptr<Shader> basicPCShader =
		std::make_shared<Shader>(pcVertexSource, pcFragmentSource);

	std::string pciVertexSource = ReadFromFile("pci.vert.glsl");
	std::string pciFragmentSource = ReadFromFile("pc.frag.glsl");
	std::shared_ptr<Shader> basicPCIShader =
		std::make_shared<Shader>(pciVertexSource, pciFragmentSource);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");

	lightingShader->AddUniform("projection");
	lightingShader->AddUniform("view");
	lightingShader->AddUniform("world");

	basicPCTShader->AddUniform("projection");
	basicPCTShader->AddUniform("view");
	basicPCTShader->AddUniform("world");

	basicPCShader->AddUniform("projection");
	basicPCShader->AddUniform("view");
	basicPCShader->AddUniform("world");

	basicPCIShader->AddUniform("projection");
	basicPCIShader->AddUniform("view");
	basicPCIShader->AddUniform("world");

	typedef std::shared_ptr<Renderer> SRenderer;
	typedef std::shared_ptr<GraphicsObject> SGraphicsObject;
	std::unordered_map<std::string, SGraphicsObject> allObjects;

	int maxNumberOfVertices = 0;
	int maxNumberOfIndices = 0;

	SRenderer lightingRenderer = std::make_shared<Renderer>();
	// The textured cube and the floor.
	Create::PCNTScene1(allObjects, lightingRenderer, lightingShader);

	SRenderer basicPCTRenderer = std::make_shared<Renderer>();
	// The lightbulb
	Create::PCTScene1(allObjects, basicPCTRenderer, basicPCTShader);

	SRenderer basicPCRenderer = std::make_shared<Renderer>();
	// The various curve objects
	Create::PCScene1(allObjects, basicPCRenderer, basicPCShader);

	//std::shared_ptr<PCVertexArray> vaBezierPatch =
	//	std::make_shared<PCVertexArray>();
	//vaBezierPatch->SetGenerator(
	//	std::make_shared<PCBezierPatchGenerator>());
	//SGraphicsObject bezierPatch = std::make_shared<GraphicsObject>();
	//vaBezierPatch->SetObject(bezierPatch);
	//bezierPatch->vertexArray = vaBezierPatch;
	//bezierPatch->primitive = GL_LINES;
	//BezierPatchParams bpParams{};
	//bpParams.steps = 20;
	//bpParams.color = { 0.0f, 0.8f, 0.0f };
	//bpParams.indexType = 1;
	//bpParams.cpBezier[0][0] = { -10, 1,-10 };
	//bpParams.cpBezier[0][1] = { -5,  3,-10 };
	//bpParams.cpBezier[0][2] = { 5, -3,-10 };
	//bpParams.cpBezier[0][3] = { 10, 2,-10 };
	//bpParams.cpBezier[1][0] = { -10, 0,-5 };
	//bpParams.cpBezier[1][1] = { -5,  3,-5 };
	//bpParams.cpBezier[1][2] = { 5, -3,-5 };
	//bpParams.cpBezier[1][3] = { 10,-3,-5 };
	//bpParams.cpBezier[2][0] = { -10, 2, 5 };
	//bpParams.cpBezier[2][1] = { -5,  3, 5 };
	//bpParams.cpBezier[2][2] = { 5, -3, 5 };
	//bpParams.cpBezier[2][3] = { 10, 1, 5 };
	//bpParams.cpBezier[3][0] = { -10,-2, 10 };
	//bpParams.cpBezier[3][1] = { -5,  3, 10 };
	//bpParams.cpBezier[3][2] = { 5, -3, 10 };
	//bpParams.cpBezier[3][3] = { 10,-2, 10 };
	//bezierPatch->vertexArray->Generate(bpParams);
	//bezierPatch->vertexArray->SetAsDynamicGraphicsObject(500, 500 * 2);
	//bezierPatch->referenceFrame[3] = glm::vec4(-15.0f, 2.0f, 15.0f, 1.0f);
	//allObjects["bezierPatch"] = bezierPatch;
	//basicPCRenderer->AddObject(bezierPatch);

	std::shared_ptr<PCVertexArray> vaBezierPatchX =
		std::make_shared<PCVertexArray>();
	vaBezierPatchX->SetGenerator(
		std::make_shared<PCBezierPatchGenerator>());
	SGraphicsObject bezierPatchX = std::make_shared<GraphicsObject>();
	vaBezierPatchX->SetObject(bezierPatchX);
	bezierPatchX->vertexArray = vaBezierPatchX;
	bezierPatchX->primitive = GL_LINES;
	BezierPatchParams bpxParams{};
	bpxParams.steps = 20;
	bpxParams.color = { 0.0f, 0.8f, 0.0f };
	bpxParams.indexType = 2;
	bpxParams.cpBezier[0][0] = { -10, 1,-10 };
	bpxParams.cpBezier[0][1] = { -5,  3,-10 };
	bpxParams.cpBezier[0][2] = { 5, -3,-10 };
	bpxParams.cpBezier[0][3] = { 10, 2,-10 };
	bpxParams.cpBezier[1][0] = { -10, 0,-5 };
	bpxParams.cpBezier[1][1] = { -5,  3,-5 };
	bpxParams.cpBezier[1][2] = { 5, -3,-5 };
	bpxParams.cpBezier[1][3] = { 10,-3,-5 };
	bpxParams.cpBezier[2][0] = { -10, 2, 5 };
	bpxParams.cpBezier[2][1] = { -5,  3, 5 };
	bpxParams.cpBezier[2][2] = { 5, -3, 5 };
	bpxParams.cpBezier[2][3] = { 10, 1, 5 };
	bpxParams.cpBezier[3][0] = { -10,-2, 10 };
	bpxParams.cpBezier[3][1] = { -5,  3, 10 };
	bpxParams.cpBezier[3][2] = { 5, -3, 10 };
	bpxParams.cpBezier[3][3] = { 10,-2, 10 };
	bezierPatchX->vertexArray->Generate(bpxParams);
	bezierPatchX->vertexArray->SetAsDynamicGraphicsObject(500, 500 * 2);
	allObjects["bezierPatchX"] = bezierPatchX;
	basicPCRenderer->AddObject(bezierPatchX);

	basicPCRenderer->SetObjectsVisibility(false);

	std::shared_ptr<PCLineCuboidGenerator> lineCuboidGenerator =
		std::make_shared<PCLineCuboidGenerator>();

	std::shared_ptr<PCVertexArray> vaLineCuboid =
		std::make_shared<PCVertexArray>();
	vaLineCuboid->SetGenerator(lineCuboidGenerator);
	SGraphicsObject lineCuboid = std::make_shared<GraphicsObject>();
	vaLineCuboid->SetObject(lineCuboid);
	lineCuboid->vertexArray = vaLineCuboid;
	lineCuboid->primitive = GL_LINES;
	LineCuboidParams lcParams{};
	lcParams.width = 5.0f;
	lcParams.height = 5.0f;
	lcParams.depth = 5.0f;
	lcParams.color = { 1.0f, 0.0f, 1.0f };
	lineCuboid->vertexArray->Generate(lcParams);
	lineCuboid->vertexArray->SetAsDynamicGraphicsObject(8, 24);
	lineCuboid->CreateBoundingBox(5.0f, 5.0f, 5.0f);
	allObjects["lineCuboid"] = lineCuboid;
	basicPCRenderer->AddObject(lineCuboid);

	//allObjects["circle"]->isVisible = true;

	std::vector<glm::vec3> worldPositions;
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			worldPositions.push_back(bpxParams.cpBezier[row][col]);
		}
	}
	std::vector<glm::vec3> instanceColors;
	for (int i = 0; i < 16; i++) {
		instanceColors.push_back({ 1.0f, 0.0f, 0.0f });
	}
	instanceColors[5] = { 1.0f, 1.0f, 1.0f };

	SRenderer basicPCIRenderer = std::make_shared<Renderer>();
	basicPCIRenderer->SetShaderProgram(basicPCIShader);

	std::shared_ptr<PCIVertexArray> vaInsLineCuboid =
		std::make_shared<PCIVertexArray>(worldPositions);
	vaInsLineCuboid->SetInstanceColors(instanceColors);
	
	vaInsLineCuboid->SetGenerator(lineCuboidGenerator);
	SGraphicsObject insLineCuboid = std::make_shared<GraphicsObject>();
	vaInsLineCuboid->SetObject(insLineCuboid);
	insLineCuboid->vertexArray = vaInsLineCuboid;
	insLineCuboid->primitive = GL_LINES;
	insLineCuboid->instances = 16;
	lcParams.width = 0.5f;
	lcParams.height = 0.5f;
	lcParams.depth = 0.5f;
	lcParams.color = { 0.0f, 0.0f, 1.0f };
	insLineCuboid->vertexArray->Generate(lcParams);
	maxNumberOfVertices = 8;
	maxNumberOfIndices = 24;
	insLineCuboid->vertexArray->SetAsDynamicGraphicsObject(
		maxNumberOfVertices, maxNumberOfIndices);
	insLineCuboid->referenceFrame[3] = glm::vec4(bpxParams.cpBezier[0][0], 1.0f);
	allObjects["insLineCuboid"] = insLineCuboid;
	basicPCIRenderer->AddObject(insLineCuboid);
	basicPCIRenderer->SetObjectsVisibility(false);

	float boxYAngle = 0;
	float boxXAngle = 0;
	float boxZAngle = 0;
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


	lightingShader->AddUniform("materialAmbientIntensity");
	lightingShader->AddUniform("materialSpecularIntensity");
	lightingShader->AddUniform("materialShininess");

	// Lights
	Light globalLight{};
	globalLight.position = glm::vec3(100.0f, 100.0f, 0.0f);
	globalLight.color = glm::vec3(1.0f, 1.0f, 1.0f); // White light
	globalLight.intensity = 0.05f;

	lightingShader->AddUniform("globalLightPosition");
	lightingShader->AddUniform("globalLightColor");
	lightingShader->AddUniform("globalLightIntensity");

	Light localLight{};
	localLight.position = glm::vec3(0.0f, -3.0f, -17.0f);
	localLight.color = glm::vec3(1.0f, 1.0f, 1.0f); // White light
	localLight.intensity = 0.5f;
	localLight.attenuationCoef = 0.0f;

	lightingShader->AddUniform("localLightPosition");
	lightingShader->AddUniform("localLightColor");
	lightingShader->AddUniform("localLightIntensity");
	lightingShader->AddUniform("localLightAttenuationCoef");
	lightingShader->AddUniform("viewPosition");

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
	//bool showCircle = true;
	//bool showSpirograph = true;
	Ray mouseRay;
	glm::vec3 rayStart{};
	glm::vec3 rayDir{};
	GeometricPlane plane;
	plane.SetDistanceFromOrigin(allObjects["floor"]->referenceFrame[3].y);
	Intersection floorIntersection, boxILeft, boxIRight, boxIFront, boxIBack, boxITop, boxIBottom;
	glm::vec3 floorIntersectionPoint{};
	glm::vec3 boxIntersectionPoint{};
	Timer timer;
	std::stringstream log;
	float litCubeAmbient = allObjects["litCube"]->material.ambientIntensity;
	while (!glfwWindowShouldClose(window)) {
		elapsedSeconds = timer.GetElapsedTimeInSeconds();
		ProcessInput(window, elapsedSeconds, axis, cameraFrame, lookWithMouse);
		glfwGetWindowSize(window, &width, &height);
		if (width == 0 || height == 0) continue; // minimized
		mouse.windowWidth = width;
		mouse.windowHeight = height;

		if (correctGamma) {
			glEnable(GL_FRAMEBUFFER_SRGB);
		}
		else {
			glDisable(GL_FRAMEBUFFER_SRGB);
		}

		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | 
			GL_STENCIL_BUFFER_BIT);

		deltaAngle = static_cast<float>(speed * elapsedSeconds);
		allObjects["litCube"]->referenceFrame = glm::rotate(
			allObjects["litCube"]->referenceFrame, glm::radians(deltaAngle), axis);

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

		if (width >= height) {
			aspectRatio = width / (height * 1.0f);
		}
		else {
			aspectRatio = height / (width * 1.0f);
		}
		projection = glm::perspective(
			glm::radians(mouse.fieldOfView), aspectRatio, nearPlane, farPlane);

		// Set up the ray
		mouseRay.Create((float)mouse.nsx, (float)mouse.nsy, projection, view);
		rayStart = mouseRay.GetStart();
		rayDir = mouseRay.GetDirection();
		floorIntersection = mouseRay.GetIntersectionWithPlane(plane);

		if (floorIntersection.isIntersecting) {
			floorIntersectionPoint = mouseRay.GetPosition(floorIntersection.offset);
			localLight.position.x = floorIntersectionPoint.x;
			localLight.position.y = -3.0f;
			localLight.position.z = floorIntersectionPoint.z;
		}
		else {
			localLight.position.x = 0.0f;
			localLight.position.y = -3.0f;
			localLight.position.z = 5.0f;
		}

		allObjects["lineCuboid"]->referenceFrame = glm::mat4(1.0f);
		allObjects["lineCuboid"]->referenceFrame = glm::rotate(
			allObjects["lineCuboid"]->referenceFrame, glm::radians(boxXAngle), {1.0f, 0.0f, 0.0f});
		allObjects["lineCuboid"]->referenceFrame = glm::rotate(
			allObjects["lineCuboid"]->referenceFrame, glm::radians(boxYAngle), { 0.0f, 1.0f, 0.0f });
		allObjects["lineCuboid"]->referenceFrame = glm::rotate(
			allObjects["lineCuboid"]->referenceFrame, glm::radians(boxZAngle), { 0.0f, 0.0f, 1.0f });

		std::string intersectionMessage = "Don't know";
		bool isIntersectingBox = 
			allObjects["lineCuboid"]->IsIntersectingWithRay(mouseRay);
		auto& boundingBox = allObjects["lineCuboid"]->GetBoundingBox();
		auto& intersections = boundingBox->GetIntersections();
		log.str(std::string()); // clear the log
		for (int i = 0; i < intersections.size(); i++) {
			log << "[" << i << "] " << intersections[i].offset;
		}

		if (isIntersectingBox == true) {
			intersectionMessage = "Intersection";
			boxIntersectionPoint = boundingBox->GetIntersectionPoint();
			localLight.position.x = allObjects["lineCuboid"]->referenceFrame[3].x;
			localLight.position.y = allObjects["lineCuboid"]->referenceFrame[3].y;
			localLight.position.z = allObjects["lineCuboid"]->referenceFrame[3].z;
		}
		else {
			boxIntersectionPoint = { -1000, -1000, -1000 };
			intersectionMessage = "No Intersection";
		}

		bool isIntersectingCube =
			allObjects["litCube"]->IsIntersectingWithRay(mouseRay);

		if (isIntersectingCube) {
			allObjects["litCube"]->material.ambientIntensity = 1.0f;
		}
		else {
			allObjects["litCube"]->material.ambientIntensity = litCubeAmbient;
		}

		allObjects["lightBulb"]->referenceFrame[3] = glm::vec4(localLight.position, 1.0f);
		PointAt(allObjects["lightBulb"]->referenceFrame, cameraPosition);
		
		lightingRenderer->Select();
		lightingRenderer->Send("projection", projection);
		lightingRenderer->Send("view", view);
		lightingRenderer->Send("globalLightPosition", globalLight.position);
		lightingRenderer->Send("globalLightColor", globalLight.color);
		lightingRenderer->Send("globalLightIntensity", globalLight.intensity);
		lightingRenderer->Send("localLightPosition", localLight.position);
		lightingRenderer->Send("localLightColor", localLight.color);
		lightingRenderer->Send("localLightIntensity", localLight.intensity);
		lightingRenderer->Send(
			"localLightAttenuationCoef", localLight.attenuationCoef);
		lightingRenderer->Send("viewPosition", cameraPosition);
		lightingRenderer->Render();

		SGraphicsObject object;

		basicPCRenderer->Select();
		basicPCRenderer->Send("projection", projection);
		basicPCRenderer->Send("view", view);

		object = allObjects["circle"];
		if (object->isVisible) {
			object->vertexArray->Generate();
		}
		object = allObjects["spirograph"];
		if (object->isVisible) {
			object->vertexArray->Generate();
		}
		object = allObjects["linearBezier"];
		if (object->isVisible) {
			object->vertexArray->Generate();
		}
		object = allObjects["quadraticBezier"];
		if (object->isVisible) {
			object->vertexArray->Generate();
		}
		object = allObjects["quadraticBezierM"];
		if (object->isVisible) {
			object->vertexArray->Generate();
		}
		object = allObjects["cubicBezier"];
		if (object->isVisible) {
			object->vertexArray->Generate();
		}
		object = allObjects["cubicBezierM"];
		if (object->isVisible) {
			object->vertexArray->Generate();
		}
		object = allObjects["bezierPatch"];
		if (object->isVisible) {
			object->vertexArray->Generate();
		}
		object = allObjects["bezierPatchX"];
		if (object->isVisible) {
			object->vertexArray->Generate(bpxParams);
		}

		basicPCRenderer->Render();

		basicPCIRenderer->Select();
		basicPCIRenderer->Send("projection", projection);
		basicPCIRenderer->Send("view", view);
		basicPCIRenderer->Render();

		basicPCTRenderer->Select();
		basicPCTRenderer->Send("projection", projection);
		basicPCTRenderer->Send("view", view);
		basicPCTRenderer->Render();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Computing Interactive Graphics");
		ImGui::Text(BaseObject::GetLog().c_str());
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("Elapsed seconds: %.3f", elapsedSeconds);
		ImGui::Text("Mouse: (%.0f, %.0f) (%.3f, %.3f)", 
			mouse.x, mouse.y, mouse.nsx, mouse.nsy);
		ImGui::DragFloat3("Ray Start", (float*)&rayStart.x);
		ImGui::DragFloat3("Ray Direction", (float*)&rayDir.x);
		ImGui::Text("Field of View: %.0f", mouse.fieldOfView);
		ImGui::Text("Theta:%.1f, Phi:%.1f)",
			mouse.spherical.theta, mouse.spherical.phi);
		ImGui::ColorEdit3("Background color", (float*)&clearColor.r);
		ImGui::SliderFloat("Speed", &speed, 0, 360);
		ImGui::SliderFloat("Box X Angle", &boxXAngle, 0, 360);
		ImGui::SliderFloat("Box Y Angle", &boxYAngle, 0, 360);
		ImGui::SliderFloat("Box Z Angle", &boxZAngle, 0, 360);
		ImGui::Checkbox("Use mouse to look", &lookWithMouse);
		ImGui::Checkbox("Reset camera position", &resetCameraPosition);
		ImGui::Checkbox("Correct gamma", &correctGamma);
		ImGui::Text("Intersections: %s", log.str().c_str());
		ImGui::Text("Floor Intersection: (%.3f %.3f %.3f)", 
			floorIntersectionPoint.x, floorIntersectionPoint.y, floorIntersectionPoint.z);
		ImGui::Text("Box Intersection: (%.3f %.3f %.3f)",
			boxIntersectionPoint.x, boxIntersectionPoint.y, boxIntersectionPoint.z);
		ImGui::Text("Intersection ?: %s", intersectionMessage.c_str());
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

