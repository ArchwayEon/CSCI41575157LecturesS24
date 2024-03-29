#pragma once
#include <glm/glm.hpp>
class ReferenceFrame
{
protected:
	glm::mat4 matrix4;

public:
	ReferenceFrame() : matrix4(1.0f) {}
	void Reset() { matrix4 = glm::mat4(1.0f); }
	const glm::mat4& GetMatrix() const { return matrix4; }
	void SetMatrix4(glm::mat4 matrix4) { this->matrix4 = matrix4; }
	glm::mat4 GetInverseMatrix() const { return glm::inverse(matrix4); }
	void PointAt(const glm::vec3& target);
	glm::vec3 GetPosition() const { 
		return { matrix4[3].x, matrix4[3].y, matrix4[3].z }; 
	}
	glm::vec3 GetXAxis() const {
		return { matrix4[0].x, matrix4[0].y, matrix4[0].z };
	}
	glm::vec3 GetYAxis() const {
		return { matrix4[1].x, matrix4[1].y, matrix4[1].z };
	}
	glm::vec3 GetZAxis() const {
		return { matrix4[2].x, matrix4[2].y, matrix4[2].z };
	}
	void SetPosition(glm::vec3 position);
	void RotateX(float deltaDegrees);
	void RotateY(float deltaDegrees);
	void RotateZ(float deltaDegrees);
};

