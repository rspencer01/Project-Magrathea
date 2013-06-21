#include "vector3.h"
#include <math.h>

Vector3::Vector3()
{
}
Vector3::Vector3(float _x,float _y,float _z)
{
	x = _x;
	y = _y;
	z = _z;
}
/*Vector3::Vector3(int _x,int _y,int _z)
{
	x = (float)_x;
	y = (float)_y;
	z = (float)_z;
}*/

Vector3 Vector3::operator=(const Vector3 & other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

Vector3 Vector3::operator+(Vector3 other)
{
	Vector3 result;
	result.x = x+other.x;
	result.y = y+other.y;
	result.z = z+other.z;
	return result;
}

Vector3 Vector3::operator-(Vector3 other)
{
	Vector3 result;
	result.x = x-other.x;
	result.y = y-other.y;
	result.z = z-other.z;
	return result;
}

Vector3 Vector3::operator*(float other)
{
	Vector3 result;
	result.x = x*other;
	result.y = y*other;
	result.z = z*other;
	return result;
}

float Vector3::dot(Vector3 other)
{
	return x*other.x+y*other.y+z*other.z;
}

float Vector3::norm()
{
	return sqrt(x*x+y*y+z*z);
}

void Vector3::normalise()
{
	float n = norm();
	x/=n;
	y/=n;
	z/=n;
}