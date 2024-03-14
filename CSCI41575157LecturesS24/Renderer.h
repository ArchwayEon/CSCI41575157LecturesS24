#pragma once
#include "IVertexArray.h"
#include <memory>

class Renderer
{
private:
	std::shared_ptr<IVertexArray> va;

public:
	Renderer(std::shared_ptr<IVertexArray> va);

	void Render();
};

