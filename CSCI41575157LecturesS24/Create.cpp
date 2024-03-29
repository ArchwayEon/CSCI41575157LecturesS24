#include "Create.h"
#include "PCNTVertexArray.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "PCTVertexArray.h"
#include "PCVertexArray.h"

unsigned int Create::Texture2D(
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

unsigned int Create::TextureFromFile(const std::string& filePath)
{
	int textureWidth, textureHeight, numChannels;
	unsigned char* textureData =
		Create::TextureDataFromFile(
			filePath, textureWidth, textureHeight, numChannels);
	unsigned int textureId =
		Create::Texture2D(textureData, textureWidth, textureHeight);
	stbi_image_free(textureData);
	textureData = nullptr;
	return textureId;
}

unsigned char* Create::TextureDataFromFile(
	const std::string& filePath, int& width, int& height, int& numChannels)
{
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data =
		stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);
	return data;
}

void Create::PCNTScene1(
    std::unordered_map<std::string, std::shared_ptr<GraphicsObject>>& allObjects,
    std::shared_ptr<Renderer>& renderer, std::shared_ptr<Shader>& shader)
{
	unsigned char* textureData = new unsigned char[] {
		0, 0, 0, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 0, 255,
		0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 0, 255,
		0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 0, 255,
		0, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 0, 0, 0, 255
	};
	unsigned int customTextureId = Create::Texture2D(textureData, 4, 4);
	delete[] textureData;
	textureData = nullptr;

    renderer->SetShaderProgram(shader);

	std::shared_ptr<PCNTVertexArray> vaLitCube =
		std::make_shared<PCNTVertexArray>();
	vaLitCube->SetGenerator(std::make_shared<PCNTCuboidGenerator>());
	std::shared_ptr<GraphicsObject> litCube = std::make_shared<GraphicsObject>();
	vaLitCube->SetObject(litCube);
	litCube->vertexArray = vaLitCube;
	litCube->primitive = GL_TRIANGLES;
	CuboidParams cParams{};
	cParams.width = 10.0f;
	cParams.height = 10.0f;
	cParams.depth = 10.0f;
	litCube->vertexArray->Generate(cParams);
	litCube->textureId = customTextureId;
	litCube->material.ambientIntensity = 0.1f;
	litCube->material.specularIntensity = 0.5f;
	litCube->material.shininess = 16.0f;
	litCube->referenceFrame[3] = glm::vec4(0.0f, 0.0f, -25.0f, 1.0f);
	litCube->CreateBoundingBox(10.1f, 10.1f, 10.1f);
	allObjects["litCube"] = litCube;
	renderer->AddObject(litCube);

	std::shared_ptr<PCNTVertexArray> vaFloor =
		std::make_shared<PCNTVertexArray>();
	vaFloor->SetGenerator(std::make_shared<PCNTXZPlaneGenerator>());
	std::shared_ptr<GraphicsObject> floor = std::make_shared<GraphicsObject>();
	vaFloor->SetObject(floor);
	floor->vertexArray = vaFloor;
	floor->primitive = GL_TRIANGLES;
	XZPlaneParams xzpParams{};
	xzpParams.width = 50.0f;
	xzpParams.depth = 50.0f;
	xzpParams.repeatS = 5.0f;
	xzpParams.repeatT = 5.0f;
	floor->vertexArray->Generate(xzpParams);
	floor->textureId = Create::TextureFromFile("stone-road-texture.jpg");
	floor->referenceFrame[3] = glm::vec4(0.0f, -5.0f, 0.0f, 1.0f);
	floor->material.ambientIntensity = 0.1f;
	floor->material.specularIntensity = 0.5f;
	floor->material.shininess = 16.0f;
	allObjects["floor"] = floor;
	renderer->AddObject(floor);
}

