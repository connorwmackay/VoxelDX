#include "mesh.h"

#include "quadPixelShader.h"
#include "quadVertexShader.h"

Mesh::Mesh() {
	
}

Mesh::Mesh(ID3D11Device* device, Vertex vertices[], unsigned int verticesSize, XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale)
	: translation(translation), rotation(rotation), scale(scale), isVerticesUpdateScheduled(false) {
	// Setup the quad buffers
	HRESULT hRes;

	D3D11_BUFFER_DESC blockVBDesc;
	ZeroMemory(&blockVBDesc, sizeof(D3D11_BUFFER_DESC));
	blockVBDesc.ByteWidth = verticesSize * sizeof(Vertex);
	blockVBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	blockVBDesc.Usage = D3D11_USAGE_DYNAMIC;
	blockVBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	blockVBDesc.MiscFlags = 0;
	blockVBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA meshVBData;
	ZeroMemory(&meshVBData, sizeof(D3D11_SUBRESOURCE_DATA));
	meshVBData.pSysMem = vertices;
	meshVBData.SysMemSlicePitch = 0;
	meshVBData.SysMemSlicePitch = 0;

	hRes = device->CreateBuffer(&blockVBDesc, &meshVBData, vertexBuffer.GetAddressOf());
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

	D3D11_BUFFER_DESC meshCBDesc;
	ZeroMemory(&meshCBDesc, sizeof(D3D11_BUFFER_DESC));
	meshCBDesc.ByteWidth = sizeof(MeshConstants);
	meshCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	meshCBDesc.Usage = D3D11_USAGE_DYNAMIC;
	meshCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	meshCBDesc.MiscFlags = 0;
	meshCBDesc.StructureByteStride = 0;

	hRes = device->CreateBuffer(
		&meshCBDesc,
		nullptr,
		constantBuffer.GetAddressOf()
	);

	shader = DXShader(device, quadVertexShader, ARRAYSIZE(quadVertexShader), quadPixelShader, ARRAYSIZE(quadPixelShader), inputElementDesc, ARRAYSIZE(inputElementDesc));
	this->verticesSize = verticesSize;
}

void Mesh::update(ID3D11DeviceContext* context, XMFLOAT4X4 view, XMFLOAT4X4 projection) {
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, XMMatrixIdentity());

	XMMATRIX worldFX = XMLoadFloat4x4(&world);
	worldFX = XMMatrixMultiplyTranspose(worldFX, XMMatrixTranslation(translation.x, translation.y, translation.z));
	worldFX = XMMatrixMultiplyTranspose(worldFX, XMMatrixRotationX(XMConvertToRadians(rotation.x)));
	worldFX = XMMatrixMultiplyTranspose(worldFX, XMMatrixRotationY(XMConvertToRadians(rotation.y)));
	worldFX = XMMatrixMultiplyTranspose(worldFX, XMMatrixRotationZ(XMConvertToRadians(rotation.z)));
	worldFX = XMMatrixMultiplyTranspose(worldFX, XMMatrixScaling(scale.x, scale.y, scale.z));

	XMStoreFloat4x4(&world, worldFX);

	// Mesh Constants
	MeshConstants meshConstants{};
	meshConstants.view = view;
	meshConstants.projection = projection;
	meshConstants.world = world;

	D3D11_MAPPED_SUBRESOURCE mappedCB;
	ZeroMemory(&mappedCB, sizeof(D3D11_MAPPED_SUBRESOURCE));
	context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCB);
	memcpy(mappedCB.pData, &meshConstants, sizeof(MeshConstants));
	context->Unmap(constantBuffer.Get(), 0);

	context->UpdateSubresource(
		constantBuffer.Get(),
		0,
		nullptr,
		&meshConstants,
		0,
		0
	);
}

void Mesh::draw(ID3D11DeviceContext* context) {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetInputLayout(shader.getInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(shader.getVertexShader(), nullptr, 0);
	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	context->PSSetShader(shader.getPixelShader(), nullptr, 0);
	context->Draw(verticesSize, 0);
}

void Mesh::setPoints(std::vector<Vertex> vertices, ID3D11DeviceContext* context) {
	D3D11_MAPPED_SUBRESOURCE mappedVB;
	ZeroMemory(&mappedVB, sizeof(D3D11_MAPPED_SUBRESOURCE));

	context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
	memcpy(mappedVB.pData, vertices.data(), sizeof(Vertex) * vertices.size());
	context->Unmap(vertexBuffer.Get(), 0);
	verticesSize = vertices.size();
}