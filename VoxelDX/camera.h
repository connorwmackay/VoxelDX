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

	XMFLOAT4X4 getViewPerspectiveMatrix(float fov, float width, float height);

	void set(XMVECTOR eye, XMVECTOR at, XMVECTOR up);
};
