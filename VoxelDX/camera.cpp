#include "camera.h"

Camera::Camera() {}

Camera::Camera(XMVECTOR eye, XMVECTOR at, XMVECTOR up)
	: eye(eye), at(at), up(up) {
	
}

XMFLOAT4X4 Camera::getViewPerspectiveMatrix(float fov, float width, float height) {
	float aspectRatio = width / height;
	float fovAngleY = fov * XM_PI / 180.0f;

	if (aspectRatio < 1.0f) {
		fovAngleY /= aspectRatio;
	}

	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));

	XMFLOAT4X4 projection;
	XMStoreFloat4x4(&projection, XMMatrixTranspose(XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, 0.1, 100.0f)));

	FXMMATRIX viewFX = XMLoadFloat4x4(&view);
	FXMMATRIX projectionFX = XMLoadFloat4x4(&projection);

	XMFLOAT4X4 viewProjection;
	// Determine the correct multiplication order for this...
	XMStoreFloat4x4(&viewProjection, XMMatrixMultiply(projectionFX, viewFX));

	return viewProjection;
}

void Camera::set(XMVECTOR eye, XMVECTOR at, XMVECTOR up) {
	this->eye = eye;
	this->at = at;
	this->up = up;
}