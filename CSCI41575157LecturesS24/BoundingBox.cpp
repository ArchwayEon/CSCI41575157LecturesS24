#include "BoundingBox.h"
#include "Ray.h"

BoundingBox::BoundingBox() : referenceFrame(1.0f)
{
	Create(1.0f, 1.0f, 1.0f);
}

void BoundingBox::Create(float width, float height, float depth)
{
	this->width = width;
	this->height = height;
	this->depth = depth;
	float hw = this->width / 2;
	float hh = this->height / 2;
	float hd = this->depth / 2;
	glm::vec3 xAxis = glm::vec3(referenceFrame[0]);
	glm::vec3 yAxis = glm::vec3(referenceFrame[1]);
	glm::vec3 zAxis = glm::vec3(referenceFrame[2]);
	planes[FRONT].Set(-zAxis, hd);
	planes[BACK].Set(zAxis, hd);
	planes[RIGHT].Set(-xAxis, hw);
	planes[LEFT].Set(xAxis, hw);
	planes[TOP].Set(-yAxis, hh);
	planes[BOTTOM].Set(yAxis, hh);
}

std::vector<Intersection> BoundingBox::GetRayIntersections(const Ray& ray) const
{
	std::vector<Intersection> intersections;
	Intersection intersection;
	Ray localRay;
	glm::vec3 localStart = glm::vec3(
		invReferenceFrame * glm::vec4(ray.GetStart(), 1.0f));
	localRay.SetStart(localStart);
	glm::vec3 localDir = glm::vec3(
		invReferenceFrame * glm::vec4(ray.GetDirection(), 1.0f));
	localRay.SetDirection(localDir);
	for (int i = 0; i < 6; i++) {
		intersection = localRay.GetIntersectionWithPlane(planes[i]);
		if (intersection.isIntersecting) {
			intersections.push_back(intersection);
		}
	}
	return intersections;
}
