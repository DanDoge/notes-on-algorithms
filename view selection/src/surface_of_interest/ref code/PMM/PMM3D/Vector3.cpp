#include <math.h>
#include <float.h>

#include "Vector3.h"

Vector3::Vector3(void) : _x(0.0), _y(0.0), _z(0.0)
{
}

Vector3::Vector3(double x, double y, double z)
{
	_x = x;
	_y = y;
	_z = z;
}

Vector3::~Vector3(void)
{
}

Vector3 Vector3::cross(const Vector3& v)
{
	return Vector3(_y*v._z - _z*v._y, _z*v._x - _x*v._z, _x*v._y - _y*v._x);
}

Vector3 Vector3::operator^(const Vector3& v)
{
	return this->cross(v);
}

Vector3 Vector3::operator*(double scalar)
{
	return Vector3(scalar*_x, scalar*_y, scalar*_z);
}

Vector3 Vector3::operator/(double scalar)
{
	if(scalar != 0.0)
	{
		return Vector3(_x/scalar , _y/scalar, _z/scalar);
	}
	else
	{
		return Vector3(0.0, 0.0, 0.0);
	}
}

double Vector3::dot(const Vector3& v)
{
	return (_x*v._x + _y*v._y + _z*v._z);
}

double Vector3::operator*(const Vector3& v)
{
	return this->dot(v);
}

double Vector3::norm()
{
	return sqrt(_x*_x + _y*_y + _z*_z);
}

void Vector3::normalize()
{
	double n = norm();
	if(n > 0.0)
	{
		_x /= n;
		_y /= n;
		_z /= n;
	}
}

Vector3 Vector3::operator-(const Vector3& v)
{
	return Vector3(_x - v._x, _y - v._y, _z - v._z);
}

Vector3 Vector3::operator+(const Vector3& v)
{
	return Vector3(_x + v._x, _y + v._y, _z + v._z);
}

bool Vector3::isValid()
{
	return (!_isnan(_x) && !_isnan(_y) && !_isnan(_z) && _finite(_x) && _finite(_y) && _finite(_z));
}
