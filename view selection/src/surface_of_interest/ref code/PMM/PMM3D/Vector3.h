#pragma once

class Vector3
{
public:
	Vector3(void);
	Vector3(double x, double y, double z);
	Vector3 operator/(double scalar);
	Vector3 operator*(double scalar);
	Vector3 operator^(const Vector3& v);
	Vector3 operator-(const Vector3& v);
	Vector3 operator+(const Vector3& v);
	Vector3 cross(const Vector3& v);
	double dot(const Vector3& v);
	double operator*(const Vector3& v);
	double norm();
	void normalize();
	bool isValid();

public:
	~Vector3(void);

public:
	double _x, _y, _z;
};
