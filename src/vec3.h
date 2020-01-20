#ifndef _VEC3_H
#define _VEC3_H

class vec3
{
public:
	vec3(float x, float y, float z) : x{x}, y{y}, z{z} {};
	float x, y, z;
	vec3 operator+(const vec3 &v)
	{
		vec3 r = v;
		r.x += x;
		r.y += y;
		r.z += z;
		return r;
	};
	vec3 operator*(const vec3 &v)
	{
		vec3 r = v;
		r.x *= x;
		r.y *= y;
		r.z *= z;
		return r;
	};
	vec3 operator*(float f)
	{
		vec3 r = *this;
		r.x *= f;
		r.y *= f;
		r.z *= f;
		return r;
	}
};
#endif