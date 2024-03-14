#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Material {
	float ambientIntensity;  // 0 to 1
	float specularIntensity; // 0 to 1
	float shininess;         // 0 to infinity
};

struct Light {
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
	float attenuationCoef;
};

struct SphericalCoordinate {
	float phi = 0.0f, theta = 0.0f, rho = 1.0f;

	glm::mat4 ToMat4() {
		float thetaRadians = glm::radians(theta);
		float phiRadians = glm::radians(phi);
		float sinPhi = sin(phiRadians);
		glm::vec3 zAxis{};
		zAxis.x = rho * sin(thetaRadians) * sinPhi;
		zAxis.y = rho * cos(phiRadians);
		zAxis.z = rho * cos(thetaRadians) * sinPhi;

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 xAxis = glm::normalize(glm::cross(up, zAxis));
		glm::vec3 yAxis = glm::cross(zAxis, xAxis);
		glm::mat4 orientation(1.0f);
		orientation[0] = glm::vec4(xAxis, 0.0f);
		orientation[1] = glm::vec4(yAxis, 0.0f);
		orientation[2] = glm::vec4(zAxis, 0.0f);
		return orientation;
	}
};

struct MouseParams {
	SphericalCoordinate spherical{};
	double x = 0, y = 0;
	double windowX = 0, windowY = 0;
	int windowWidth = 0, windowHeight = 0;
	float fieldOfView = 60.0f;
};

struct Result {
	bool isSuccess;
	std::string message;

	Result() : isSuccess(true) {}
};

struct VertexDataPC {
	glm::vec3 position{};
	glm::vec3 color{};
};

struct VertexDataPCT {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 tex;
};

struct VertexDataPCNT {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 normal;
	glm::vec2 tex;
};

struct PCData {
	std::vector<VertexDataPC> vertexData;
	std::vector<unsigned short> indexData;
};

struct GraphicsObject {
	VertexDataPCNT* vertexDataPCNT = nullptr;
	VertexDataPCT* vertexDataPCT = nullptr;
	std::vector<VertexDataPC> vertexDataPC;
	std::vector<unsigned short> indexData;
	std::size_t sizeOfVertexBuffer = 0;
	std::size_t maxSizeOfVertexBuffer = 0;
	std::size_t sizeOfIndexBuffer = 0;
	std::size_t maxSizeOfIndexBuffer = 0;
	std::size_t numberOfVertices = 0;
	std::size_t numberOfIndices = 0;
	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int ibo = 0;
	bool isDynamic = false;
	bool isVisible = true;
	unsigned int shaderProgram = 0;
	unsigned int textureId = 0;
	unsigned int primitive = 0;
	glm::mat4 referenceFrame = glm::mat4(1.0f);
	Material material{};
};

struct LightingShaderLocation {
	unsigned int worldLoc = 0;
	unsigned int projectionLoc = 0;
	unsigned int viewLoc = 0;
	unsigned int materialAmbientLoc = 0;
	unsigned int materialSpecularLoc = 0;
	unsigned int materialShininessLoc = 0;
	unsigned int globalLightPositionLoc = 0;
	unsigned int globalLightColorLoc = 0;
	unsigned int globalLightIntensityLoc = 0;
	unsigned int localLightPositionLoc = 0;
	unsigned int localLightColorLoc = 0;
	unsigned int localLightIntensityLoc = 0;
	unsigned int localLightAttenuationLoc = 0;
	unsigned int viewPositionLoc = 0;
};

struct BasicShaderLocation {
	unsigned int worldLoc = 0;
	unsigned int projectionLoc = 0;
	unsigned int viewLoc = 0;
};
