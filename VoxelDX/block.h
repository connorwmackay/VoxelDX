#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "dxShader.h"
using namespace DirectX;

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 colour;
};

typedef struct BlockConstants
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 viewProjection;
};

class Block
{
protected:
	XMFLOAT3 translation;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constantBuffer;
	DXShader shader;

	int indicesCount;
public:
	Block();
	Block(ID3D11Device* device, XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale, XMFLOAT3 colour);

	void update(ID3D11DeviceContext* context, XMFLOAT4X4 viewProjection);
	void draw(ID3D11DeviceContext* context);
};

