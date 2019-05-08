#pragma once

#include <directxmath.h>
#include <d3d11_1.h>

using namespace DirectX;

class Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);

	~Vector3();

	//Operator Overloading
	////////////////////////////////////////////////////////
	//addition
	Vector3 operator+(const Vector3& v1);
	Vector3 operator+=(const Vector3& v1);

	//negative
	Vector3 operator-();

	//subtract
	Vector3 operator-(const Vector3& v1);
	Vector3 operator-=(const Vector3& v1);

	//multiply
	Vector3 operator*(const Vector3& v1);
	Vector3 operator*(const float& scalar);

	//divide
	Vector3 operator/(const Vector3& v1);
	Vector3 operator/(const float& scalar);

	//Squared by scalar
	Vector3 operator^(const float& scalar);

	//Vector Manipulation
	////////////////////////////////////////////////////////
	Vector3 Normalization(const Vector3& v1);
	float Magnitude(const Vector3& v1);
	Vector3 Cross(const Vector3& v1, const Vector3& v2);
	float Dot(const Vector3& v1, const Vector3& v2);
	float Dist(const Vector3& v1, const Vector3& v2);
	void Zero() { x = y = z = 0; }

	float x, y, z;
};