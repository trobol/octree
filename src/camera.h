#ifndef _CAMERA_H
#define _CAMERA_H
#include "vec3.h"
#include "mat4.h"
class Camera {
public:
	Camera() : mPos{ 0, 0, 0 }, mRot{ 0, 0, 0 }, mFov{ 90 } {};
	Camera(vec3 pos, vec3 rot, float fov) : mPos{ pos }, mRot{ rot }, mFov{ fov } {};

	vec3 mPos;
	vec3 mRot;
	float mFov;

	mat4 getProjMatrix() {
		mat4 result;

	}
private:

};

#endif