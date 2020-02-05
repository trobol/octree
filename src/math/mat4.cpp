
#include "math.h"
#include "mat4.h"
#include <math.h>

//SSE and AVX from https://gist.github.com/rygorous/4172889

unsigned int mat4::size = 64;

mat4::mat4()
{
}

// linear combination:
// a[0] * B.row[0] + a[1] * B.row[1] + a[2] * B.row[2] + a[3] * B.row[3]
static inline __m128 lincomb_SSE(const mat4& A, const __m128& b)
{
	__m128 result;
	result = _mm_mul_ps(_mm_shuffle_ps(b, b, 0x00), A.column[0]);
	result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(b, b, 0x55), A.column[1]));
	result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(b, b, 0xaa), A.column[2]));
	result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(b, b, 0xff), A.column[3]));
	return result;
}

// this is the right approach for SSE ... SSE4.2
// out = A * B
static inline void matmult_SSE(mat4& out, const mat4& A, const mat4& B)
{
	// out_ij = sum_k a_ik b_kj
	// => out_0j = a_00 * b_0j + a_01 * b_1j + a_02 * b_2j + a_03 * b_3j
	__m128 out0x = lincomb_SSE(A, B.column[0]);
	__m128 out1x = lincomb_SSE(A, B.column[1]);
	__m128 out2x = lincomb_SSE(A, B.column[2]);
	__m128 out3x = lincomb_SSE(A, B.column[3]);

	out.column[0] = out0x;
	out.column[1] = out1x;
	out.column[2] = out2x;
	out.column[3] = out3x;
}

mat4::mat4(float m[16])
{
	for (int i = 0; i < 16; i++)
	{
		matrix[i] = m[i];
	}
}

mat4 mat4::lookAt(vec3 const& eye, vec3 const& center, vec3 const& up)
{
	vec3  f = (center - eye).normalized();
	vec3  u = up.normalized();
	vec3  s = vec3::cross(f, u).normalized();
	u = vec3::cross(s, f);

	mat4 Result;
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = -f.x;
	Result[1][2] = -f.y;
	Result[2][2] = -f.z;
	Result[3][0] = -vec3::dot(s, eye);
	Result[3][1] = -vec3::dot(u, eye);
	Result[3][2] = vec3::dot(f, eye);
	return Result;
}

//https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
mat4::mat4(Quaternion q)
{

	q.toNormalized();

	float xx = q.x * q.x;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float xw = q.x * q.w;

	float yy = q.y * q.y;
	float yz = q.y * q.z;
	float yw = q.y * q.w;

	float zz = q.z * q.z;
	float zw = q.z * q.w;

	/*
		|0,  4,  8,  12|
		|1,  5,  9,  13|
		|2,  6,  10, 14|
		|3,  7,  11, 15|
		*/

	matrix[0] = 1 - 2 * (yy + zz);
	matrix[1] = 2 * (xy - zw);
	matrix[2] = 2 * (xz + yw);

	matrix[4] = 2 * (xy + zw);
	matrix[5] = 1 - 2 * (xx + zz);
	matrix[6] = 2 * (yz - xw);

	matrix[8] = 2 * (xz - yw);
	matrix[9] = 2 * (yz + xw);
	matrix[10] = 1 - 2 * (xx + yy);

	matrix[15] = 1;
}

mat4::mat4(const mat4& m)
{

	for (int i = 0; i < 4; i++)
		column[i] = m.column[i];
}

mat4& mat4::operator=(const mat4& m)
{

	for (int i = 0; i < 4; i++)
		_mm_store_ps(&matrix[i * 4], m.column[i]);

	return *this;
}

void mat4::Rotate(Quaternion q)
{

	(*this) = mat4(q) * (*this);
}
/*
	|0,  4,  8,  12|
	|1,  5,  9,  13|
	|2,  6,  10, 14|
	|3,  7,  11, 15|
	*/

float identityMat[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1 };

const mat4 mat4::identity = mat4(identityMat);

void mat4::SetValue(int i, int j, float v)
{
	matrix[i * 4 + j] = v;
}
float mat4::GetValue(int i, int j)
{
	return matrix[i * 4 + j];
}

