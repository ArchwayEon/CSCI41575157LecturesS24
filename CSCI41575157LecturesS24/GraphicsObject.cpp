#include "GraphicsObject.h"

void GraphicsObject::SetPosition(glm::vec3 pos)
{
	referenceFrame[3].x = pos.x;
	referenceFrame[3].y = pos.y;
	referenceFrame[3].z = pos.z;
}

void GraphicsObject::CreateBoundingBox(float width, float height, float depth)
{
	boundingBox = std::make_shared<BoundingBox>();
	boundingBox->Create(width, height, depth);
	boundingBox->SetReferenceFrame(referenceFrame);
}
