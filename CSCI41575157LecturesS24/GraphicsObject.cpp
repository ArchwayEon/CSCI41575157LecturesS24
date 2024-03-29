#include "GraphicsObject.h"
#include "Ray.h"

void GraphicsObject::SetPosition(glm::vec3 pos)
{
	referenceFrame.SetPosition(pos);
}

void GraphicsObject::CreateBoundingBox(float width, float height, float depth)
{
	boundingBox = std::make_shared<BoundingBox>();
	boundingBox->Create(width, height, depth);
	boundingBox->SetReferenceFrame(referenceFrame);
}

bool GraphicsObject::IsIntersectingWithRay(const Ray& ray)
{
	if (boundingBox == nullptr) return false;
	boundingBox->SetReferenceFrame(referenceFrame);
	return boundingBox->IsIntersectingWithRay(ray);
}
