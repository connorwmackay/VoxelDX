#include "mesh.h"

#include "quadPixelShader.h"
#include "quadVertexShader.h"

Mesh::Mesh() {
	
}

Mesh::Mesh(ID3D11Device* device, Vertex vertices[], unsigned int verticesSize, XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale)
	: translation(translation), rotation(rotation), scale(scale), isVerticesUpdateScheduled(false) {
	// Setup the quad buffers
	HRESULT hRes;

	this->vertices = std::vector<Vertex>(vertices, vertices + verticesSize);

	CD3D11_BUFFER_DESC blockVBDesc(this->vertices.size() * sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA meshVBData;
	ZeroMemory(&meshVBData, sizeof(D3D11_SUBRESOURCE_DATA));
	meshVBData.pSysMem = this->vertices.data();
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

	CD3D11_BUFFER_DESC meshCBDesc(sizeof(MeshConstants), D3D11_BIND_CONSTANT_BUFFER);

	hRes = device->CreateBuffer(
		&meshCBDesc,
		nullptr,
		constantBuffer.GetAddressOf()
	);

	shader = DXShader(device, quadVertexShader, ARRAYSIZE(quadVertexShader), quadPixelShader, ARRAYSIZE(quadPixelShader), inputElementDesc, ARRAYSIZE(inputElementDesc));
}

void Mesh::update(ID3D11DeviceContext* context, XMFLOAT4X4 viewProjection) {
	MeshConstants meshConstants{};
	meshConstants.viewProjection = viewProjection;

	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, XMMatrixIdentity());

	XMMATRIX worldFX = XMLoadFloat4x4(&world);
	worldFX = XMMatrixMultiply(worldFX, XMMatrixTranslation(translation.x, translation.y, translation.z));
	worldFX = XMMatrixMultiply(worldFX, XMMatrixRotationX(XMConvertToRadians(rotation.x)));
	worldFX = XMMatrixMultiply(worldFX, XMMatrixRotationY(XMConvertToRadians(rotation.y)));
	worldFX = XMMatrixMultiply(worldFX, XMMatrixRotationZ(XMConvertToRadians(rotation.z)));
	worldFX = XMMatrixMultiply(worldFX, XMMatrixScaling(scale.x, scale.y, scale.z));

	XMStoreFloat4x4(&world, worldFX);
	meshConstants.world = world;

	context->UpdateSubresource(
		constantBuffer.Get(),
		0,
		nullptr,
		&meshConstants,
		0,
		0
	);

	if (isVerticesUpdateScheduled) {
		isVerticesUpdateScheduled = false;
	}
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
	context->Draw(vertices.size(), 0);
}

void Mesh::addPoints(std::vector<Vertex> inVertices) {
	vertices.insert(vertices.end(), inVertices.begin(), inVertices.end());
	isVerticesUpdateScheduled = true;
}

void Mesh::setPoints(std::vector<Vertex> inVertices) {
	vertices = inVertices;
	isVerticesUpdateScheduled = true;
}

std::vector<Vertex> Mesh::getVertices() const {
	return vertices;
}