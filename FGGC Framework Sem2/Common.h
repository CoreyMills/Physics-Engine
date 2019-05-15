#pragma once

#include <directxmath.h>
#include <d3d11_1.h>
#include <vector>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 PosL;
	XMFLOAT3 NormL;
	XMFLOAT2 Tex;
};

struct SurfaceInfo
{
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 SpecularMtrl;
};

struct Light
{
	XMFLOAT4 AmbientLight;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 SpecularLight;

	float SpecularPower;
	XMFLOAT3 LightVecW;
};

struct Geometry
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numberOfIndices;

	UINT vertexBufferStride;
	UINT vertexBufferOffset;
};

struct Material
{
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
	float specularPower;
};

struct ConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;

	SurfaceInfo surface;

	Light light;

	XMFLOAT3 EyePosW;
	float HasTexture;
};

struct Vector2
{
	float x, y;

	Vector2::Vector2()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vector2::Vector2(float value)
	{
		this->x = value;
		this->y = value;
	}

	Vector2::Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
};

struct Vector3
{
	float x, y, z;

	Vector3::Vector3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vector3::Vector3(float value)
	{
		this->x = value;
		this->y = value;
		this->z = value;
	}

	Vector3::Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	//Operator Overloading
	////////////////////////////////////////////////////////
	bool Vector3::operator==(const Vector3& v1)
	{
		return (this->x == v1.x && this->y == v1.y && this->z == v1.z);
	}

	Vector3 Vector3::operator+(const Vector3& v1)
	{
		Vector3 v2;
		v2.x = this->x + v1.x;
		v2.y = this->y + v1.y;
		v2.z = this->z + v1.z;

		return v2;
	}

	Vector3 Vector3::operator+=(const Vector3& v1)
	{
		this->x += v1.x;
		this->y += v1.y;
		this->z += v1.z;

		return *this;
	}

	//Using XMFLOAT3
	Vector3 Vector3::operator+(const XMFLOAT3& v1)
	{
		Vector3 v2;
		v2.x = this->x + v1.x;
		v2.y = this->y + v1.y;
		v2.z = this->z + v1.z;

		return v2;
	}

	Vector3 Vector3::operator+=(const XMFLOAT3& v1)
	{
		this->x += v1.x;
		this->y += v1.y;
		this->z += v1.z;

		return *this;
	}

	Vector3 Vector3::operator-(const Vector3& v1)
	{
		Vector3 v2;
		v2.x = this->x - v1.x;
		v2.y = this->y - v1.y;
		v2.z = this->z - v1.z;

		return v2;
	}

	Vector3 Vector3::operator-=(const Vector3& v1)
	{
		this->x -= v1.x;
		this->y -= v1.y;
		this->z -= v1.z;

		return *this;
	}

	Vector3 Vector3::operator*(const Vector3& v1)
	{
		Vector3 v2;
		v2.x = this->x * v1.x;
		v2.y = this->y * v1.y;
		v2.z = this->z * v1.z;

		return v2;
	}

	Vector3 Vector3::operator*(const float& scalar)
	{
		Vector3 v2;
		v2.x = this->x * scalar;
		v2.y = this->y * scalar;
		v2.z = this->z * scalar;

		return v2;
	}

	Vector3 Vector3::operator*(const int& scalar)
	{
		Vector3 v2;
		v2.x = this->x * scalar;
		v2.y = this->y * scalar;
		v2.z = this->z * scalar;

		return v2;
	}

	Vector3 Vector3::operator*(const XMMATRIX matrix)
	{
		XMVECTOR temp = XMVector3Transform(XMVectorSet(this->x, this->y, this->z, 1.0f), matrix);
		XMFLOAT4 temp1;
		XMStoreFloat4(&temp1, temp);
		return Vector3(temp1.x, temp1.y, temp1.z);
	}

	Vector3 Vector3::operator/(const Vector3& v1)
	{
		Vector3 v2;
		v2.x = this->x / v1.x;
		v2.y = this->y / v1.y;
		v2.z = this->z / v1.z;

		return v2;
	}

	Vector3 Vector3::operator/(const float& scalar)
	{
		Vector3 v2;
		v2.x = this->x / scalar;
		v2.y = this->y / scalar;
		v2.z = this->z / scalar;

		return v2;
	}

	Vector3 Vector3::operator/=(const float& scalar)
	{
		this->x /= scalar;
		this->y /= scalar;
		this->z /= scalar;

		return *this;
	}

	Vector3 Vector3::operator^(const float & scalar)
	{
		for (int i = 1; i < scalar; i++)
		{
			*this = *this * *this;
		}
		return *this;
	}

	Vector3 Vector3::operator-()
	{
		return { -this->x, -this->y, -this->z };
	}

	//Vector Manipulation
	////////////////////////////////////////////////////////
	float Vector3::Dot(const Vector3& v2)
	{
		return ((this->x*v2.x) + (this->y*v2.y) + (this->z*v2.z));
	}

	Vector3 Vector3::Cross(const Vector3& v2)
	{
		float tempX = this->y * v2.z - v2.y * this->z;
		float tempY = v2.x * this->z - this->x * v2.z;
		float tempZ = this->x * v2.y - v2.x * this->y;

		return Vector3(tempX, tempY, tempZ);
	}

	Vector3 Vector3::Normalise()
	{
		return Vector3(this->x, this->y, this->z) / this->Length();
	}

