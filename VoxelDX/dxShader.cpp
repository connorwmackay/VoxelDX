#include "dxShader.h"

DXShader::DXShader() {
	
}

DXShader::DXShader(ID3D11Device* device, const BYTE vertexShaderBytes[], size_t vertexShaderBytesSize, const BYTE pixelShaderBytes[], size_t pixelShaderBytesSize, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, size_t inputElemDescSize) {
	HRESULT hResult;

	hResult = device->CreateVertexShader(
		vertexShaderBytes,
		vertexShaderBytesSize,
		nullptr,
		&vertexShader
	);

	assert(SUCCEEDED(hResult));

	hResult = device->CreateInputLayout(
		inputElementDesc,
		inputElemDescSize,
		vertexShaderBytes,
		vertexShaderBytesSize,
		&inputLayout
	);

	assert(SUCCEEDED(hResult));

	hResult = device->CreatePixelShader(
		pixelShaderBytes,
		pixelShaderBytesSize,
		nullptr,
		&pixelShader
	);

	assert(SUCCEEDED(hResult));
}

ID3D11VertexShader* DXShader::getVertexShader() {
	return vertexShader;
}

ID3D11PixelShader* DXShader::getPixelShader() {
	return pixelShader;
}

ID3D11InputLayout* DXShader::getInputLayout() {
	return inputLayout;
}