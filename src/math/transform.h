#ifndef _TRANSFORM_H
#define _TRANSFORM_H
#include "vec3.h"
#include "quaternion.h"
#include "mat4.h"

class Transform
{
public:
	vec3 position;
	Quaternion rotation;
	vec3 scale;
	mat4 ToMatrix();

	vec3 getForwards();
	vec3 getBackwards();
	vec3 getUp();
	vec3 getDown();
	vec3 getLeft();
	vec3 getRight();

	void rotateAround(vec3 point, float angle);
	void rotateAroundOrigin(float angle);
	void rotateAroundDistance(vec3 point, float distance);
	void lookAt(vec3 point);
	

private:
};
#endif