#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "GraphicsStructures.h"
#include "BoundingBox.h"

class IVertexArray; // Forward declaration
class Ray;

class GraphicsObject {
protected:
	std::shared_ptr<BoundingBox> boundingBox = nullptr;

public:
	std::shared_ptr<IVertexArray> vertexArray = nullptr;
	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int ibo = 0;
	bool isDynamic = false;
	bool isVisible = true;
	unsigned int shaderProgram = 0;
	unsigned int textureId = 0;
	unsigned int primitive = 0;
	unsigned int instances = 1;
	glm::mat4 referenceFrame = glm::mat4(1.0f);
	Material material{};

public:
	void SetPosition(glm::vec3 pos);
	void CreateBoundingBox(float width, float height, float depth);
	bool HasBoundingBox() const { return boundingBox != nullptr; }
	const std::shared_ptr<BoundingBox>& GetBoundingBox() const { 
		return boundingBox; 
	}
	bool IsIntersectingWithRay(const Ray& ray);
};
