#pragma once
#include "GraphicsStructures.h"
#include <vector>
#include <memory>
#include "VertexDataParams.h"

class IVertexGenerator
{
protected:
	std::vector<unsigned short> indexData;
public:
	IVertexGenerator() = default;
	virtual ~IVertexGenerator() = default;

	virtual std::size_t GetNumberOfVertices() = 0;
	virtual void GenerateVertices(IVertexDataParams& params) = 0;
	virtual void GenerateIndices() {}

	std::vector<unsigned short>& GetIndexData() {
		return indexData;
	}

	virtual std::vector<IVertexData>& GetVertexData() = 0;

	std::size_t GetNumberOfIndices() {
		return indexData.size();
	}

	long long GetIndexDataSize() {
		return indexData.size() * sizeof(unsigned short);
	}

	virtual long long GetVertexDataSize() = 0;

protected:
	virtual void GenerateLinesIndexDataConnected();
};

class PCCircleGenerator : public IVertexGenerator
{
protected:
	std::vector<VertexDataPC> vertexData;
public:
	PCCircleGenerator() : IVertexGenerator() {}
	~PCCircleGenerator() = default;

	std::size_t GetNumberOfVertices() override {
		return vertexData.size();
	}
	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices() override;

	std::vector<IVertexData>& GetVertexData() override {
		return reinterpret_cast<std::vector<IVertexData>&>(vertexData);
	}

	long long GetVertexDataSize() override {
		return vertexData.size() * sizeof(VertexDataPC);
	}
};