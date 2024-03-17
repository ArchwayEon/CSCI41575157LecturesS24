#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Ray
{
protected:
	glm::mat4 projView{};
	glm::mat4 projInverse{};
	glm::vec3 rayStart{};
	glm::vec3 rayStartW{};
	glm::vec3 rayEnd{};
	glm::vec3 rayEndW{};
public:
	Ray() = default;
	~Ray() = default;
	void Create(float screenX, float screenY);
};

