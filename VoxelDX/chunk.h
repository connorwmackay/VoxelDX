#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "mesh.h"

using namespace DirectX;

#define CHUNK_SIZE XMFLOAT3 (160.0f, 160.0f, 160.0f)
#define VOXEL_SIZE 0.1f;

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
	Block* blocks;

	std::vector<Vertex> setVerticesByPosition();
	int getBlocksIndex(int x, int y, int z);
public:
	Chunk();
	Chunk(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT3 position);

	void setVisibleBlocks();

	void buildMesh(ID3D11DeviceContext* context);

	void update(ID3D11DeviceContext* context, XMFLOAT4X4 view, XMFLOAT4X4 projection);
	void draw(ID3D11DeviceContext* context);
};

