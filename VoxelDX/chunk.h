#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "mesh.h"

using namespace DirectX;

// TODO: Only half of Y blocks show up
#define CHUNK_SIZE XMFLOAT3 (16.0f, 16.0f, 16.0f)

enum class BlockFaces
{
	TOP,
	BOTTOM,
	FRONT,
	BACK,
	LEFT,
	RIGHT
};

struct Block
{
	bool isAir;
	bool isVisible;
	XMFLOAT3 colour;
	std::vector<BlockFaces> visibleBlockFaces;
};

class Chunk
{
protected:
	XMFLOAT3 position;
	Mesh mesh;
	Block blocks[(int)CHUNK_SIZE.x][(int)CHUNK_SIZE.y][(int)CHUNK_SIZE.z];
	std::vector<Vertex> chunkVertices;

	void setVerticesByPosition();
public:
	Chunk();
	Chunk(ID3D11Device* device, XMFLOAT3 position);

	void setVisibleBlocks();

	void buildMesh();

	void update(ID3D11DeviceContext* context, XMFLOAT4X4 view, XMFLOAT4X4 projection);
	void draw(ID3D11DeviceContext* context);
};

