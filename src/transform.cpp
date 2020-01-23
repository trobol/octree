#include "transform.h"
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