#include <octree/math/vec3int.h>

#include <math.h>

vec3int::vec3int(int x, int y, int z) : x{ x }, y{ y }, z{ z } {};

vec3 vec3int::toFloat()
{
	return vec3((float)x, (float)y, (float)z);
}

/*
vec3int::vec3int(vec2 v1, float z1)
{
	x = v1.x;
	y = v1.y;
	z = z1;
}
*/

vec3int::vec3int(const vec3int& v1)
{
	x = v1.x;
	y = v1.y;
	z = v1.z;
}
vec3int vec3int::one = vec3int(1, 1, 1);
vec3int vec3int::down = vec3int(0, -1, 0);
vec3int vec3int::up = vec3int(0, 1, 0);
vec3int vec3int::right = vec3int(1, 0, 0);
vec3int vec3int::left = vec3int(-1, 0, 0);
vec3int vec3int::forwards = vec3int(0, 0, 1);
vec3int vec3int::backwards = vec3int(0, 0, -1);

std::ostream& operator<<(std::ostream& os, const vec3int& v)
{
	os << v.x << ", " << v.y << ", " << v.z;
	return os;
}

std::istream& operator>>(std::istream& is, vec3int& v)
{
	is >> v.x >> v.y >> v.z;
	return is;
}

float vec3int::magnitude()
{
	return sqrtf((float)(x * x + y * y + z * z));
}

/*
vec3int vec3int::normalized()
{
	float m = magnitude();
	return (*this) / m;
}
*/

vec3int vec3int::cross(const vec3int& a, const vec3int& b)
{

	return vec3int(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float vec3int::dot(const vec3int& a, const vec3int& b)
{
	return (float)(a.x * b.x + a.y * b.y + a.z * b.z);
}

vec3int& vec3int::operator*=(const vec3int& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}
vec3int& vec3int::operator/=(const vec3int& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}
vec3int& vec3int::operator+=(const vec3int& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
vec3int& vec3int::operator-=(const vec3int& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

vec3int& vec3int::operator*=(const int& i)
{
	x *= i;
	y *= i;
	z *= i;
	return *this;
}
vec3int& vec3int::operator/=(const int& i)
{
	x /= i;
	y /= i;
	z /= i;
	return *this;
}
vec3int& vec3int::operator+=(const int& i)
{
	x += i;
	y += i;
	z += i;
	return *this;
}
vec3int& vec3int::operator-=(const int& i)
{
	x -= i;
	y -= i;
	z -= i;
	return *this;
}
