
#ifndef _MATH_VEC3_H
#define _MATH_VEC3_H

#include <emmintrin.h>
#include <iostream>
class vec2;

class vec3
{
public:
	vec3(float x1, float y1, float z1);

	//vec3(vec2 v2, float z1);

	vec3() {};

	vec3(const vec3& v);
	union {
		struct {float x,y,z;};
		__m128 mm;
	};

	static vec3 one, left, right, up, down, forwards, backwards;

	static vec3 cross(const vec3& a, const vec3& b);

	static float dot(const vec3& a, const vec3& b);

	// largest component
	static float largest(vec3 v);
	// smallest component
	static float smallest(vec3 v);

	float magnitude() const;
	vec3 normalized() const;

	friend vec3 operator*(const float& f, const vec3& v);

	bool operator!=(const vec3& v);

	vec3& operator*=(const vec3& v1);
	vec3& operator/=(const vec3& v1);
	vec3& operator+=(const vec3& v1);
	vec3& operator-=(const vec3& v1);

	vec3& operator*=(const float& i);
	vec3& operator/=(const float& i);
	vec3& operator+=(const float& i);
	vec3& operator-=(const float& i);
};

std::ostream& operator<<(std::ostream& os, const vec3& v);

inline vec3 operator*(vec3 v0, const vec3& v1)
{
	v0 *= v1;
	return v0;
}
inline vec3 operator/(vec3 v0, const vec3& v1)
{
	v0 /= v1;
	return v0;
}
inline vec3 operator+(vec3 v0, const vec3& v1)
{
	v0 += v1;
	return v0;
}
inline vec3 operator-(vec3 v0, const vec3& v1)
{
	v0 -= v1;
	return v0;
}

inline vec3 operator*(vec3 v, const float& i)
{
	v *= i;
	return v;
}
inline vec3 operator/(vec3 v, const float& i)
{
	v /= i;
	return v;
}
inline vec3 operator+(vec3 v, const float& i)
{
	v += i;
	return v;
}
inline vec3 operator-(vec3 v, const float& i)
{
	v -= i;
	return v;
}

#endif // !VEC3