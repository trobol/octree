#ifndef QUATERNION_H
#define QUATERNION_H

#include "vec3.h"
#include <emmintrin.h>

class Quaternion
{
public:
	Quaternion();
	Quaternion(float q0, float q1, float q2, float q3);

	Quaternion(vec3 eulerAngles);
	Quaternion(float pitch, float yaw, float roll);

	float x, y, z, w;

	vec3 ToEuler();
	void normalize();


	static Quaternion FromEuler(vec3 euler);
	static Quaternion AxisAngle(vec3 axis, float angle);
	static Quaternion identity;

	void toNormalized();
	Quaternion normalized() const;

	void operator+=(const Quaternion &q);
	//void operator-=(const Quaternion &q);
	void operator*=(const Quaternion &q);
	//void operator/=(const Quaternion &q);

	Quaternion operator+(const Quaternion &q) const;
	//Quaternion operator-(const Quaternion &q) const;
	Quaternion operator*(const Quaternion &q) const;
	//Quaternion operator/(const Quaternion &q) const;

	vec3 operator*(const vec3 &v) const;

	Quaternion inverse();
	void toInvert();

	static Quaternion lookRotation(vec3 const &eye, vec3 const &center, vec3 const &up);

private:
};

#endif