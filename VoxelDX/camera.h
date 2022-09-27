#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
protected:
	XMVECTOR eye;
	XMVECTOR at;
	XMVECTOR up;
public:
	Camera();
	Camera(XMVECTOR eye, XMVECTOR at, XMVECTOR up);

	XMFLOAT4X4 getProjectionMatrix(float fov, float width, float height);
	XMFLOAT4X4 getViewMatrix();

	void set(XMVECTOR eye, XMVECTOR at, XMVECTOR up);
};
