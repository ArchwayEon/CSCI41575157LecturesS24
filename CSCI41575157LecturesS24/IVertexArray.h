#pragma once
#include <memory>
#include "GraphicsStructures.h"
#include "Shader.h"
#include "VertexGenerators.h"
//#include "GraphicsObject.h"

class GraphicsObject;

class IVertexArray
{
protected:
	std::shared_ptr<IVertexGenerator> generator = nullptr;
	std::shared_ptr<GraphicsObject> object = nullptr;
	std::vector<unsigned short> indexData;
	int numberOfVertices = 0;
	int numberOfIndices = 0;

public:
	IVertexArray();
	virtual ~IVertexArray() = default;

	void SetGenerator(std::shared_ptr<IVertexGenerator> generator) {
		this->generator = generator;
	}
	std::shared_ptr<IVertexGenerator>& GetGenerator() {
		return generator;
	}
	void SetObject(std::shared_ptr<GraphicsObject> object) {
		this->object = object;
	}

	virtual void RenderObject() = 0;

	virtual unsigned int AllocateVertexBuffer(unsigned int vao) = 0;
	virtual unsigned int AllocateIndexBuffer(unsigned int vao) {
		return 0;
	};

	virtual void EnableAttributes() = 0;

	virtual void SendObjectUniforms(std::shared_ptr<Shader> shader) = 0;

	virtual void SetUpDynamicGraphicsObject(
		std::shared_ptr<GraphicsObject> object, PCData& pcData, 
		std::size_t maxVertexCount) {};
	virtual void SetAsDynamicGraphicsObject(
		std::shared_ptr<GraphicsObject> object, long long maxVertexCount) {};

	virtual void Generate(IVertexDataParams& params);

	virtual std::size_t GetNumberOfVertices() {
		if (generator != nullptr) {
			return generator->GetNumberOfVertices();
		}
		return 0;
	}

	virtual std::size_t GetNumberOfIndices() {
		return indexData.size();
	}

	virtual long long GetVertexDataSize() {
		if (generator != nullptr) {
			return generator->GetVertexDataSize();
		}
		return 0;
	}

	virtual long long GetIndexDataSize() {
		return indexData.size() * sizeof(unsigned short);
	}

	virtual std::size_t GetVertexSize() {
		return 0;
	}

	virtual std::size_t GetIndexSize() {
		return sizeof(unsigned short);
	}


	virtual std::vector<IVertexData>& GetVertexData() {
		return generator->GetVertexData();
	}

	virtual std::vector<unsigned short>& GetIndexData() {
		return indexData;
	}

protected:
	void EnableAttribute(
		int attribIndex, int elementCount, int sizeInBytes, void* offset);
};

