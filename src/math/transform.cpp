#include "transform.h"

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
	vec3 dist = position - point;
	float yaw = -atan2(dist.x, dist.z);
	//rotation = Quaternion::AxisAngle(vec3::up, );
	Quaternion rightRot = Quaternion::AxisAngle(vec3::right, atan2(dist.y, yaw) * 0.5);
	Quaternion upRot = Quaternion::AxisAngle(vec3::up, yaw);
	rotation = upRot;
	//rotation =	rightRot *(inverse oldRot * worldRot) * oldRot
	//atan2(dist.y, abs(dist.z)); //atan2(dist.y, sqrt(dist.y * dist.y + dist.y * dist.y));
}