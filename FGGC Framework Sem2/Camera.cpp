#include "Camera.h"

Camera::Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
	: _eye(position), _at(at), _up(up), _windowWidth(windowWidth), _windowHeight(windowHeight), _nearDepth(nearDepth), _farDepth(farDepth)
{
	_originalEye = _eye;
	_parent = nullptr;
	Update();
}

Camera::~Camera()
{
}

void Camera::Update()
{
	if (_parent)
	{
		Vector3 pos = _parent->GetTransform()->GetPosition();
		Vector3 pForward = _parent->GetTransform()->GetForward();

		Vector3 offset = { pForward.x * -_originalEye.z, _originalEye.y, pForward.z * _originalEye.z };

		XMFLOAT3 oldEye = _eye;
		XMFLOAT3 newEye = { pos.x + offset.x,  pos.y + offset.y, pos.z + offset.z };

		_at = { pos.x,  pos.y, pos.z };

		XMStoreFloat3(&_eye, (XMVectorLerp(XMLoadFloat3(&newEye), XMLoadFloat3(&oldEye), 0.01f)));
	}

    // Initialize the view matrix
	XMFLOAT4 eye = XMFLOAT4(_eye.x, _eye.y, _eye.z, 1.0f);
	XMFLOAT4 at = XMFLOAT4(_at.x, _at.y, _at.z, 1.0f);
	XMFLOAT4 up = XMFLOAT4(_up.x, _up.y, _up.z, 0.0f);

	XMVECTOR EyeVector = XMLoadFloat4(&eye);
	XMVECTOR AtVector = XMLoadFloat4(&at);
	XMVECTOR UpVector = XMLoadFloat4(&up);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(EyeVector, AtVector, UpVector));

    // Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(0.25f * XM_PI, _windowWidth / _windowHeight, _nearDepth, _farDepth));
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
}

XMFLOAT4X4 Camera::GetViewProjection() const 
{ 
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);

	XMFLOAT4X4 viewProj;

	XMStoreFloat4x4(&viewProj, view * projection);

	return viewProj;
}