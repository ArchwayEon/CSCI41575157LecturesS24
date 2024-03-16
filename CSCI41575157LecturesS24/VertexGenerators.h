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
	virtual void GenerateIndices(int type=1, int steps=0) {}

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
	virtual void GenerateLinesIndexDataForBezierSurface(int steps);
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
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCSpirographGenerator : public PCGenerator
{
protected:

public:
	PCSpirographGenerator() : PCGenerator() {}
	~PCSpirographGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCLinearBezierGenerator : public PCGenerator
{
protected:

public:
	PCLinearBezierGenerator() : PCGenerator() {}
	~PCLinearBezierGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCQuadraticBezierGenerator : public PCGenerator
{
protected:

public:
	PCQuadraticBezierGenerator() : PCGenerator() {}
	~PCQuadraticBezierGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCQuadraticBezierMGenerator : public PCGenerator
{
protected:

public:
	PCQuadraticBezierMGenerator() : PCGenerator() {}
	~PCQuadraticBezierMGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCCubicBezierGenerator : public PCGenerator
{
protected:

public:
	PCCubicBezierGenerator() : PCGenerator() {}
	~PCCubicBezierGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCCubicBezierMGenerator : public PCGenerator
{
protected:

public:
	PCCubicBezierMGenerator() : PCGenerator() {}
	~PCCubicBezierMGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCBezierPatchGenerator : public PCGenerator
{
protected:

public:
	PCBezierPatchGenerator() : PCGenerator() {}
	~PCBezierPatchGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCLineCuboidGenerator : public PCGenerator
{
protected:

public:
	PCLineCuboidGenerator() : PCGenerator() {}
	~PCLineCuboidGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCNTGenerator : public IVertexGenerator
{
protected:
	std::vector<VertexDataPCNT> vertexData;

public:
	PCNTGenerator() : IVertexGenerator() {}
	~PCNTGenerator() = default;

	std::size_t GetNumberOfVertices() override {
		return vertexData.size();
	}

	long long GetVertexDataSize() override {
		return vertexData.size() * sizeof(VertexDataPCNT);
	}

	std::vector<IVertexData>& GetVertexData() {
		return reinterpret_cast<std::vector<IVertexData>&>(vertexData);
	}

	std::vector<VertexDataPCNT>& GetPCNTVertexData() {
		return vertexData;
	}
};

class PCNTCuboidGenerator : public PCNTGenerator
{
protected:

public:
	PCNTCuboidGenerator() : PCNTGenerator() {}
	~PCNTCuboidGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCNTXZPlaneGenerator : public PCNTGenerator
{
protected:

public:
	PCNTXZPlaneGenerator() : PCNTGenerator() {}
	~PCNTXZPlaneGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCTGenerator : public IVertexGenerator
{
protected:
	std::vector<VertexDataPCT> vertexData;

public:
	PCTGenerator() : IVertexGenerator() {}
	~PCTGenerator() = default;

	std::size_t GetNumberOfVertices() override {
		return vertexData.size();
	}

	long long GetVertexDataSize() override {
		return vertexData.size() * sizeof(VertexDataPCT);
	}

	std::vector<IVertexData>& GetVertexData() {
		return reinterpret_cast<std::vector<IVertexData>&>(vertexData);
	}

	std::vector<VertexDataPCT>& GetPCNTVertexData() {
		return vertexData;
	}
};

class PCTXYPlaneGenerator : public PCTGenerator
{
protected:

public:
	PCTXYPlaneGenerator() : PCTGenerator() {}
	~PCTXYPlaneGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};

class PCIGenerator : public IVertexGenerator
{
protected:
	std::vector<VertexDataPCI> vertexData;

public:
	PCIGenerator() : IVertexGenerator() {}
	~PCIGenerator() = default;

	std::size_t GetNumberOfVertices() override {
		return vertexData.size();
	}

	long long GetVertexDataSize() override {
		return vertexData.size() * sizeof(VertexDataPCI);
	}

	std::vector<IVertexData>& GetVertexData() {
		return reinterpret_cast<std::vector<IVertexData>&>(vertexData);
	}
};

class PCILineCuboidGenerator : public PCIGenerator
{
protected:
	std::vector<glm::vec3> worldPositions;
public:
	PCILineCuboidGenerator(std::vector<glm::vec3> worldPositions) : 
		PCIGenerator() {
		this->worldPositions = worldPositions;
	}
	~PCILineCuboidGenerator() = default;

	void GenerateVertices(IVertexDataParams& params) override;
	void GenerateIndices(int type = 1, int steps = 0) override;
};
