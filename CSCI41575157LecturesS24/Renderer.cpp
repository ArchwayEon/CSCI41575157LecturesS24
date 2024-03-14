#include "Renderer.h"

Renderer::Renderer(std::shared_ptr<IVertexArray> va)
{
	this->va = va;
}

void Renderer::Render()
{
	va->Render();
}