void Create::PCTScene1(
	std::unordered_map<std::string, std::shared_ptr<GraphicsObject>>& allObjects, 
	std::shared_ptr<Renderer>& renderer, std::shared_ptr<Shader>& shader)
{
	renderer->SetShaderProgram(shader);

	std::shared_ptr<PCTVertexArray> vaLightBulb =
		std::make_shared<PCTVertexArray>();
	vaLightBulb->SetGenerator(std::make_shared<PCTXYPlaneGenerator>());
	std::shared_ptr<GraphicsObject> lightBulb = std::make_shared<GraphicsObject>();
	vaLightBulb->SetObject(lightBulb);
	lightBulb->vertexArray = vaLightBulb;
	lightBulb->primitive = GL_TRIANGLES;
	XYPlaneParams xypParams{};
	lightBulb->vertexArray->Generate(xypParams);
	lightBulb->textureId = Create::TextureFromFile("lightbulb.png");
	allObjects["lightBulb"] = lightBulb;
	renderer->AddObject(lightBulb);
}

void Create::PCScene1(
	std::unordered_map<std::string, std::shared_ptr<GraphicsObject>>& allObjects, 
	std::shared_ptr<Renderer>& renderer, std::shared_ptr<Shader>& shader)
{
	renderer->SetShaderProgram(shader);
	// Circle
	std::shared_ptr<PCVertexArray> vaCircle = std::make_shared<PCVertexArray>();
	vaCircle->SetGenerator(std::make_shared<PCCircleGenerator>());
	std::shared_ptr<GraphicsObject> circle = std::make_shared<GraphicsObject>();
	vaCircle->SetObject(circle);
	circle->vertexArray = vaCircle;
	circle->primitive = GL_LINES;
	CircleParams circleParams{};
	circleParams.radius = 5.0f;
	circleParams.color = { 1.0f, 1.0f, 1.0f };
	circleParams.steps = 10;
	circle->vertexArray->Generate(circleParams);
	circle->vertexArray->SetAsDynamicGraphicsObject(360, 360 * 2);
	circle->referenceFrame[3] = glm::vec4(-20.0f, 0.0f, -10.0f, 1.0f);
	allObjects["circle"] = circle;
	renderer->AddObject(circle);
	// Spirograph
	std::shared_ptr<PCVertexArray> vaSpirograph =
		std::make_shared<PCVertexArray>();
	vaSpirograph->SetGenerator(std::make_shared<PCSpirographGenerator>());
	std::shared_ptr<GraphicsObject> spirograph = std::make_shared<GraphicsObject>();
	vaSpirograph->SetObject(spirograph);
	spirograph->vertexArray = vaSpirograph;
	spirograph->primitive = GL_LINES;
	SpirographParams sParams{};
	sParams.color = { 1.0f, 1.0f, 1.0f };
	sParams.k = 0.55f;
	sParams.l = 0.955f;
	sParams.R = 4.0f;
	sParams.revolutions = 20.0f;
	sParams.steps = 10;
	spirograph->vertexArray->Generate(sParams);
	int maxNumberOfVertices = static_cast<int>(360 * sParams.revolutions);
	int maxNumberOfIndices = maxNumberOfVertices * 2;
	spirograph->vertexArray->SetAsDynamicGraphicsObject(
		maxNumberOfVertices, maxNumberOfIndices);
	spirograph->referenceFrame[3] = glm::vec4(-10.0f, 0.0f, -10.0f, 1.0f);
	allObjects["spirograph"] = spirograph;
	renderer->AddObject(spirograph);
	// Linear Bezier
	std::shared_ptr<PCVertexArray> vaLinearBezier =
		std::make_shared<PCVertexArray>();
	vaLinearBezier->SetGenerator(std::make_shared<PCLinearBezierGenerator>());
	std::shared_ptr<GraphicsObject> linearBezier = std::make_shared<GraphicsObject>();
	vaLinearBezier->SetObject(linearBezier);
	linearBezier->vertexArray = vaLinearBezier;
	linearBezier->primitive = GL_LINES;
	LinearBezierParams lbParams{};
	lbParams.steps = 10;
	lbParams.p0 = { -5.0f, 0.0f, 0.0f };
	lbParams.p1 = { 5.0f, 0.0f, 0.0f };
	lbParams.color = { 1.0f, 1.0f, 1.0f };
	linearBezier->vertexArray->Generate(lbParams);
	linearBezier->vertexArray->SetAsDynamicGraphicsObject(50, 50 * 2);
	linearBezier->referenceFrame[3] = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);
	allObjects["linearBezier"] = linearBezier;
	renderer->AddObject(linearBezier);
}
