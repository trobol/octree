#include "vec3.h"

#include <math.h>

vec3::vec3(float x1, float y1, float z1)
{
	x = x1;
	y = y1;
	z = z1;
}

/*
vec3::vec3(vec2 v1, float z1)
{
	x = v1.x;
	y = v1.y;
	z = z1;
}
*/

vec3::vec3(const vec3 &v1)
{
	x = v1.x;
	y = v1.y;
	z = v1.z;
}
vec3 vec3::one = vec3(1, 1, 1);
vec3 vec3::down = vec3(0, -1, 0);
vec3 vec3::up = vec3(0, 1, 0);
vec3 vec3::right = vec3(1, 0, 0);
vec3 vec3::left = vec3(-1, 0, 0);
vec3 vec3::forwards = vec3(0, 0, 1);
vec3 vec3::backwards = vec3(0, 0, -1);

float vec3::magnitude()
{
	return sqrtf(x * x + y * y + z * z);
}

vec3 vec3::normalized()
{
	float m = magnitude();
	return (*this) / m;
}

vec3 vec3::cross(const vec3 &a, const vec3 &b)
{

	return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float vec3::dot(const vec3 &a, const vec3 &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 operator*(const float &f, const vec3 &v)
{
	return vec3(f * v.x, f * v.y, f * v.z);
}

vec3 vec3::operator*(const vec3 &v)
{
	return vec3(x * v.x, y * v.y, z * v.z);
}

vec3 vec3::operator/(vec3 const &v)
{
	return vec3(x / v.x, y / v.y, z / v.z);
}

vec3 vec3::operator+(vec3 const &v)
{
	return vec3(x + v.x, y + v.y, z + v.z);
}

vec3 vec3::operator-(vec3 const &v)
{
	return vec3(x - v.x, y - v.y, z - v.z);
}

vec3 vec3::operator*(float const &f)
{
	return vec3(x * f, y * f, z * f);
}

vec3 vec3::operator/(float const &f)
{
	return vec3(x / f, y / f, z / f);
}
vec3 vec3::operator+(float const &f)
{
	return vec3(x + f, y + f, z + f);
}
vec3 vec3::operator-(float const &f)
{
	return vec3(x - f, y - f, z - f);
}

/*
vec3 vec3::operator*(int const &i)
{
	return vec3(x * i, y * i, z * i);
}

vec3 vec3::operator/(int const &i)
{
	return vec3(x / i, y / i, z / i);
}
vec3 vec3::operator+(int const &i)
{
	return vec3(x + i, y + i, z + i);
}
vec3 vec3::operator-(int const &i)
{
	return vec3(x - i, y - i, z - i);
}
*/