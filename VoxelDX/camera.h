#pragma once
#include <DirectXMath.h>
#include <complex>
using namespace DirectX;

class Camera
{
protected:
	XMFLOAT3 translation;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
public:
	Camera();
	Camera(XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale);

	XMFLOAT4X4 getProjectionMatrix(float fov, float width, float height);
	XMFLOAT4X4 getViewMatrix();

	void set(XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale);

	/*
	 * Move the camera from it's current position to by
	 * the vector parameter provided.
	 */
	void move(XMVECTORF32 vec);

	/*
	 * Move the camera directly to the position provided
	 * by the position parameter.
	 */
	void moveTo(XMFLOAT3 position);

	/*
	 * Increase the rotation of the camera on the relevant
	 * axis by the angle provided as a parameter.
	 */
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);
};
