#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <cstdio>
#include <assert.h>
#include <fstream>

class DXShader
{
protected:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
public:
	DXShader();
	DXShader(ID3D11Device* device, const BYTE vertexShaderBytes[], size_t vertexShaderBytesSize, const BYTE pixelShaderBytes[], size_t pixelShaderBytesSize, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, size_t inputElemDescSize);

	ID3D11VertexShader* getVertexShader();
	ID3D11PixelShader* getPixelShader();
	ID3D11InputLayout* getInputLayout();
};
