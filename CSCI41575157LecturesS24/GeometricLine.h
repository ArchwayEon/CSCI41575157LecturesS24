#pragma once
#include <glm/glm.hpp>

// Vince, J. (2014). Mathematics for Computer Graphics, Springer

class GeometricLine
{
protected:
	glm::vec3 direction{};
	glm::vec3 startPoint{};

public:
	GeometricLine();
	~GeometricLine() = default;

	void SetDirection(glm::vec3 direction) {
		this->direction = glm::normalize(direction);
	}

	void SetDirection(glm::vec3 start, glm::vec3 end);

	const glm::vec3& GetDirection() const { return direction; }

	void SetStartPoint(glm::vec3 startPoint) {
		this->startPoint = startPoint;
	}

	const glm::vec3& GetStartPoint() const { return startPoint; }

	glm::vec3 GetPosition(float offset) const;
};

