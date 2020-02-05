#include "transform.h"
#include "../math/math.h"
#include <math.h>

mat4 Transform::ToMatrix()
{
	mat4 m;
	m.Scale(scale);

	m.Rotate(rotation);
	m.Translate(position);

	return m;
}

vec3 Transform::getUp()
{
	return rotation * vec3::forwards;
}

void Transform::rotateAroundOrigin(float angle)
{
	float x = position.x * cosf(angle) - position.z * sin(angle);
	float z = position.z * cosf(angle) + position.x * sin(angle);
	position.x = x;
	position.z = z;
}

void Transform::rotateAround(vec3 point, float angle)
{
	vec3 diff = position - point;
	float x = diff.x * cosf(angle) - diff.z * sin(angle);
	float z = diff.z * cosf(angle) + diff.x * sin(angle);
	position.x = point.x + x;
	position.z = point.z + z;
}

void Transform::lookAt(vec3 point)
{
	/*
		x pitch
		y yaw
		z roll
	*/
	vec3 forward = (point - position).normalized();
	vec3 zeroRight = vec3::cross(vec3::up, forward).normalized();
	vec3 zeroUp = vec3::cross(forward, zeroRight);

	float pitch = asin(-forward.y);
	float yaw = atan2(forward.x, forward.z);

	vec3 right = vec3::cross(vec3(0, 1, 0), forward).normalized();
	vec3 orthonormalUp = vec3::cross(forward, right);

	float cosThetaZ = vec3::dot(zeroUp, orthonormalUp);
	float largestRight;
	if (fabs(right.x) > fabs(right.z)) {
		if (fabs(right.x) > fabs(right.y)) {
			largestRight = right.x;
		}
		else {
			largestRight = right.y;
		}
	}
	else {
		if (fabs(right.z) > fabs(right.y)) {
			largestRight = right.z;
		}
		else {
			largestRight = right.y;
		}
	}
	float sinThetaZ = (zeroUp.x * cosThetaZ - orthonormalUp.x) / largestRight;
	float roll = asin(sinThetaZ);


	rotation = Quaternion(0, yaw, 0);
	//rotation =	rightRot *(inverse oldRot * worldRot) * oldRot
	//atan2(dist.y, abs(dist.z)); //atan2(dist.y, sqrt(dist.y * dist.y + dist.y * dist.y));
}