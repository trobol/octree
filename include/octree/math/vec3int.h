
#ifndef _MATH_VEC3_INT_H
#define _MATH_VEC3_INT_H

#include <iostream>
#include "vec3.h"

struct vec3int
{
public:
	vec3int(int x, int y, int z);

	//vec3(vec2 v2, float z1);

	vec3int(){};

	vec3int(const vec3int &v);
	int x = 0, y = 0, z = 0;

	static vec3int one, left, right, up, down, forwards, backwards;

	static vec3int cross(const vec3int &a, const vec3int &b);

	static float dot(const vec3int &a, const vec3int &b);

	float magnitude();
	vec3int normalized();

	vec3 toFloat();

	vec3int &operator*=(const vec3int &v1);
	vec3int &operator/=(const vec3int &v1);
	vec3int &operator+=(const vec3int &v1);
	vec3int &operator-=(const vec3int &v1);

	vec3int &operator*=(const int &i);
	vec3int &operator/=(const int &i);
	vec3int &operator+=(const int &i);
	vec3int &operator-=(const int &i);
};

std::ostream &operator<<(std::ostream &os, const vec3int &v);
std::istream &operator>>(std::istream &is, vec3int &v);

inline vec3int operator*(vec3int v0, const vec3int &v1)
{
	v0 *= v1;
	return v0;
}
inline vec3int operator/(vec3int v0, const vec3int &v1)
{
	v0 /= v1;
	return v0;
}
inline vec3int operator+(vec3int v0, const vec3int &v1)
{
	v0 += v1;
	return v0;
}
inline vec3int operator-(vec3int v0, const vec3int &v1)
{
	v0 -= v1;
	return v0;
}

inline vec3int operator*(vec3int v, const int &i)
{
	v *= i;
	return v;
}
inline vec3int operator/(vec3int v, const int &i)
{
	v /= i;
	return v;
}
inline vec3int operator+(vec3int v, const int &i)
{
	v += i;
	return v;
}
inline vec3int operator-(vec3int v, const int &i)
{
	v -= i;
	return v;
}
#endif // !VEC3