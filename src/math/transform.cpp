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