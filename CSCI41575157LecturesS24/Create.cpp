#include "Create.h"
#include "PCNTVertexArray.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "PCTVertexArray.h"
#include "PCVertexArray.h"
#include "PCIVertexArray.h"

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
	litCube->referenceFrame.SetPosition(glm::vec3(0.0f, 0.0f, -25.0f));
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
	floor->referenceFrame.SetPosition(glm::vec3(0.0f, -5.0f, 0.0f));
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
	circle->referenceFrame.SetPosition(glm::vec3(-20.0f, 0.0f, -10.0f));
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
	spirograph->referenceFrame.SetPosition(glm::vec3(-10.0f, 0.0f, -10.0f));
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
	linearBezier->referenceFrame.SetPosition(glm::vec3(0.0f, 0.0f, -10.0f));
	allObjects["linearBezier"] = linearBezier;
	renderer->AddObject(linearBezier);
	// Quadratic Bezier
	std::shared_ptr<PCVertexArray> vaQuadraticBezier =
		std::make_shared<PCVertexArray>();
	vaQuadraticBezier->SetGenerator(
		std::make_shared<PCQuadraticBezierGenerator>());
	std::shared_ptr<GraphicsObject> quadraticBezier = std::make_shared<GraphicsObject>();
	vaQuadraticBezier->SetObject(quadraticBezier);
	quadraticBezier->vertexArray = vaQuadraticBezier;
	quadraticBezier->primitive = GL_LINES;
	QuadraticBezierParams qbParams{};
	qbParams.steps = 10;
	qbParams.p0 = { -5.0f, 0.0f, 0.0f };
	qbParams.p1 = { 0.0f, 8.0f, 0.0f };
	qbParams.p2 = { 5.0f, -8.0f, 0.0f };
	qbParams.color = { 1.0f, 0.0f, 1.0f };
	quadraticBezier->vertexArray->Generate(qbParams);
	quadraticBezier->vertexArray->SetAsDynamicGraphicsObject(50, 50 * 2);
	quadraticBezier->referenceFrame.SetPosition(glm::vec3(10.0f, 0.0f, -10.0f));
	allObjects["quadraticBezier"] = quadraticBezier;
	renderer->AddObject(quadraticBezier);
	// Quadratic Bezier using a matrix
	std::shared_ptr<PCVertexArray> vaQuadraticBezierM =
		std::make_shared<PCVertexArray>();
	vaQuadraticBezierM->SetGenerator(
		std::make_shared<PCQuadraticBezierMGenerator>());
	std::shared_ptr<GraphicsObject> quadraticBezierM = std::make_shared<GraphicsObject>();
	vaQuadraticBezierM->SetObject(quadraticBezierM);
	quadraticBezierM->vertexArray = vaQuadraticBezierM;
	quadraticBezierM->primitive = GL_LINES;
	QuadraticBezierMParams qbmParams{};
	qbmParams.steps = 10;
	qbmParams.color = { 0.0f, 0.8f, 1.0f };
	qbmParams.pM[0] = { -5.0f, 0.0f, 0.0f };
	qbmParams.pM[1] = { 0.0f, 8.0f, 0.0f };
	qbmParams.pM[2] = { 5.0f, -8.0f, 0.0f };
	quadraticBezierM->vertexArray->Generate(qbmParams);
	quadraticBezierM->vertexArray->SetAsDynamicGraphicsObject(50, 50 * 2);
	quadraticBezierM->referenceFrame.SetPosition(glm::vec3(20.0f, 0.0f, -10.0f));
	allObjects["quadraticBezierM"] = quadraticBezierM;
	renderer->AddObject(quadraticBezierM);
	// Cubic Bezier
	std::shared_ptr<PCVertexArray> vaCubicBezier =
		std::make_shared<PCVertexArray>();
	vaCubicBezier->SetGenerator(
		std::make_shared<PCCubicBezierGenerator>());
	std::shared_ptr<GraphicsObject> cubicBezier = std::make_shared<GraphicsObject>();
	vaCubicBezier->SetObject(cubicBezier);
	cubicBezier->vertexArray = vaCubicBezier;
	cubicBezier->primitive = GL_LINES;
	CubicBezierParams cbParams{};
	cbParams.steps = 20;
	cbParams.color = { 0.7f, 0.8f, 0.0f };
	cbParams.p0 = { -5.0f, 0.0f, 0.0f };
	cbParams.p1 = { 0.0f, 8.0f, 0.0f };
	cbParams.p2 = { 5.0f, -8.0f, 0.0f };
	cbParams.p3 = { 5.0f, 0.0f, 0.0f };
	cubicBezier->vertexArray->Generate(cbParams);
	maxNumberOfVertices = 50;
	maxNumberOfIndices = maxNumberOfVertices * 2;
	cubicBezier->vertexArray->SetAsDynamicGraphicsObject(
		maxNumberOfVertices, maxNumberOfIndices);
	cubicBezier->referenceFrame.SetPosition(glm::vec3(-20.0f, 0.0f, 0.0f));
	allObjects["cubicBezier"] = cubicBezier;
	renderer->AddObject(cubicBezier);
	// Cubix Bezier using a matrix
	std::shared_ptr<PCVertexArray> vaCubicBezierM =
		std::make_shared<PCVertexArray>();
	vaCubicBezierM->SetGenerator(
		std::make_shared<PCCubicBezierMGenerator>());
	std::shared_ptr<GraphicsObject> cubicBezierM = std::make_shared<GraphicsObject>();
	vaCubicBezierM->SetObject(cubicBezierM);
	cubicBezierM->vertexArray = vaCubicBezierM;
	cubicBezierM->primitive = GL_LINES;
	CubicBezierMParams cbmParams{};
	cbmParams.steps = 20;
	cbmParams.color = { 0.2f, 0.8f, 0.1f };
	cbmParams.pM[0] = { -5.0f, 0.0f, 0.0f, 1.0f };
	cbmParams.pM[1] = { 0.0f, 8.0f, 0.0f, 1.0f };
	cbmParams.pM[2] = { 5.0f, -8.0f, 0.0f, 1.0f };
	cbmParams.pM[3] = { 5.0f, 0.0f, 0.0f, 1.0f };
	cubicBezierM->vertexArray->Generate(cbmParams);
	cubicBezierM->vertexArray->SetAsDynamicGraphicsObject(50, 50 * 2);
	cubicBezierM->referenceFrame.SetPosition(glm::vec3(-8.0f, 0.0f, 0.0f));
	allObjects["cubicBezierM"] = cubicBezierM;
	renderer->AddObject(cubicBezierM);
	// Bezier Patch
	std::shared_ptr<PCVertexArray> vaBezierPatch =
		std::make_shared<PCVertexArray>();
	vaBezierPatch->SetGenerator(std::make_shared<PCBezierPatchGenerator>());
	std::shared_ptr<GraphicsObject> bezierPatch = std::make_shared<GraphicsObject>();
	vaBezierPatch->SetObject(bezierPatch);
	bezierPatch->vertexArray = vaBezierPatch;
	bezierPatch->primitive = GL_LINES;
	BezierPatchParams bpParams{};
	bpParams.steps = 20;
	bpParams.color = { 0.0f, 0.8f, 0.0f };
	bpParams.indexType = 1;
	bpParams.cpBezier[0][0] = { -10, 1,-10 };
	bpParams.cpBezier[0][1] = { -5,  3,-10 };
	bpParams.cpBezier[0][2] = { 5, -3,-10 };
	bpParams.cpBezier[0][3] = { 10, 2,-10 };
	bpParams.cpBezier[1][0] = { -10, 0,-5 };
	bpParams.cpBezier[1][1] = { -5,  3,-5 };
	bpParams.cpBezier[1][2] = { 5, -3,-5 };
	bpParams.cpBezier[1][3] = { 10,-3,-5 };
	bpParams.cpBezier[2][0] = { -10, 2, 5 };
	bpParams.cpBezier[2][1] = { -5,  3, 5 };
	bpParams.cpBezier[2][2] = { 5, -3, 5 };
	bpParams.cpBezier[2][3] = { 10, 1, 5 };
	bpParams.cpBezier[3][0] = { -10,-2, 10 };
	bpParams.cpBezier[3][1] = { -5,  3, 10 };
	bpParams.cpBezier[3][2] = { 5, -3, 10 };
	bpParams.cpBezier[3][3] = { 10,-2, 10 };
	bezierPatch->vertexArray->Generate(bpParams);
	bezierPatch->vertexArray->SetAsDynamicGraphicsObject(500, 500 * 2);
	bezierPatch->referenceFrame.SetPosition(glm::vec3(-15.0f, 2.0f, 15.0f));
	allObjects["bezierPatch"] = bezierPatch;
	renderer->AddObject(bezierPatch);
	// Bezier Patch criss cross
	std::shared_ptr<PCVertexArray> vaBezierPatchX =
		std::make_shared<PCVertexArray>();
	vaBezierPatchX->SetGenerator(std::make_shared<PCBezierPatchGenerator>());
	std::shared_ptr<GraphicsObject> bezierPatchX = std::make_shared<GraphicsObject>();
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
	renderer->AddObject(bezierPatchX);
	// Line Cuboid
	std::shared_ptr<PCLineCuboidGenerator> lineCuboidGenerator =
		std::make_shared<PCLineCuboidGenerator>();
	std::shared_ptr<PCVertexArray> vaLineCuboid =
		std::make_shared<PCVertexArray>();
	vaLineCuboid->SetGenerator(lineCuboidGenerator);
	std::shared_ptr<GraphicsObject> lineCuboid = std::make_shared<GraphicsObject>();
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
	renderer->AddObject(lineCuboid);

}

