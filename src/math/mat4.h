
#ifndef _MAT4_H
#define _MAT4_H
#include "vec3.h"
#include "quaternion.h"
#include <xmmintrin.h>

class mat4
{
public:
	mat4();
	mat4(
		float, float, float, float,
		float, float, float, float,
		float, float, float, float,
		float, float, float, float);
	mat4(Quaternion q);
	//matrix constuctor
	mat4(float m[16]);

	//copy constructor
	mat4(const mat4& m);

	union {
		__m128 column[4];
		float matrix[16] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };
		struct
		{
			float m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33;
		};
	};

	static unsigned int size;

	void SetValue(int i, int j, float vS);
	float GetValue(int x, int y);

	void Rotate(Quaternion q);
	void Rotate(vec3 axis, float angle);
	void Translate(vec3 v);
	void Translate(vec2 v);

	void Scale(vec3 v);

	static const mat4 identity;
	static mat4 FromAxisAngle(vec3 axis, float angle);
	static mat4 EulerAngle(vec3 v);
	static mat4 PerspectiveProj(float fov, float aspectRatio, float nearClip, float farClip);

	static mat4 lookAt(vec3 const& eye, vec3 const& center, vec3 const& up);

	mat4 operator*(const mat4& m);
	mat4& operator*=(const mat4& m);

	mat4& operator=(const mat4& m);
	float* operator[](int i);

	float getDeterminant();

	static const mat4 left, right, up, down;
	operator float* ()
	{
		return matrix;
	};
};

#endif