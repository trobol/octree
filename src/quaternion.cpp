
#define _USE_MATH_DEFINES
#include <math.h>

#include "quaternion.h"
#include "mat4.h"

Quaternion Quaternion::identity = Quaternion(0, 0, 0, 1);

Quaternion::Quaternion(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion::Quaternion(vec3 eulerAngles)
{
	float cy = cosf(eulerAngles.z * 0.5f);
	float sy = sinf(eulerAngles.z * 0.5f);
	float cp = cosf(eulerAngles.y * 0.5f);
	float sp = sinf(eulerAngles.y * 0.5f);
	float cr = cosf(eulerAngles.x * 0.5f);
	float sr = sinf(eulerAngles.x * 0.5f);

	w = cy * cp * cr + sy * sp * sr;
	x = cy * cp * sr - sy * sp * cr;
	y = sy * cp * sr + cy * sp * cr;
	z = sy * cp * cr - cy * sp * sr;
}

vec3 Quaternion::EulerAngle()
{
	vec3 v;
	// roll (x-axis rotation)
	float sinr_cosp = 2.0f * (w * x + y * z);
	float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
	v.x = atan2f(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float sinp = 2.0f * (w * y - z * x);
	if (fabs(sinp) >= 1)
		v.y = copysignf(M_PI / 2.0f, sinp); // use 90 degrees if out of range
	else
		v.y = asinf(sinp);

	// yaw (z-axis rotation)
	float siny_cosp = 2.0f * (w * z + x * y);
	float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
	v.z = atan2f(siny_cosp, cosy_cosp);

	return v;
}

Quaternion::Quaternion()
{
	x = 0;
	y = 0;
	z = 0;
	w = 1;
}

Quaternion Quaternion::AxisAngle(vec3 axis, float angle)
{
	float a = angle / 2,
		  sa = sin(a);
	axis.normalized();
	return Quaternion(
		axis.x * sa,
		axis.y * sa,
		axis.z * sa,
		cosf(a));
}

void Quaternion::normalize()
{
	if (w + x + y + z == 1)
		return;

	float n = sqrtf(x * x + y * y + z * z + w * w);
	if (n == 0)
		return;
	x /= n;
	y /= n;
	z /= n;
	w /= n;
}

vec3 Quaternion::operator*(const vec3 &v)
{

	vec3 u(x, y, z);

	float s = w;

	return 2.0f * vec3::dot(u, v) * u + (s * s - vec3::dot(u, u)) * v + 2.0f * s * vec3::cross(u, v);
}
