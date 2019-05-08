#include "Transform.h"

Transform::Transform(Vector3 scale, Vector3 pos, Vector3 rot)
{
	_forward = Vector3(0.0f, 0.0f, 1.0f);

	_rotation.SetQuaternion(rot);
	_scale = scale;
	_pos = pos;

	_rotFlag = true;
}

Transform::Transform()
{
	_forward = Vector3(0.0f, 0.0f, 1.0f);
	_rotation.SetQuaternion(Vector3());
	_rotFlag = true;
}

Transform::~Transform()
{
}

void Transform::Update()
{
	XMMATRIX scaleMatrix = XMMatrixScaling(_scale.x, _scale.y, _scale.z);

	XMMATRIX rotMatrix;
	CalculateTransformMatrixRowMajor(rotMatrix, Vector3(), _rotation);
	//CalculateTransformMatrix(rotMatrix, Vector3(), _rotation);

	if (_pos != _oldPos)
		_pos.Round2D();
	else
		_pos.Round(1000);

	XMMATRIX positionMatrix = XMMatrixTranslation(_pos.x, _pos.y, _pos.z);

	XMVECTOR temp = XMVector3Transform(XMLoadFloat3(&ToF3(_forward)), rotMatrix);
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, temp);
	_forward = ToVec3(forward);

	XMStoreFloat4x4(&_world, scaleMatrix * rotMatrix * positionMatrix);
}

XMMATRIX Transform::GetRotationMatrix()
{
	XMMATRIX temp;
	CalculateTransformMatrixRowMajor(temp, Vector3(), _rotation);
	//CalculateTransformMatrix(temp, Vector3(), _rotation);
	return temp;
}

void Transform::ApplyRotation(Quaternion quaternion)
{
	_rotation *= quaternion;
	_rotFlag = true;
}