	float Vector3::Length()
	{
		return sqrtf((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
	}

	float Vector3::LengthSq()
	{
		return (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
	}

	float Vector3::Dist(const Vector3 & v2)
	{
		return sqrtf(((this->x - v2.x) * (this->x - v2.x)) +
					((this->y - v2.y) * (this->y - v2.y)) +
					((this->z - v2.z) * (this->z - v2.z)));
	}

	float Vector3::DistSq(const Vector3 & v2)
	{
		return ((this->x - v2.x) * (this->x - v2.x)) +
				((this->y - v2.y) * (this->y - v2.y)) +
				((this->z - v2.z) * (this->z - v2.z));
	}

	float Vector3::AbsMaxAxis()
	{
		return max(max(abs(this->x), abs(this->y)), abs(this->z));
	}

	void Vector3::Round()
	{
		this->x = roundf(this->x);
		this->y = roundf(this->y);
		this->z = roundf(this->z);
	}

	void Vector3::Round1D()
	{
		this->x = roundf(this->x * 10) / 10;
		this->y = roundf(this->y * 10) / 10;
		this->z = roundf(this->z * 10) / 10;
	}

	void Vector3::Round2D()
	{
		this->x = roundf(this->x * 100) / 100;
		this->y = roundf(this->y * 100) / 100;
		this->z = roundf(this->z * 100) / 100;
	}

	void Vector3::Round(int value)
	{
		this->x = roundf(this->x * value) / value;
		this->y = roundf(this->y * value) / value;
		this->z = roundf(this->z * value) / value;
	}

	bool Vector3::IsZero()
	{
		return(this->x == 0 && this->y == 0 && this->z == 0);
	}

	bool Vector3::IsMaxed(float max)
	{
		return (this->x >= max && this->y >= max && this->z >= max);
	}

	void Vector3::Zero() { this->x = this->y = this->z = 0; }

	void Vector3::Truncate(float max)
	{
		if (this->Length() > max)
		{
			this->Normalise();
			*this = *this * max;
		}
	}
};

//more Vector Manipulation

inline float Vec3Dist(Vector3 v1, Vector3 v2)
{
	return sqrtf(((v1.x - v2.x) * (v1.x - v2.x)) +
				((v1.y - v2.y) * (v1.y - v2.y)) +
				((v1.z - v2.z) * (v1.z - v2.z)));
}

inline float Vec3DistSq(Vector3 v1, Vector3 v2)
{
	return ((v1.x - v2.x) * (v1.x - v2.x)) +
			((v1.y - v2.y) * (v1.y - v2.y)) +
			((v1.z - v2.z) * (v1.z - v2.z));
}

inline float Vec3Length(const Vector3& v1)
{
	return sqrtf((v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z));
}

inline float Vec3LengthSq(const Vector3& v1)
{
	return (v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z);
}

inline Vector3 Vec3Normalization(const Vector3& v1)
{
	float mag = Vec3Length(v1);
	if (mag == 0)
		return v1;

	return Vector3(v1.x, v1.y, v1.z) / mag;
}

inline float Dot(const Vector3& v1, const Vector3& v2)
{
	return ((v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z));
}

inline Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	float tempX = v1.y * v2.z - v1.z * v2.y;
	float tempY = v1.z * v2.x - v1.x * v2.z;
	float tempZ = v1.x * v2.y - v1.y * v2.x;

	return Vector3(tempX, tempY, tempZ);
}

inline Vector3 ClampVec3(Vector3 v1, float max, float min)
{
	if (v1.x > max) v1.x = max;
	if (v1.x < min) v1.x = min;

	if (v1.y > max) v1.y = max;
	if (v1.y < min) v1.y = min;

	if (v1.z > max) v1.z = max;
	if (v1.z < min) v1.z = min;

	return v1;
}

inline float ClampF(float val, float max, float min)
{
	if (val > max) val = max;
	if (val < min) val = min;

	return val;
}

inline float RandomClamped()
{
	float temp = (float)(rand() % 200 - 100);
	return temp /= 100.0f;
}

inline Vector3 MaxVec3(const Vector3& v1, const Vector3& v2)
{
	if (v1.x + v1.y + v1.z > v2.x + v2.y, v2.z)
	{
		return v1;
	}
	return v2;
}

inline Vector3 MinVec3(const Vector3& v1, const Vector3& v2)
{
	if (v1.x + v1.y + v1.z < v2.x + v2.y, v2.z)
	{
		return v1;
	}
	return v2;
}

//move vector overloading
inline Vector3 operator*(const float& scalar, const Vector3& v1)
{
	Vector3 v2;
	v2.x = scalar * v1.x;
	v2.y = scalar * v1.y;
	v2.z = scalar * v1.z;

	return v2;
}

inline bool operator !=(const Vector3 v1, const Vector3 v2)
{
	return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z);
}

//sqaure float values
inline float Sq(float value)
{
	return value * value;
}

inline XMFLOAT3 ToF3(Vector3 v1)
{
	return XMFLOAT3(v1.x, v1.y, v1.z);
}

inline Vector3 ToVec3(XMFLOAT3 v1)
{
	return Vector3(v1.x, v1.y, v1.z);
}

inline XMFLOAT4 ToF4(Vector3 v1, float w)
{
	return XMFLOAT4(v1.x, v1.y, v1.z, w);
}

//XMFLOAT4 VF4(XMVECTOR& vec)
//{
//	XMFLOAT4 val;
//	XMStoreFloat4(&val, vec);
//	return val;
//}
//
//XMFLOAT3 VF3(XMVECTOR& vec)
//{
//	XMFLOAT3 val;
//	XMStoreFloat3(&val, vec);
//	return val;
//}

struct Rect3
{
	Vector3 maxPoint;
	Vector3 minPoint;

	Rect3()
	{
		maxPoint = Vector3();
		minPoint = Vector3();
	}

	Rect3(Vector3 max, Vector3 min)
	{
		maxPoint = max;
		minPoint = min;
	}
};