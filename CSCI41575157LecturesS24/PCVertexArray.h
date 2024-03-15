#pragma once
#include "IVertexArray.h"
class PCVertexArray :  public IVertexArray
{
protected:
public:
	PCVertexArray();
	~PCVertexArray() = default;

	void RenderObject(std::shared_ptr<GraphicsObject> object) override;

	unsigned int AllocateVertexBuffer(
		unsigned int vao, std::shared_ptr<GraphicsObject> object) override;
	unsigned int AllocateIndexBuffer(
		unsigned int vao, std::shared_ptr<GraphicsObject> object) override;

	void EnableAttributes() override;

	void SendObjectUniforms(
		std::shared_ptr<GraphicsObject> object, std::shared_ptr<Shader> shader) override;

	void SetUpDynamicGraphicsObject(
		std::shared_ptr<GraphicsObject> object, 
		PCData& pcData, std::size_t maxVertexCount) override;

	void SetAsDynamicGraphicsObject(
		std::shared_ptr<GraphicsObject> object,
		std::size_t maxVertexCount) override;

	std::size_t GetVertexSize() override {
		return sizeof(VertexDataPC);
	}

};

