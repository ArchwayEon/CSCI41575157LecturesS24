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
	virtual void GenerateLinesIndexDataUnconnected();
};

class PCGenerator : public IVertexGenerator
{
protected:
	std::vector<VertexDataPC> vertexData;

public:
	PCGenerator() : IVertexGenerator() {}
	~PCGenerator() = default;

	std::size_t GetNumberOfVertices() override {
		return vertexData.size();
	}

	long long GetVertexDataSize() override {
		return vertexData.size() * sizeof(VertexDataPC);
	}

	std::vector<IVertexData>& GetVertexData() {
		return reinterpret_cast<std::vector<IVertexData>&>(vertexData);
	}
};

class PCCircleGenerator : public PCGenerator
{
protected:

public:
	PCCircleGenerator() : PCGenerator() {}
	~PCCircleGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices() override;
};

class PCSpirographGenerator : public PCGenerator
{
protected:

public:
	PCSpirographGenerator() : PCGenerator() {}
	~PCSpirographGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices() override;
};

class PCLinearBezierGenerator : public PCGenerator
{
protected:

public:
	PCLinearBezierGenerator() : PCGenerator() {}
	~PCLinearBezierGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices() override;
};

class PCQuadraticBezierGenerator : public PCGenerator
{
protected:

public:
	PCQuadraticBezierGenerator() : PCGenerator() {}
	~PCQuadraticBezierGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices() override;
};

class PCQuadraticBezierMGenerator : public PCGenerator
{
protected:

public:
	PCQuadraticBezierMGenerator() : PCGenerator() {}
	~PCQuadraticBezierMGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices() override;
};

class PCCubicBezierGenerator : public PCGenerator
{
protected:

public:
	PCCubicBezierGenerator() : PCGenerator() {}
	~PCCubicBezierGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices() override;
};
