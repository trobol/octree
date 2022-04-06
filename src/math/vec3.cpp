#include <octree/math/vec3.h>
#include <algorithm>
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

bool vec3::operator!=(const vec3& v)
{
	return x != v.x || y != v.y || z != v.z;
}

vec3::vec3(const vec3& v1)
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

std::ostream& operator<<(std::ostream& os, const vec3& v)
{
	os << v.x << ", " << v.y << ", " << v.z;
	return os;
}

float vec3::magnitude() const
{
	return sqrtf(x * x + y * y + z * z);
}

vec3 vec3::normalized() const
{
	float m = magnitude();
	return (*this) / m;
}

vec3 vec3::cross(const vec3& a, const vec3& b)
{

	return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float vec3::dot(const vec3& a, const vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3& vec3::operator*=(const vec3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}
vec3& vec3::operator/=(const vec3& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}
vec3& vec3::operator+=(const vec3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
vec3& vec3::operator-=(const vec3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

vec3& vec3::operator*=(const float& f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}
vec3& vec3::operator/=(const float& f)
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}
vec3& vec3::operator+=(const float& f)
{
	x += f;
	y += f;
	z += f;
	return *this;
}
vec3& vec3::operator-=(const float& f)
{
	x -= f;
	y -= f;
	z -= f;
	return *this;
}


float vec3::largest(vec3 v) {
	return std::max(v.x, std::max(v.y, v.z));
}
	
float vec3::smallest(vec3 v) {
	return std::min(v.x, std::min(v.y, v.z));
}