void Create::PCIScene1(
	std::unordered_map<std::string, std::shared_ptr<GraphicsObject>>& allObjects, 
	std::shared_ptr<Renderer>& renderer, std::shared_ptr<Shader>& shader)
{
	auto& generator = reinterpret_cast<PCBezierPatchGenerator&>(
		allObjects["bezierPatchX"]->vertexArray->GetGenerator());
	auto& bpxParams = generator.GetParameters();

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

	renderer->SetShaderProgram(shader);

	std::shared_ptr<PCIVertexArray> vaInsLineCuboid =
		std::make_shared<PCIVertexArray>(worldPositions);
	vaInsLineCuboid->SetInstanceColors(instanceColors);

	vaInsLineCuboid->SetGenerator(std::make_shared<PCLineCuboidGenerator>());
	std::shared_ptr<GraphicsObject> insLineCuboid = std::make_shared<GraphicsObject>();
	vaInsLineCuboid->SetObject(insLineCuboid);
	insLineCuboid->vertexArray = vaInsLineCuboid;
	insLineCuboid->primitive = GL_LINES;
	insLineCuboid->instances = 16;
	LineCuboidParams lcParams{};
	lcParams.width = 0.5f;
	lcParams.height = 0.5f;
	lcParams.depth = 0.5f;
	lcParams.color = { 0.0f, 0.0f, 1.0f };
	insLineCuboid->vertexArray->Generate(lcParams);
	insLineCuboid->vertexArray->SetAsDynamicGraphicsObject(8, 24);
	insLineCuboid->referenceFrame.SetPosition(glm::vec3(bpxParams.cpBezier[0][0]));
	allObjects["insLineCuboid"] = insLineCuboid;
	renderer->AddObject(insLineCuboid);
	renderer->SetObjectsVisibility(false);
}
