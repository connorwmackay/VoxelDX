#include "camera.h"

Camera::Camera() {}

Camera::Camera(XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale)
: translation(translation), rotation(rotation), scale(scale) {
}

XMFLOAT4X4 Camera::getProjectionMatrix(float fov, float width, float height) {
	float aspectRatio = width / height;
	float fovAngleY = fov * XM_PI / 180.0f;

	if (aspectRatio < 1.0f) {
		fovAngleY /= aspectRatio;
	}

	XMFLOAT4X4 projection;
	XMStoreFloat4x4(&projection, XMMatrixTranspose(XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, 0.1, 100.0f)));

	return projection;
}

XMFLOAT4X4 Camera::getViewMatrix() {
	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view, XMMatrixTranspose(XMMatrixIdentity()));

	XMMATRIX viewMat = XMLoadFloat4x4(&view);
	viewMat = XMMatrixMultiplyTranspose(viewMat, XMMatrixTranslation(translation.x, translation.y, translation.z));
	viewMat = XMMatrixInverse(NULL, viewMat);
	viewMat = XMMatrixMultiplyTranspose(viewMat, XMMatrixRotationX(XMConvertToRadians(rotation.x)));
	viewMat = XMMatrixMultiplyTranspose(viewMat, XMMatrixRotationY(XMConvertToRadians(rotation.y)));
	viewMat = XMMatrixMultiplyTranspose(viewMat, XMMatrixRotationZ(XMConvertToRadians(rotation.z)));
	viewMat = XMMatrixMultiplyTranspose(viewMat, XMMatrixScaling(scale.x, scale.y, scale.z));

	viewMat = (NULL, viewMat);
	XMStoreFloat4x4(&view, viewMat);

	return view;
}

void Camera::set(XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale) {
	this->translation = translation;
	this->rotation = rotation;
	this->scale = scale;
}

void Camera::move(XMVECTORF32 vec) {
	XMVECTOR translationVec = XMLoadFloat3(&translation);
	translationVec = XMVectorAdd(translationVec, vec);
	XMStoreFloat3(&translation, translationVec);
}

void Camera::moveTo(XMFLOAT3 position) {
	translation = position;
}

void Camera::rotateX(float angle) {
	float newAngle = rotation.x + angle;

	if (newAngle > 360.0f) {
		newAngle = static_cast<float>(fabs(360.0f - newAngle));
	} else if (newAngle < 0.0f) {
		newAngle = 360.0f + newAngle;
	}

	rotation.x = newAngle;
}

void Camera::rotateY(float angle) {
	float newAngle = rotation.y + angle;

	if (newAngle > 360.0f) {
		newAngle = static_cast<float>(fabs(360.0f - newAngle));
	}
	else if (newAngle < 0.0f) {
		newAngle = 360.0f + newAngle;
	}

	rotation.y = newAngle;
}

void Camera::rotateZ(float angle) {
	float newAngle = rotation.z + angle;

	if (newAngle > 360.0f) {
		newAngle = static_cast<float>(fabs(360.0f - newAngle));
	}
	else if (newAngle < 0.0f) {
		newAngle = 360.0f + newAngle;
	}

	rotation.z = newAngle;
}