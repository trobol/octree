
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

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
	double cy = cos(eulerAngles.z * 0.5);
	double sy = sin(eulerAngles.z * 0.5);
	double cp = cos(eulerAngles.y * 0.5);
	double sp = sin(eulerAngles.y * 0.5);
	double cr = cos(eulerAngles.x * 0.5);
	double sr = sin(eulerAngles.x * 0.5);

	w = cy * cp * cr + sy * sp * sr;
	x = cy * cp * sr - sy * sp * cr;
	y = sy * cp * sr + cy * sp * cr;
	z = sy * cp * cr - cy * sp * sr;
}

Quaternion::Quaternion(float pitch, float yaw, float roll) : Quaternion(vec3(pitch, yaw, roll)) {}

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

Quaternion Quaternion::normalized() const
{
	if (w + x + y + z == 1)
		return Quaternion(*this);

	float n = sqrtf(x * x + y * y + z * z + w * w);
	if (n == 0)
		return Quaternion(*this);

	return Quaternion(x / n, y / n, z / n, w / n);
}

void Quaternion::toNormalized()
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

vec3 Quaternion::operator*(const vec3 &v) const
{

	vec3 u(x, y, z);

	float s = w;

	return u * vec3::dot(u, v) * 2.0f + v * (s * s - vec3::dot(u, u)) + vec3::cross(u, v) * 2.0f * s;
}

Quaternion Quaternion::operator*(const Quaternion &q2) const
{
	//a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x
	//dot a.x * b.x + a.y * b.y + a.z * b.z
	return Quaternion(
		x * q2.w + y * q2.z - z * q2.y + w * q2.x,
		-x * q2.z + y * q2.w + z * q2.x + w * q2.y,
		x * q2.y - y * q2.x + z * q2.w + w * q2.z,
		-x * q2.x - y * q2.y - z * q2.z + w * q2.w);
}

Quaternion Quaternion::operator+(const Quaternion &q) const
{
	return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}

/*
Quaternion Quaternion::lookRotation(vec3 const &target, vec3 const &current, vec3 const &eye, vec3 const &up)
{

	// turn vectors into unit vectors
	vec3 n1 = (current - eye).normalized();
	vec3 n2 = (target - eye).normalized();
	float d = vec3::dot(n1, n2);
	// if no noticable rotation is available return zero rotation
	// this way we avoid Cross product artifacts
	if (d > 0.9998)
		return Quaternion(0, 0, 1, 0);
	// in this case there are 2 lines on the same axis
	if (d < -0.9998)
	{
		n1 = n1.Rotx(0.5f);
		// there are an infinite number of normals
		// in this case. Anyone of these normals will be
		// a valid rotation (180 degrees). so rotate the curr axis by 0.5 radians this way we get one of these normals
	}
	vec3 axis = n1;
	vec3::cross(axis, n2);
	sfquat pointToTarget = new sfquat(1.0 + d, axis.x, axis.y, axis.z);
	pointToTarget.norm();
	// now twist around the target vector, so that the 'up' vector points along the z axis
	sfmatrix projectionMatrix = new sfmatrix();
	double a = pointToTarget.x;
	double b = pointToTarget.y;
	double c = pointToTarget.z;
	projectionMatrix.m00 = b * b + c * c;
	projectionMatrix.m01 = -a * b;
	projectionMatrix.m02 = -a * c;
	projectionMatrix.m10 = -b * a;
	projectionMatrix.m11 = a * a + c * c;
	projectionMatrix.m12 = -b * c;
	projectionMatrix.m20 = -c * a;
	projectionMatrix.m21 = -c * b;
	projectionMatrix.m22 = a * a + b * b;
	sfvec3f upProjected = projectionMatrix.transform(up);
	sfvec3f yaxisProjected = projectionMatrix.transform(new sfvec(0,1,0);
    d = sfvec3f.dot(upProjected,yaxisProjected);
    // so the axis of twist is n2 and the angle is arcos(d)
    //convert this to quat as follows   
	double s=Math.sqrt(1.0 - d*d);
	sfquat twist=new sfquat(d,n2*s,n2*s,n2*s);
	return sfquat.mul(pointToTarget,twist);
}
*/

/*
Quaternion Quaternion::inverse()
{
	float absoluteValue = norm();
	absoluteValue *= absoluteValue;
	absoluteValue = 1 / absoluteValue;

	R4DQuaternion conjugateValue = conjugate();

	float scalar = conjugateValue.s * absoluteValue;
	R4DVector3n imaginary = conjugateValue.v * absoluteValue;

	return R4DQuaternion(scalar, imaginary);
}
*/