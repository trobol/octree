
#ifndef VEC3_H

#define VEC3_H
#pragma once

class vec2;

class vec3
{
public:
	vec3(float x1, float y1, float z1);

	//vec3(vec2 v2, float z1);

	vec3(){};

	vec3(const vec3 &v);
	float x = 0, y = 0, z = 0;

	static vec3 one, left, right, up, down, forwards, backwards;

	static vec3 cross(const vec3 &a, const vec3 &b);

	static float dot(const vec3 &a, const vec3 &b);

	float magnitude();
	vec3 normalized();

	friend vec3 operator*(const float &f, const vec3 &v);

	vec3 operator*(const vec3 &v);
	vec3 operator/(const vec3 &v);
	vec3 operator+(const vec3 &v);
	vec3 operator-(const vec3 &v1);

	vec3 operator*(float const &f);
	vec3 operator/(float const &f);
	vec3 operator+(float const &f);
	vec3 operator-(float const &f);

	/*
	vec3 operator*(int const &i);
	vec3 operator/(int const &i);
	vec3 operator+(int const &i);
	vec3 operator-(int const &i);
	*/
};

#endif // !VEC3