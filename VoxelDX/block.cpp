#include "block.h"

#include "quadPixelShader.h"
#include "quadVertexShader.h"

Block::Block() {
	
}

Block::Block(ID3D11Device* device, XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale, XMFLOAT3 colour)
	: translation(translation), rotation(rotation), scale(scale) {
	// Setup the quad buffers
	HRESULT hRes;

	Vertex blockVertices[] = {
	{ XMFLOAT3(-0.5f,-0.5f,-0.5f), colour, },
	{ XMFLOAT3(-0.5f,-0.5f, 0.5f), colour, },
	{ XMFLOAT3(-0.5f, 0.5f,-0.5f), colour, },
	{ XMFLOAT3(-0.5f, 0.5f, 0.5f), colour, },

	{ XMFLOAT3(0.5f,-0.5f,-0.5f), colour, },
	{ XMFLOAT3(0.5f,-0.5f, 0.5f), colour, },
	{ XMFLOAT3(0.5f, 0.5f,-0.5f), colour, },
	{ XMFLOAT3(0.5f, 0.5f, 0.5f), colour, },
	};

	unsigned short blockIndices[] =
	{
		0,2,1, // -x
		1,2,3,

		4,5,6, // +x
		5,7,6,

		0,1,5, // -y
		0,5,4,

		2,6,7, // +y
		2,7,3,

		0,4,6, // -z
		0,6,2,

		1,3,7, // +z
		1,7,5,
	};

	CD3D11_BUFFER_DESC blockVBDesc(sizeof(blockVertices), D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA blockVBData;
	ZeroMemory(&blockVBData, sizeof(D3D11_SUBRESOURCE_DATA));
	blockVBData.pSysMem = blockVertices;
	blockVBData.SysMemSlicePitch = 0;
	blockVBData.SysMemSlicePitch = 0;

	hRes = device->CreateBuffer(&blockVBDesc, &blockVBData, &vertexBuffer);
	assert(SUCCEEDED(hRes));

	indicesCount = ARRAYSIZE(blockIndices);

	CD3D11_BUFFER_DESC blockIBDesc(sizeof(blockIndices), D3D11_BIND_INDEX_BUFFER);

	D3D11_SUBRESOURCE_DATA blockIBData;
	ZeroMemory(&blockIBData, sizeof(D3D11_SUBRESOURCE_DATA));
	blockIBData.pSysMem = blockIndices;
	blockIBData.SysMemPitch = 0;
	blockIBData.SysMemSlicePitch = 0;

	hRes = device->CreateBuffer(&blockIBDesc, &blockIBData, &indexBuffer);
	assert(SUCCEEDED(hRes));

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
	{
			"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
		},
	{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};

	CD3D11_BUFFER_DESC blockCBDesc(sizeof(BlockConstants), D3D11_BIND_CONSTANT_BUFFER);

	hRes = device->CreateBuffer(
		&blockCBDesc,
		nullptr,
		&constantBuffer
	);

	shader = DXShader(device, quadVertexShader, ARRAYSIZE(quadVertexShader), quadPixelShader, ARRAYSIZE(quadPixelShader), inputElementDesc, ARRAYSIZE(inputElementDesc));
}

void Block::update(ID3D11DeviceContext* context, XMFLOAT4X4 viewProjection) {
	BlockConstants blockConstants;
	blockConstants.viewProjection = viewProjection;

	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, XMMatrixIdentity());

	XMMATRIX worldFX = XMLoadFloat4x4(&world);
	worldFX = XMMatrixMultiply(worldFX, XMMatrixTranslation(translation.x, translation.y, translation.z));
	worldFX = XMMatrixMultiply(worldFX, XMMatrixRotationX(XMConvertToRadians(rotation.x)));
	worldFX = XMMatrixMultiply(worldFX, XMMatrixRotationY(XMConvertToRadians(rotation.y)));
	worldFX = XMMatrixMultiply(worldFX, XMMatrixRotationZ(XMConvertToRadians(rotation.z)));
	worldFX = XMMatrixMultiply(worldFX, XMMatrixScaling(scale.x, scale.y, scale.z));

	XMStoreFloat4x4(&world, worldFX);
	blockConstants.world = world;

	context->UpdateSubresource(
		constantBuffer,
		0,
		nullptr,
		&blockConstants,
		0,
		0
	);
}

void Block::draw(ID3D11DeviceContext* context) {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetInputLayout(shader.getInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(shader.getVertexShader(), nullptr, 0);
	context->VSSetConstantBuffers(0, 1, &constantBuffer);
	context->PSSetShader(shader.getPixelShader(), nullptr, 0);
	context->DrawIndexed(indicesCount, 0, 0);
}