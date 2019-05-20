#pragma once
#include "Common.h"
#include "Quaternion.h"

class Transform
{
public:
	Transform(Vector3 scale, Vector3 pos, Vector3 rot);
	Transform();
	~Transform();

	void Update();

	XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4(&_world); }
	XMFLOAT4X4 GetWorldMatrix4X4() { return _world; }
	void SetWorldMatrix(XMFLOAT4X4 newMatrix) { _world = newMatrix; }
	void SetWorldMatrix(XMMATRIX newMatrix) { XMStoreFloat4x4(&_world, newMatrix); }

	//Setters and Getters for forward
	Vector3 GetForward() { return _forward; }
	void SetForward(Vector3 newVel) { _forward = newVel; }

	// Setters and Getters for position/rotation/scale
	Vector3 GetPosition() const { return _pos; }
	void SetPosition(Vector3 position) { _pos = position; }

	void SetOldPosition(Vector3 position) { _oldPos = position; }
	Vector3 GetOldPosition() const { return _oldPos; }

	Vector3 GetScale() const { return _scale; }
	void SetScale(Vector3 scale) { _scale = scale; }

	Quaternion GetRotation() { return _rotation; }
	XMMATRIX GetRotationMatrix();

	bool GetRotFlag() { return _rotFlag; }
	void SetRotFlag(bool flag) { _rotFlag = flag; }

	void ApplyRotation(Quaternion quaternion);
	void SetRotation(Quaternion newRot) { _rotation = newRot; }

private:
	XMFLOAT4X4 _world;
	Vector3 _forward, _pos, _oldPos, _scale;
	
	Quaternion _rotation;
	bool _rotFlag;
};