mat4 mat4::operator*(const mat4& m)
{

	mat4 result;
	matmult_SSE(result, *this, m);
	return result;
}

mat4& mat4::operator*=(const mat4& m)
{

	mat4 m2 = m;

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			float value = 0;
			for (int i = 0; i < 4; i++)
			{
				value += m2[i][y] * m.matrix[x * 4 + i];
			}
			(*this)[x][y] = value;
		}
	}
	return *this;
}

float* mat4::operator[](int i)
{
	return &matrix[i * 4];
}

mat4 mat4::FromAxisAngle(vec3 axis, float angle)
{

	Quaternion q = Quaternion::AxisAngle(axis, angle);
	return mat4(q);
}

void mat4::Translate(vec3 v)
{

	/*
		|0,  4,  8,  12|
		|1,  5,  9,  13|
		|2,  6,  10, 14|
		|3,  7,  11, 15|
		*/

	union {
		__m128 vector;
		float f[4];
	};

	f[0] = v.x;
	f[1] = v.y;
	f[2] = v.z;
	f[3] = 0;

	column[0] = _mm_add_ps(column[0], _mm_mul_ps(_mm_set1_ps(matrix[3]), vector));
	column[1] = _mm_add_ps(column[1], _mm_mul_ps(_mm_set1_ps(matrix[7]), vector));
	column[2] = _mm_add_ps(column[2], _mm_mul_ps(_mm_set1_ps(matrix[11]), vector));
	column[3] = _mm_add_ps(column[3], _mm_mul_ps(_mm_set1_ps(matrix[15]), vector));
}
/*
void mat4::Translate(vec2 v)
{
	mat4 t;
	t[3][0] = v.x;
	t[3][1] = v.y;
	(*this) *= t;
}
*/
void mat4::Scale(vec3 v)
{
	(*this)[0][0] *= v.x;
	(*this)[1][1] *= v.y;
	(*this)[2][2] *= v.z;
}

mat4 mat4::PerspectiveProj(float fov, float aspect,
	float znear, float zfar)
{

	float xymax = znear * tan(fov * PI / 360);
	float ymin = -xymax;
	float xmin = -xymax;

	float width = xymax - xmin;
	float height = xymax - ymin;

	float depth = zfar - znear;
	float q = -(zfar + znear) / depth;
	float qn = -2 * (zfar * znear) / depth;

	float w = 2 * znear / width;
	w = w / aspect;
	float h = 2 * znear / height;

	return mat4(
		w,
		0.,
		0.,
		0.,

		0.,
		h,
		0.,
		0.,

		0.,
		0.,
		q,
		-1.,

		0.,
		0.,
		qn,
		0.);
}

mat4::mat4(float m0, float m1, float m2, float m3,
	float m4, float m5, float m6, float m7,
	float m8, float m9, float m10, float m11,
	float m12, float m13, float m14, float m15)
{

	/*
	|0,  4,  8,  12|
	|1,  5,  9,  13|
	|2,  6,  10, 14|
	|3,  7,  11, 15|
	*/

	matrix[0] = m0;
	matrix[4] = m1;
	matrix[8] = m2;
	matrix[12] = m3;

	matrix[1] = m4;
	matrix[5] = m5;
	matrix[9] = m6;
	matrix[13] = m7;

	matrix[2] = m8;
	matrix[6] = m9;
	matrix[10] = m10;
	matrix[14] = m11;

	matrix[3] = m12;
	matrix[7] = m13;
	matrix[11] = m14;
	matrix[15] = m15;
}

const mat4 mat4::left = {
	0, 0, -1, 0,
	0, 1, 0, 0,
	1, 0, 0, 0,
	0, 0, 0, 1 };

const mat4 mat4::right = {
	0, 0, 1, 0,
	0, 1, 0, 0,
	-1, 0, 0, 0,
	0, 0, 0, 1 };

const mat4 mat4::up = {
	1, 0, 0, 0,
	0, 0, -1, 0,
	0, 1, 0, 0,
	0, 0, 0, 1 };

const mat4 mat4::down = {
	1, 0, 0, 0,
	0, 0, 1, 0,
	0, -1, 0, 0,
	0, 0, 0, 1 };
