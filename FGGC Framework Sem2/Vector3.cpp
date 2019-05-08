#include "Vector3.h"
#include <string>

Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}
Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::~Vector3()
{
}

//Operator Overloading
////////////////////////////////////////////////////////
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
Vector3 Vector3::Normalization(const Vector3& v1)
{
	float mag = Magnitude(v1);

	Vector3 v2 = v2 / mag;
	return v2;
}

float Vector3::Magnitude(const Vector3& v1)
{
	float temp = sqrt((v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z));
	return temp;
}

Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2)
{
	float tempX = v1.y * v2.z - v2.y * v1.z;
	float tempY = v2.x * v1.z - v1.x * v2.z;
	float tempZ = v1.x * v2.y - v2.x * v1.y;

	return { tempX, tempY ,tempZ };
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
	return ((v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z));
}

float Vector3::Dist(const Vector3 & v1, const Vector3 & v2)
{
	float temp = sqrt(((v1.x - v2.x) * (v1.x - v2.x)) +
					((v1.y - v2.y) * (v1.y - v2.y)) + 
					((v1.z - v2.z) * (v1.z - v2.z)));
	return temp;
}
