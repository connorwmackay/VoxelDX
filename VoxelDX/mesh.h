#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>

#include "dxShader.h"
using namespace DirectX;

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 colour;
};

struct MeshConstants
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};

class Mesh
{
protected:
	XMFLOAT3 translation;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	DXShader shader;

	int verticesSize;

	bool isVerticesUpdateScheduled;
public:
	Mesh();
	Mesh(ID3D11Device* device, Vertex vertices[], unsigned int verticesSize, XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale);

	void setPoints(std::vector<Vertex> vertices, ID3D11DeviceContext* context);

	void update(ID3D11DeviceContext* context, XMFLOAT4X4 view, XMFLOAT4X4 projection);
	void draw(ID3D11DeviceContext* context);
};

